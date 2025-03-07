/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
 *
 * Copyright (c) 2021-2025 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RhinoEncoder.h"

#include "TextureEncoder.h"

#include "prtx/DataBackend.h"
#include "prtx/EncodePreparator.h"
#include "prtx/EncoderInfoBuilder.h"
#include "prtx/Exception.h"
#include "prtx/GenerateContext.h"
#include "prtx/Geometry.h"
#include "prtx/Mesh.h"
#include "prtx/Log.h"
#include "prtx/PRTUtils.h"
#include "prtx/ReportsCollector.h"
#include "prtx/Shape.h"
#include "prtx/ShapeIterator.h"
#include "prtx/prtx.h"

#include "prt/MemoryOutputCallbacks.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>

#include <assert.h>

namespace {

constexpr bool ENC_DBG = false;

using MemoryOutputCallbacksUPtr = std::unique_ptr<prt::MemoryOutputCallbacks, prtx::PRTUtils::ObjectDestroyer>;

const wchar_t* EO_BASE_NAME = L"baseName";
const wchar_t* EO_ERROR_FALLBACK = L"errorFallback";
const std::wstring ENCFILE_EXT = L".txt";
const wchar_t* EO_EMIT_REPORTS = L"emitReport";
const wchar_t* EO_EMIT_GEOMETRY = L"emitGeometry";
const wchar_t* EO_EMIT_MATERIALS = L"emitMaterials";

const prtx::EncodePreparator::PreparationFlags ENC_PREP_FLAGS =
        prtx::EncodePreparator::PreparationFlags()
                .instancing(false)
                .triangulate(false)
                .mergeVertices(false)
                .cleanupUVs(true)
                .cleanupVertexNormals(true)
                .processVertexNormals(prtx::VertexNormalProcessor::SET_MISSING_TO_FACE_NORMALS)
                .indexSharing(prtx::EncodePreparator::PreparationFlags::INDICES_SAME_FOR_ALL_VERTEX_ATTRIBUTES)
                .meshMerging(prtx::MeshMerging::ALL_OF_SAME_MATERIAL_AND_TYPE)
                .processHoles(prtx::HoleProcessor::TRIANGULATE_FACES_WITH_HOLES);

std::vector<const wchar_t*> toPtrVec(const prtx::WStringVector& wsv) {
	std::vector<const wchar_t*> pw(wsv.size());
	for (size_t i = 0; i < wsv.size(); i++)
		pw[i] = wsv[i].c_str();
	return pw;
}

template <typename T>
std::pair<std::vector<const T*>, std::vector<size_t>> toPtrVec(const std::vector<std::vector<T>>& v) {
	std::vector<const T*> pv(v.size());
	std::vector<size_t> ps(v.size());
	for (size_t i = 0; i < v.size(); i++) {
		pv[i] = v[i].data();
		ps[i] = v[i].size();
	}
	return std::make_pair(pv, ps);
}

std::vector<const prt::AttributeMap*> toPtrVec(const std::vector<prtx::PRTUtils::AttributeMapUPtr>& managedPtrVec) {
	std::vector<const prt::AttributeMap*> rawPtrVec(managedPtrVec.size(), nullptr);
	std::transform(managedPtrVec.begin(), managedPtrVec.end(), rawPtrVec.begin(),
	               [](const auto& managed) { return managed.get(); });
	return rawPtrVec;
}

template <typename C, typename FUNC, typename OBJ, typename... ARGS>
std::basic_string<C> callAPI(FUNC f, OBJ& obj, ARGS&&... args) {
	std::vector<C> buffer(1024, 0x0);
	size_t size = buffer.size();
	std::invoke(f, obj, args..., buffer.data(), size);
	if (size == 0)
		return {}; // error case
	else if (size > buffer.size()) {
		buffer.resize(size);
		std::invoke(f, obj, args..., buffer.data(), size);
	}
	return {buffer.data()};
}

std::wstring getTexturePath(const prtx::TexturePtr& texture, IRhinoCallbacks* callbacks, prt::Cache* cache) {
	if (!texture || !texture->isValid())
		return {};

	const prtx::URIPtr& uri = texture->getURI();
	const std::wstring& uriStr = uri->wstring();
	const std::wstring& scheme = uri->getScheme();

	if (!uri->isComposite() && (scheme == prtx::URI::SCHEME_FILE || scheme == prtx::URI::SCHEME_UNC)) {
		// textures from the local file system or a mounted share on Windows can be directly passed to Rhino
		return uri->getNativeFormat();
	}
	else if (uri->isComposite() && (scheme == prtx::URI::SCHEME_RPK)) {
		// textures from within an RPK can be directly copied out, no need for encoding
		// just need to make sure we have useful filename for embedded texture blocks without names

		const prtx::BinaryVectorPtr data = prtx::DataBackend::resolveBinaryData(cache, uriStr);
		const std::wstring fileName = uri->getBaseName() + uri->getExtension();
		const std::wstring assetPath = callAPI<wchar_t>(&IRhinoCallbacks::addAsset, *callbacks, uriStr.c_str(),
		                                                fileName.c_str(), data->data(), data->size());
		return assetPath;
	}
	else {
		// all other textures (builtin or from memory) need to be extracted and potentially re-encoded
		try {
			MemoryOutputCallbacksUPtr moc(prt::MemoryOutputCallbacks::create());

			prtx::AsciiFileNamePreparator namePrep;
			const prtx::NamePreparator::NamespacePtr& namePrepNamespace = namePrep.newNamespace();
			const std::wstring validatedFilename =
			        TextureEncoder::encode(texture, moc.get(), namePrep, namePrepNamespace, {});

			if (moc->getNumBlocks() == 1) {
				size_t bufferSize = 0;
				const uint8_t* buffer = moc->getBlock(0, &bufferSize);
				const std::wstring assetPath = callAPI<wchar_t>(&IRhinoCallbacks::addAsset, *callbacks, uriStr.c_str(),
				                                                validatedFilename.c_str(), buffer, bufferSize);
				if (!assetPath.empty())
					return assetPath;
				else
					log_warn("Received invalid asset path while trying to write asset with URI: %1%") % uriStr;
			}
		}
		catch (std::exception& e) {
			log_warn("Failed to encode or write texture at %1% to the local filesystem: %2%") % uriStr % e.what();
		}
	}

	return {};
}

// we blacklist all CGA-style material attribute keys, see prtx/Material.h
const std::set<std::wstring> MATERIAL_ATTRIBUTE_BLACKLIST = {
        L"ambient.b",
        L"ambient.g",
        L"ambient.r",
        L"bumpmap.rw",
        L"bumpmap.su",
        L"bumpmap.sv",
        L"bumpmap.tu",
        L"bumpmap.tv",
        L"color.a",
        L"color.b",
        L"color.g",
        L"color.r",
        L"color.rgb",
        L"colormap.rw",
        L"colormap.su",
        L"colormap.sv",
        L"colormap.tu",
        L"colormap.tv",
        L"dirtmap.rw",
        L"dirtmap.su",
        L"dirtmap.sv",
        L"dirtmap.tu",
        L"dirtmap.tv",
        L"normalmap.rw",
        L"normalmap.su",
        L"normalmap.sv",
        L"normalmap.tu",
        L"normalmap.tv",
        L"opacitymap.rw",
        L"opacitymap.su",
        L"opacitymap.sv",
        L"opacitymap.tu",
        L"opacitymap.tv",
        L"specular.b",
        L"specular.g",
        L"specular.r",
        L"specularmap.rw",
        L"specularmap.su",
        L"specularmap.sv",
        L"specularmap.tu",
        L"specularmap.tv",
        L"bumpmap",
        L"colormap",
        L"dirtmap",
        L"normalmap",
        L"opacitymap",
        L"specularmap"

        //#if PRT_VERSION_MAJOR > 1
        // also blacklist CGA-style PBR attrs from CE 2019.0, PRT 2.x
        //,
        L"opacitymap.mode",
        L"emissive.b",
        L"emissive.g",
        L"emissive.r",
        L"emissivemap.rw",
        L"emissivemap.su",
        L"emissivemap.sv",
        L"emissivemap.tu",
        L"emissivemap.tv",
        L"metallicmap.rw",
        L"metallicmap.su",
        L"metallicmap.sv",
        L"metallicmap.tu",
        L"metallicmap.tv",
        L"occlusionmap.rw",
        L"occlusionmap.su",
        L"occlusionmap.sv",
        L"occlusionmap.tu",
        L"occlusionmap.tv",
        L"roughnessmap.rw",
        L"roughnessmap.su",
        L"roughnessmap.sv",
        L"roughnessmap.tu",
        L"roughnessmap.tv",
        L"emissivemap",
        L"metallicmap",
        L"occlusionmap",
        L"roughnessmap"
        //#endif
};

void convertMaterialToAttributeMap(prtx::PRTUtils::AttributeMapBuilderPtr amb, const prtx::Material& prtxAttr,
                                   const prtx::WStringVector& keys, IRhinoCallbacks* cb, prt::Cache* cache) {
	if constexpr (ENC_DBG)
		log_debug("Converting material %1%") % prtxAttr.name();

	for (const auto& key : keys) {
		if (MATERIAL_ATTRIBUTE_BLACKLIST.count(key) > 0)
			continue;

		if constexpr (ENC_DBG)
			log_debug("key: %1%") % key;

		switch (prtxAttr.getType(key)) {
			case prt::Attributable::PT_BOOL:
				amb->setBool(key.c_str(), prtxAttr.getBool(key) == prtx::PRTX_TRUE);
				break;
			case prt::Attributable::PT_FLOAT:
				amb->setFloat(key.c_str(), prtxAttr.getFloat(key));
				break;
			case prt::Attributable::PT_INT:
				amb->setInt(key.c_str(), prtxAttr.getInt(key));
				break;
			case prt::Attributable::PT_STRING: {
				const std::wstring& v = prtxAttr.getString(key);
				amb->setString(key.c_str(), v.c_str());
				break;
			}
			case prt::Attributable::PT_BOOL_ARRAY: {
				const prtx::BoolVector& ba = prtxAttr.getBoolArray(key);
				auto boo = std::unique_ptr<bool[]>(new bool[ba.size()]);
				for (size_t i = 0; i < ba.size(); ++i) {
					boo[i] = (ba[i] == prtx::PRTX_TRUE);
				}
				amb->setBoolArray(key.c_str(), boo.get(), ba.size());
				break;
			}
			case prt::Attributable::PT_INT_ARRAY: {
				const auto& array = prtxAttr.getIntArray(key);
				amb->setIntArray(key.c_str(), &array[0], array.size());
				break;
			}
			case prt::AttributeMap::PT_FLOAT_ARRAY: {
				const std::vector<double>& array = prtxAttr.getFloatArray(key);
				amb->setFloatArray(key.c_str(), array.data(), array.size());
				break;
			}
			case prt::Attributable::PT_STRING_ARRAY: {
				const prtx::WStringVector& strVect = prtxAttr.getStringArray(key);
				std::vector<const wchar_t*> pVect = toPtrVec(strVect);
				amb->setStringArray(key.c_str(), pVect.data(), pVect.size());
				break;
			}
			case prtx::Material::PT_TEXTURE: {

				const auto& tex = prtxAttr.getTexture(key);
				const std::wstring texPath = getTexturePath(tex, cb, cache);
				if (texPath.length() > 0) {
					if constexpr (ENC_DBG)
						log_debug("Using getTexture with key: %1% : %2%") % key % texPath;
					amb->setString(key.c_str(), texPath.c_str());
				}
				break;
			}
			case prtx::Material::PT_TEXTURE_ARRAY: {
				if constexpr (ENC_DBG)
					log_debug("Texture array with key: %1%") % key;

				const auto& texArray = prtxAttr.getTextureArray(key);

				prtx::WStringVector texPaths;
				texPaths.reserve(texArray.size());
				for (const auto& tex : texArray) {
					const std::wstring texPath = getTexturePath(tex, cb, cache);
					if (!texPath.empty())
						texPaths.push_back(texPath);
				}

				if (texPaths.size() > 0) {
					std::vector<const wchar_t*> pTexPaths = toPtrVec(texPaths);
					amb->setStringArray(key.c_str(), pTexPaths.data(), pTexPaths.size());
				}

				break;
			}
			default: {
				if constexpr (ENC_DBG)
					log_debug("Ignored attribute %1%") % key;
				continue;
			}
		}
	}
}

const prtx::PRTUtils::AttributeMapPtr convertReportToAttributeMap(const prtx::ReportsPtr& r) {
	prtx::PRTUtils::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());

