#include "RhinoEncoder.h"

#include "Logger.h"

#include "prtx/EncoderInfoBuilder.h"
#include "prtx/GenerateContext.h"
#include "prtx/Exception.h"
#include "prtx/Geometry.h"
#include "prtx/Mesh.h"
#include "prtx/ReportsCollector.h"
#include "prtx/Shape.h"
#include "prtx/ShapeIterator.h"
#include "prtx/prtx.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#define DEBUG

namespace {

	const wchar_t* EO_BASE_NAME = L"baseName";
	const wchar_t* EO_ERROR_FALLBACK = L"errorFallback";
	const std::wstring ENCFILE_EXT = L".txt";
	const wchar_t* EO_EMIT_REPORTS = L"emitReport";
	const wchar_t* EO_EMIT_GEOMETRY = L"emitGeometry";
	const wchar_t* EO_EMIT_MATERIALS = L"emitMaterials";

	const prtx::EncodePreparator::PreparationFlags ENC_PREP_FLAGS =
		prtx::EncodePreparator::PreparationFlags()
		.instancing(false)
		.triangulate(true)
		.mergeVertices(false)
		.cleanupUVs(false)
		.cleanupVertexNormals(false)
		.mergeByMaterial(true);

	std::vector<const wchar_t*> toPtrVec(const prtx::WStringVector& wsv) {
		std::vector<const wchar_t*> pw(wsv.size());
		for (size_t i = 0; i < wsv.size(); i++)
			pw[i] = wsv[i].c_str();
		return pw;
	}

	std::wstring getTexturePath(const prtx::TexturePtr& t) {
		return t->getURI()->getPath();
	}

	void convertMaterialToAttributeMap(
				prtx::PRTUtils::AttributeMapBuilderPtr amb,
				const prtx::Material& prtxAttr, 
				const prtx::WStringVector& keys)
	{
#ifdef DEBUG
		LOG_DBG << L"Converting material " << prtxAttr.name();
#endif

		for (const auto& key : keys) {
			//TODO: check for blacklisted attributes??? Why is that?

#ifdef DEBUG
			LOG_DBG << L"   key: " << key;
#endif

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
			case prt::Attributable::PT_STRING:
				const std::wstring& v = prtxAttr.getString(key);
				amb->setString(key.c_str(), v.c_str());
				break;
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
				const std::wstring texPath = getTexturePath(tex);
				amb->setString(key.c_str(), texPath.c_str());
				break;
			}
			case prtx::Material::PT_TEXTURE_ARRAY: {
				const auto& texArray = prtxAttr.getTextureArray(key);

				prtx::WStringVector texPaths(texArray.size());
				std::transform(texArray.begin(), texArray.end(), texPaths.begin(), getTexturePath);

				std::vector<const wchar_t*> pTexPaths = toPtrVec(texPaths);
				amb->setStringArray(key.c_str(), pTexPaths.data(), pTexPaths.size());
				break;
			}
			default:
#ifdef DEBUG
				LOG_DBG << L"Ignored attribute " << key;
#endif
			}
		}
	}

	const prt::AttributeMap* convertReportToAttributeMap(const prtx::ReportsPtr& r)
	{

		prtx::PRTUtils::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());

		for (const auto& b : r->mBools)
			amb->setBool(b.first->c_str(), b.second);
		for (const auto& f : r->mFloats)
			amb->setFloat(f.first->c_str(), f.second);
		for (const auto& s : r->mStrings)
			amb->setString(s.first->c_str(), s.second->c_str());

		return amb->createAttributeMap();
	}

	struct TextureUVMapping {
		std::wstring key;
		uint8_t      index;
		int8_t       uvSet;
	};

	/// For a first pass of material implementation, only a colormap is supported
	const std::vector<TextureUVMapping> TEXTURE_UV_MAPPINGS = []() -> std::vector<TextureUVMapping> {
		return {
			// shader key   | idx | uv set  | CGA key
			{ L"diffuseMap",   0,    0 },  // colormap
			/*{ L"bumpMap",      0,    1 },  // bumpmap
			{ L"diffuseMap",   1,    2 },  // dirtmap
			{ L"specularMap",  0,    3 },  // specularmap
			{ L"opacityMap",   0,    4 },  // opacitymap
			{ L"normalMap",    0,    5 }   // normalmap

	#if PRT_VERSION_MAJOR > 1
			,
			{ L"emissiveMap",  0,    6 },  // emissivemap
			{ L"occlusionMap", 0,    7 },  // occlusionmap
			{ L"roughnessMap", 0,    8 },  // roughnessmap
			{ L"metallicMap",  0,    9 }   // metallicmap
	#endif*/

		};
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
} // namespace

const std::wstring RhinoEncoder::ID = L"com.esri.rhinoprt.RhinoEncoder";
const std::wstring RhinoEncoder::NAME = L"Rhino Geometry and Report Encoder";
const std::wstring RhinoEncoder::DESCRIPTION = L"Encodes geometry and CGA report for Rhino.";