	for (const auto& b : r->mBools)
		amb->setBool(b.first->c_str(), b.second);
	for (const auto& f : r->mFloats)
		amb->setFloat(f.first->c_str(), f.second);
	for (const auto& s : r->mStrings)
		amb->setString(s.first->c_str(), s.second->c_str());

	return prtx::PRTUtils::AttributeMapPtr{amb->createAttributeMap()};
}

struct TextureUVMapping {
	std::wstring key;
	uint8_t index;
	int8_t uvSet;
};

const std::vector<TextureUVMapping> TEXTURE_UV_MAPPINGS = []() -> std::vector<TextureUVMapping> {
	// clang-format off
	return {
	        // shader key   | idx | uv set  | CGA key
	        {L"diffuseMap", 0, 0},  // colormap
	        {L"bumpMap", 0, 1},     // bumpmap
	        {L"diffuseMap", 1, 2},  // dirtmap
	        {L"specularMap", 0, 3}, // specularmap
	        {L"opacityMap", 0, 4},  // opacitymap
	        {L"normalMap", 0, 5}    // normalmap
	        /*
	    #if PRT_VERSION_MAJOR > 1
	            ,
	            { L"emissiveMap",  0,    6 },  // emissivemap
	            { L"occlusionMap", 0,    7 },  // occlusionmap
	            { L"roughnessMap", 0,    8 },  // roughnessmap
	            { L"metallicMap",  0,    9 }   // metallicmap
	    #endif*/

	};
	// clang-format on
}();

// return the highest required uv set (where a valid texture is present)
uint32_t scanValidTextures(const prtx::MaterialPtr& mat) {
	int8_t highestUVSet = -1;
	for (const auto& t : TEXTURE_UV_MAPPINGS) {
		const auto& ta = mat->getTextureArray(t.key);
		if (ta.size() > t.index && ta[t.index]->isValid())
			highestUVSet = std::max(highestUVSet, t.uvSet);
	}
	if (highestUVSet < 0)
		return 0;
	else
		return highestUVSet + 1;
}

const prtx::DoubleVector EMPTY_UVS;
const prtx::IndexVector EMPTY_IDX;

auto scanMeshes(const prtx::MeshPtrVector& meshes) {
	uint32_t numCoords = 0;
	uint32_t numNormalCoords = 0;
	uint32_t numFaceCounts = 0;
	uint32_t numIndices = 0;

	for (const auto& mesh : meshes) {
		numCoords += static_cast<uint32_t>(mesh->getVertexCoords().size());
		numNormalCoords += static_cast<uint32_t>(mesh->getVertexNormalsCoords().size());
		numFaceCounts += static_cast<uint32_t>(mesh->getFaceCount());

		const auto& vtxCnts = mesh->getFaceVertexCounts();
		numIndices = std::accumulate(vtxCnts.begin(), vtxCnts.end(), numIndices);
	}

	return std::make_tuple(numCoords, numNormalCoords, numFaceCounts, numIndices);
}

} // namespace