void RhinoEncoder::init(prtx::GenerateContext& context) {
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
	prtx::ReportsAccumulatorPtr reportsAccumulator{ prtx::SummarizingReportsAccumulator::create() };
	prtx::ReportingStrategyPtr reportsCollector{ prtx::AllShapesReportingStrategy::create(context, initialShapeIndex, reportsAccumulator) };

#ifdef DEBUG
	LOG_DBG << L"Starting leaf iteration";
#endif

	try {
		prtx::LeafIteratorPtr li = prtx::LeafIterator::create(context, initialShapeIndex);

		for (prtx::ShapePtr shape = li->getNext(); shape.get() != nullptr; shape = li->getNext()) {
			mEncodePreparator->add(context.getCache(), shape, initialShape.getAttributeMap());
		}
	}
	catch (std::exception& e) {
		LOG_ERR << e.what();
			
		mEncodePreparator->add(context.getCache(), initialShape, initialShapeIndex);
	}
	catch (...) {
		LOG_ERR << "Unknown exception while encoding geometry." << std::endl;

		mEncodePreparator->add(context.getCache(), initialShape, initialShapeIndex);
	}

	prtx::EncodePreparator::InstanceVector finalizedInstances;
	mEncodePreparator->fetchFinalizedInstances(finalizedInstances, ENC_PREP_FLAGS);
	convertGeometry(initialShape, finalizedInstances, cb);

	if (getOptions()->getBool(EO_EMIT_REPORTS)) {
		auto reports = reportsCollector->getReports();

		if (reports) {
			auto reportMap = convertReportToAttributeMap(reports);
			// a single report map by initial shape.
			cb->addReport(initialShapeIndex, reportMap);
		}
	}
}

void RhinoEncoder::convertGeometry(const prtx::InitialShape& initialShape,
								   const prtx::EncodePreparator::InstanceVector& instances,
								   IRhinoCallbacks* cb) 
{
	const bool emitMaterials = getOptions()->getBool(EO_EMIT_MATERIALS);

	uint32_t vertexIndexBase = 0;

	std::vector<double> vertexCoords;
	std::vector<uint32_t> faceIndices;
	std::vector<uint32_t> faceCounts;
	std::vector<uint32_t> shapeIDs;
	std::vector<const prt::AttributeMap*> matAttrMap;

	std::vector<prtx::DoubleVector> uvs;
	std::vector<prtx::IndexVector> uvCounts;
	std::vector<prtx::IndexVector> uvIndices;

	uint32_t maxNumUVSets = 0; // for now we support only 1 uv set.

	prtx::GeometryPtrVector geometries;
	std::vector<prtx::MaterialPtrVector> materials;

	prtx::PRTUtils::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());

	for (const auto& instance : instances) {

		const prtx::MeshPtrVector& meshes = instance.getGeometry()->getMeshes();
		const prtx::MaterialPtrVector& material = instance.getMaterials();

		vertexCoords.clear();
		faceIndices.clear();
		faceCounts.clear();

		for (size_t mi = 0; mi < meshes.size(); ++mi) {
			const prtx::MeshPtr& mesh = meshes.at(mi);
			const prtx::DoubleVector& verts = mesh->getVertexCoords();
			vertexCoords.insert(vertexCoords.end(), verts.begin(), verts.end());

			for (uint32_t fi = 0; fi < mesh->getFaceCount(); ++fi) {
				const uint32_t* vtxIdx = mesh->getFaceVertexIndices(fi);
				const uint32_t vtxCnt = mesh->getFaceVertexCount(fi);
				faceCounts.push_back(vtxCnt);
				for (uint32_t vi = 0; vi < vtxCnt; vi++)
					faceIndices.push_back(vtxIdx[vi] + vertexIndexBase);
			}
			vertexIndexBase += (uint32_t)verts.size() / 3;

			if (emitMaterials) {
				const prtx::MaterialPtr& mat = material.at(mi);
				const uint32_t requiredUVSetsByMaterial = scanValidTextures(mat);

				//for now, only 1 uv set is supported
				if (mesh->getUVSetsCount() > 0 && (bool)requiredUVSetsByMaterial) maxNumUVSets = 1;

				// copy first uv set data. 
				// TODO: For now only one is supported
				const uint32_t numUVSets = mesh->getUVSetsCount();
				const prtx::DoubleVector& uvs0 = (numUVSets > 0) ? mesh->getUVCoords(0) : EMPTY_UVS;
				const prtx::IndexVector faceUVCounts0 = (numUVSets > 0) ? mesh->getFaceUVCounts(0) : prtx::IndexVector(mesh->getFaceCount(), 0);

				convertMaterialToAttributeMap(amb, *(mat.get()), mat->getKeys());
				matAttrMap.push_back(amb->createAttributeMapAndReset());
			}
		}

		cb->addGeometry(instance.getInitialShapeIndex(), vertexCoords.data(), vertexCoords.size(),
			faceIndices.data(), faceIndices.size(), faceCounts.data(), faceCounts.size());
	}
}

void RhinoEncoder::finish(prtx::GenerateContext& context) {
	LOG_DBG << "In finish  function...";

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
	amb->setBool(EO_ERROR_FALLBACK, prtx::PRTX_TRUE);
	amb->setBool(EO_EMIT_GEOMETRY, prtx::PRTX_TRUE);
	amb->setBool(EO_EMIT_REPORTS, prtx::PRTX_TRUE);
	encoderInfoBuilder.setDefaultOptions(amb->createAttributeMap());

	return new RhinoEncoderFactory(encoderInfoBuilder.create());
}