const std::wstring RhinoEncoder::ID = L"com.esri.rhinoprt.RhinoEncoder";
const std::wstring RhinoEncoder::NAME = L"Rhino Geometry and Report Encoder";
const std::wstring RhinoEncoder::DESCRIPTION = L"Encodes geometry and CGA report for Rhino.";

void RhinoEncoder::init(prtx::GenerateContext&) {
	prtx::NamePreparator::NamespacePtr nsMaterials = mNamePreparator.newNamespace();
	prtx::NamePreparator::NamespacePtr nsMeshes = mNamePreparator.newNamespace();
	mEncodePreparator = prtx::EncodePreparator::create(true, mNamePreparator, nsMeshes, nsMaterials);
}

void RhinoEncoder::encode(prtx::GenerateContext& context, size_t initialShapeIndex) {

	const prtx::InitialShape& initialShape = *context.getInitialShape(initialShapeIndex);

	auto* cb = dynamic_cast<IRhinoCallbacks*>(getCallbacks());
	if (cb == nullptr)
		throw prtx::StatusException(prt::STATUS_ILLEGAL_CALLBACK_OBJECT);

	// Initialization of report accumulator and strategy
	prtx::ReportsAccumulatorPtr reportsAccumulator{prtx::SummarizingReportsAccumulator::create()};
	prtx::ReportingStrategyPtr reportsCollector{
	        prtx::AllShapesReportingStrategy::create(context, initialShapeIndex, reportsAccumulator)};

	if constexpr (ENC_DBG)
		log_debug("Starting leaf iteration");

	try {
		prtx::LeafIteratorPtr li = prtx::LeafIterator::create(context, initialShapeIndex);

		for (prtx::ShapePtr shape = li->getNext(); shape.get() != nullptr; shape = li->getNext()) {
			mEncodePreparator->add(context.getCache(), shape, initialShape.getAttributeMap());
		}
	}
	catch (std::exception& e) {
		log_error("Caught exception: %1%") % e.what();

		mEncodePreparator->add(context.getCache(), initialShape, initialShapeIndex);
	}
	catch (...) {
		log_error("Unknown exception while encoding geometry.");

		mEncodePreparator->add(context.getCache(), initialShape, initialShapeIndex);
	}

	prtx::EncodePreparator::InstanceVector finalizedInstances;
	mEncodePreparator->fetchFinalizedInstances(finalizedInstances, ENC_PREP_FLAGS);
	convertGeometry(initialShape, finalizedInstances, cb, context.getCache());

	if (getOptions()->getBool(EO_EMIT_REPORTS)) {
		const prtx::ReportsPtr& reports = reportsCollector->getReports();

		if (reports) {
			auto reportMap = convertReportToAttributeMap(reports);
			// a single report map by initial shape.
			cb->addReport(initialShapeIndex, reportMap);
		}
	}
}

void RhinoEncoder::convertGeometry(const prtx::InitialShape&, const prtx::EncodePreparator::InstanceVector& instances,
                                   IRhinoCallbacks* cb, prt::Cache* cache) {
	bool emitMaterials = getOptions()->getBool(EO_EMIT_MATERIALS);

	uint32_t vertexIndexBase = 0;
	uint32_t maxNumUVSets = 0;
	std::vector<uint32_t> uvIndexBases(maxNumUVSets, 0u);

	std::vector<double> vertexCoords;
	prtx::DoubleVector normals;
	std::vector<uint32_t> faceIndices;
	std::vector<uint32_t> faceCounts;
	std::vector<prtx::PRTUtils::AttributeMapUPtr> matAttrMap;

	uint32_t faceCount = 0;
	std::vector<uint32_t> faceRanges;

	std::vector<prtx::DoubleVector> uvs;
	std::vector<prtx::IndexVector> uvCounts;
	std::vector<prtx::IndexVector> uvIndices;

	prtx::PRTUtils::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());

	size_t instanceIndex = 0;
	for (const auto& instance : instances) {

		const prtx::MeshPtrVector& meshes = instance.getGeometry()->getMeshes();
		const prtx::MaterialPtrVector& materials = instance.getMaterials();

		if constexpr (ENC_DBG) {
			size_t mesh_count = meshes.size();
			log_debug("Material count for instance %1%: %2%, meshes: %3%") % instance.getInitialShapeIndex() % materials.size() % mesh_count;
		}

		vertexIndexBase = 0;
		maxNumUVSets = 0;
		uvIndexBases.clear();
		vertexCoords.clear();
		normals.clear();
		faceIndices.clear();
		faceCounts.clear();
		matAttrMap.clear();
		uvs.clear();
		uvCounts.clear();
		uvIndices.clear();

		// 1st pass: scan the geometries to preallocate the sizes of vectors
		const auto [numCoords, numNormalCoords, numFaceCounts, numIndices] = scanMeshes(meshes);

		vertexCoords.reserve(3 * numCoords);
		normals.reserve(3 * numNormalCoords);
		faceCounts.reserve(numFaceCounts);
		faceIndices.reserve(numIndices);

		// 2nd pass: fill the vectors
		for (size_t mi = 0; mi < meshes.size(); ++mi) {
			const prtx::MeshPtr& mesh = meshes.at(mi);

			const prtx::DoubleVector& verts = mesh->getVertexCoords();
			vertexCoords.insert(vertexCoords.end(), verts.begin(), verts.end());

			const prtx::DoubleVector& norms = mesh->getVertexNormalsCoords();
			normals.insert(normals.end(), norms.begin(), norms.end());

			for (uint32_t fi = 0; fi < mesh->getFaceCount(); ++fi) {
				const uint32_t* vtxIdx = mesh->getFaceVertexIndices(fi);
				const uint32_t vtxCnt = mesh->getFaceVertexCount(fi);
				faceCounts.push_back(vtxCnt);

				for (uint32_t vi = 0; vi < vtxCnt; vi++) {
					faceIndices.push_back(vtxIdx[vi] + vertexIndexBase);
				}
			}
			vertexIndexBase += (uint32_t)verts.size() / 3;

			if (emitMaterials) {
				faceRanges.push_back(faceCount);
				faceCount += mesh->getFaceCount();

				const prtx::MaterialPtr& mat = materials.at(mi);
				const uint32_t requiredUVSetsByMaterial = scanValidTextures(mat);
				maxNumUVSets = std::max(maxNumUVSets, std::max(mesh->getUVSetsCount(), requiredUVSetsByMaterial));

				if (mesh->getUVSetsCount() > 0 && (bool)requiredUVSetsByMaterial) {
					uvIndexBases.resize(maxNumUVSets, 0u);
					uvs.resize(maxNumUVSets);
					uvCounts.resize(maxNumUVSets);
					uvIndices.resize(maxNumUVSets);
				}

				// copy first uv set data.
				const uint32_t numUVSets = mesh->getUVSetsCount();
				const prtx::DoubleVector& uvs0 = (numUVSets > 0) ? mesh->getUVCoords(0) : EMPTY_UVS;
				const prtx::IndexVector faceUVCounts0 =
				        (numUVSets > 0) ? mesh->getFaceUVCounts(0) : prtx::IndexVector(mesh->getFaceCount(), 0);

				if constexpr (ENC_DBG)
					log_debug("-- mesh: numUVSets = %1%") % numUVSets;

				if (numUVSets > 0) {
					for (uint32_t uvSet = 0; uvSet < uvs.size(); uvSet++) {
						// append texture coordinates
						const prtx::DoubleVector& currUVs = (uvSet < numUVSets) ? mesh->getUVCoords(uvSet) : EMPTY_UVS;
						const auto& src = currUVs.empty() ? uvs0 : currUVs;

						// insert the curent uvs into the corresponding position in the vector of uvs
						auto& tgt = uvs[uvSet];
						tgt.insert(tgt.end(), src.begin(), src.end());

						// append uv face counts
						const prtx::IndexVector& faceUVCounts =
						        (uvSet < numUVSets && !currUVs.empty()) ? mesh->getFaceUVCounts(uvSet) : faceUVCounts0;
						assert(faceUVCounts.size() == mesh->getFaceCount());
						auto& tgtCounts = uvCounts[uvSet];
						tgtCounts.insert(tgtCounts.end(), faceUVCounts.begin(), faceUVCounts.end());

						if constexpr (ENC_DBG)
							log_debug("  -- uvset %1%: face counts size = %2%") % uvSet % faceUVCounts.size();

						// append uv vertex indices
						for (uint32_t faceId = 0; faceId < static_cast<uint32_t>(faceUVCounts.size()); ++faceId) {
							const uint32_t* faceUVIdx0 =
							        (numUVSets > 0) ? mesh->getFaceUVIndices(faceId, 0) : EMPTY_IDX.data();
							const uint32_t* faceUVIdx = (uvSet < numUVSets && !currUVs.empty())
							                                    ? mesh->getFaceUVIndices(faceId, uvSet)
							                                    : faceUVIdx0;
							const uint32_t faceUVCnt = faceUVCounts[faceId];

							if constexpr (ENC_DBG)
								log_debug("      faceId %1%: faceUVCnt = %2%, faceVtxCnt = %3%") % faceId % faceUVCnt %  mesh->getFaceVertexCount(faceId);

							for (uint32_t vrtxId = 0; vrtxId < faceUVCnt; ++vrtxId) {
								uvIndices[uvSet].push_back(uvIndexBases[uvSet] + faceUVIdx[vrtxId]);
							}
						}

						uvIndexBases[uvSet] += static_cast<uint32_t>(src.size()) / 2u;
					}
				}
				convertMaterialToAttributeMap(amb, *(mat.get()), mat->getKeys(), cb, cache);
				matAttrMap.emplace_back(amb->createAttributeMapAndReset());
				if constexpr (ENC_DBG)
					log_debug("mat map: %1%") % prtx::PRTUtils::objectToXML(matAttrMap.back().get());
			}
		}
		faceRanges.push_back(faceCount);

		assert(matAttrMap.empty() || matAttrMap.size() == 1);

		assert(uvs.size() == uvCounts.size());
		assert(uvs.size() == uvIndices.size());

		auto puvs = toPtrVec(uvs);
		auto puvCounts = toPtrVec(uvCounts);
		auto puvIndices = toPtrVec(uvIndices);

		assert(uvs.size() == puvCounts.first.size());
		assert(uvs.size() == puvCounts.second.size());

		const std::vector<const prt::AttributeMap*> matAttrPtrs = toPtrVec(matAttrMap);

		cb->add(instance.getInitialShapeIndex(), instanceIndex, vertexCoords.data(), vertexCoords.size(),
		        normals.data(), normals.size(), faceIndices.data(), faceIndices.size(), faceCounts.data(),
		        faceCounts.size(),

		        puvs.first.data(), puvs.second.data(), puvCounts.first.data(), puvCounts.second.data(),
		        puvIndices.first.data(), puvIndices.second.data(), static_cast<uint32_t>(uvs.size()),

		        faceRanges.data(), faceRanges.size(), matAttrPtrs.data(), matAttrPtrs.size());

		instanceIndex++;
	}
}

void RhinoEncoder::finish(prtx::GenerateContext&) {
	if constexpr (ENC_DBG)
		log_debug("In finish  function...");
}

RhinoEncoderFactory* RhinoEncoderFactory::createInstance() {
	prtx::EncoderInfoBuilder encoderInfoBuilder;

	encoderInfoBuilder.setID(RhinoEncoder::ID);
	encoderInfoBuilder.setName(RhinoEncoder::NAME);
	encoderInfoBuilder.setDescription(RhinoEncoder::DESCRIPTION);
	encoderInfoBuilder.setType(prt::CT_GEOMETRY);
	encoderInfoBuilder.setExtension(ENCFILE_EXT);

	// Default encoder options
	prtx::PRTUtils::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());
	amb->setString(EO_BASE_NAME, L"enc_default_name");
	amb->setBool(EO_ERROR_FALLBACK, true);
	amb->setBool(EO_EMIT_GEOMETRY, true);
	amb->setBool(EO_EMIT_REPORTS, true);
	amb->setBool(EO_EMIT_MATERIALS, true);
	encoderInfoBuilder.setDefaultOptions(amb->createAttributeMap());

	return new RhinoEncoderFactory(encoderInfoBuilder.create());
}
