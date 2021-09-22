/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021 Esri R&D Center Zurich
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

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "RhinoCallbacks.h"
#include "AssetCache.h"
#include "PRTContext.h"

#include <filesystem>
#include <wchar.h>

namespace {

constexpr bool DBG = false;

const Reporting::ReportMap EMPTY_REPORT_MAP;

} // namespace

RhinoCallbacks::RhinoCallbacks(const size_t initialShapeCount) {
	mModels.resize(initialShapeCount);
}

bool RhinoCallbacks::addGeometry(const size_t initialShapeIndex, const double* vertexCoords,
                                 const size_t vertexCoordsCount, const double* normals, const size_t normalsCount,
                                 const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
                                 const size_t faceCountsCount) {
	if (vertexCoords == nullptr || normals == nullptr || faceIndices == nullptr || faceCounts == nullptr)
		return false;

	GeneratedModel& currentModel = *mModels[initialShapeIndex];
	ModelPart& modelPart = currentModel.addModelPart();

	modelPart.mVertices.assign(vertexCoords, vertexCoords + vertexCoordsCount);
	modelPart.mNormals.assign(normals, normals + normalsCount);
	modelPart.mIndices.assign(faceIndices, faceIndices + faceIndicesCount);
	modelPart.mFaces.assign(faceCounts, faceCounts + faceCountsCount);

	return true;
}

void RhinoCallbacks::addUVCoordinates(const size_t initialShapeIndex, double const* const* uvs, size_t const* uvsSizes,
                                      uint32_t const* const* uvCounts, size_t const* uvCountsSizes,
                                      uint32_t const* const* uvIndices, size_t const* uvIndicesSizes, uint32_t uvSets) {
	GeneratedModel& currentModel = *mModels[initialShapeIndex];
	ModelPart& modelPart = currentModel.getCurrentModelPart();

	// Add texture coordinates
	for (size_t uvSet = 0; uvSet < uvSets; ++uvSet) {
		size_t const psUVSSize = uvsSizes[uvSet];
		size_t const psUVCountsSize = uvCountsSizes[uvSet];
		size_t const psUVIndicesSize = uvIndicesSizes[uvSet];

		if (psUVSSize > 0 && psUVIndicesSize > 0 && psUVCountsSize > 0) {
			if (uvSet == 0) {
				if constexpr (DBG)
					LOG_DBG << " -- uvset " << uvSet << ": psUVCountsSize = " << psUVCountsSize
					        << ", psUVIndicesSize = " << psUVIndicesSize;

				double const* const psUVS = uvs[uvSet];
				uint32_t const* const psUVCounts = uvCounts[uvSet];
				uint32_t const* const psUVIndices = uvIndices[uvSet];

				modelPart.mUVIndices.reserve(psUVIndicesSize);
				modelPart.mUVs.Reserve(psUVIndicesSize);
				modelPart.mUVCounts.reserve(psUVCountsSize);

				modelPart.mUVCounts.insert(modelPart.mUVCounts.end(), psUVCounts + 0, psUVCounts + psUVCountsSize);

				for (size_t uvi = 0; uvi < psUVIndicesSize; ++uvi) {
					const uint32_t uvIdx = psUVIndices[uvi];
					const auto du = psUVS[uvIdx * 2 + 0];
					const auto dv = psUVS[uvIdx * 2 + 1];
					modelPart.mUVs.Append(ON_2fPoint(static_cast<float>(du), static_cast<float>(dv)));
					modelPart.mUVIndices.push_back(static_cast<uint32_t>(uvi));
				}
			}
			else {
				LOG_INF << "IGNORED UV SET " << uvSet << ": Rhino does not support multiple uv sets.";
			}
		}
	}
}

void RhinoCallbacks::add(const size_t initialShapeIndex, const size_t instanceIndex, const double* vertexCoords,
                         const size_t vertexCoordsCount, const double* normals, const size_t normalsCount,
                         const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
                         const size_t faceCountsCount, double const* const* uvs, size_t const* uvsSizes,
                         uint32_t const* const* uvCounts, size_t const* uvCountsSizes, uint32_t const* const* uvIndices,
                         size_t const* uvIndicesSizes, uint32_t uvSets, const uint32_t* /*faceRanges*/,
                         size_t /*faceRangesSize*/, prt::AttributeMap const* const* materials, const size_t matCount) {

	GeneratedModel& currentModel = getOrCreateModel(initialShapeIndex);

	if (!addGeometry(initialShapeIndex, vertexCoords, vertexCoordsCount, normals, normalsCount, faceIndices,
	                 faceIndicesCount, faceCounts, faceCountsCount))
		return;
	addUVCoordinates(initialShapeIndex, uvs, uvsSizes, uvCounts, uvCountsSizes, uvIndices, uvIndicesSizes, uvSets);

	// -- convert materials into material attributes
	if constexpr (DBG)
		LOG_DBG << "got " << matCount << " materials";
	if (matCount > 0 && materials != nullptr) {
		if (matCount > 1) {
			LOG_ERR << L"Multiple material for a single mesh part is not supported by Rhino. Taking only the first "
			           L"material"
			        << std::endl;
		}

		const prt::AttributeMap* attrMap = materials[0];
		auto ma = Materials::extractMaterials(instanceIndex, attrMap);
		currentModel.addMaterial(ma);
	}
}

void RhinoCallbacks::addReport(const size_t initialShapeIndex, const prtx::PRTUtils::AttributeMapPtr reports) {
	if constexpr (DBG)
		LOG_DBG << "In RhinoCallback::addReport";

	if (!reports) {
		LOG_WRN << "Trying to add null report, ignoring.";
		return;
	}

	if (mModels.size() <= initialShapeIndex) {
		LOG_ERR << "Shape index is bigger than the number of generated models.";
		return;
	}

	if (!mModels[initialShapeIndex])
		return;

	GeneratedModel& model = *mModels[initialShapeIndex];

	Reporting::extractReports(initialShapeIndex, model, reports);

	if constexpr (DBG)
		LOG_DBG << "End of RhinoCallback::addReport";
}

void RhinoCallbacks::addAsset(const wchar_t* uri, const wchar_t* fileName, const uint8_t* buffer, size_t size,
                              wchar_t* result,
                              size_t& resultSize) {
	if (uri == nullptr || std::wcslen(uri) == 0 || fileName == nullptr || std::wcslen(fileName) == 0) {
		LOG_WRN << "Skipping asset caching for invalid uri '" << uri << "' or filename '" << fileName << '"';
		resultSize = 0;
		return;
	}

	const std::filesystem::path& assetPath = PRTContext::get()->getAssetCache().put(uri, fileName, buffer, size);
	if (assetPath.empty()) {
		resultSize = 0;
		return;
	}

	const std::wstring pathStr = assetPath.wstring();

	if (resultSize <= pathStr.size()) {  // also check for null-terminator
		resultSize = pathStr.size() + 1; // ask for space for null-terminator
		return;
	}

	wcsncpy_s(result, resultSize, pathStr.c_str(), resultSize);
	result[resultSize - 1] = 0x0;
	resultSize = pathStr.length() + 1;
}

const std::vector<GeneratedModelPtr>& RhinoCallbacks::getModels() const {
	return mModels;
}

const Reporting::ReportMap& RhinoCallbacks::getReport(const size_t initialShapeIdx) const {
	if (initialShapeIdx >= mModels.size())
		throw std::out_of_range("initial shape index is out of range.");

	if (!mModels[initialShapeIdx])
		return EMPTY_REPORT_MAP;

	return mModels[initialShapeIdx]->getReports();
}

prt::Status RhinoCallbacks::generateError(size_t isIndex, prt::Status status, const wchar_t* message) {
	LOG_ERR << L"GENERATE ERROR:" << isIndex << " " << status << " " << message;
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::assetError(size_t isIndex, prt::CGAErrorLevel level, const wchar_t* key, const wchar_t* uri,
                                       const wchar_t* message) {
	GeneratedModel& model = getOrCreateModel(isIndex);

	if (message != nullptr) {
		auto msg = std::wstring(L"Asset Error: ").append(message);
		if (key != nullptr)
			msg.append(L"; CGA key = '").append(key).append(L"'");
		if (uri != nullptr)
			msg.append(L"; CGA URI = '").append(uri).append(L"'");
		model.addErrorOutput(msg);
	}

	LOG_ERR << L"ASSET ERROR:" << isIndex << " " << level << " " << key << " " << uri << " " << message << std::endl;
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::cgaError(size_t isIndex, int32_t shapeID, prt::CGAErrorLevel level, int32_t methodId,
                                     int32_t pc, const wchar_t* message) {
	GeneratedModel& model = getOrCreateModel(isIndex);

	if (message != nullptr) {
		auto msg = std::wstring(L"CGA Error: ").append(message);
		model.addErrorOutput(msg);
	}

	LOG_ERR << L"CGA ERROR:" << isIndex << " " << shapeID << " " << level << " " << methodId << " " << pc << " "
	        << message << std::endl;
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::cgaPrint(size_t isIndex, int32_t shapeID, const wchar_t* txt) {
	GeneratedModel& model = getOrCreateModel(isIndex);

	if (txt != nullptr)
		model.addPrintOutput(txt);

	LOG_INF << L"CGA PRINT:" << isIndex << " " << shapeID << " " << txt << std::endl;
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::cgaReportBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                          bool /*value*/) {
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::cgaReportFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                           double /*value*/) {
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::cgaReportString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                            const wchar_t* /*value*/) {
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::attrBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, bool /*value*/) {
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::attrFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                      double /*value*/) {
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::attrString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                       const wchar_t* /*value*/) {
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::attrBoolArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                          const bool* /*ptr*/, size_t /*size*/, size_t /*nRows*/) {
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::attrFloatArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                           const double* /*ptr*/, size_t /*size*/, size_t /*nRows*/) {
	return prt::STATUS_OK;
}

prt::Status RhinoCallbacks::attrStringArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                            const wchar_t* const* /*ptr*/, size_t /*size*/, size_t /*nRows*/) {
	return prt::STATUS_OK;
}

GeneratedModel& RhinoCallbacks::getOrCreateModel(size_t initialShapeIndex) {
	if (!mModels[initialShapeIndex])
		mModels[initialShapeIndex] = std::make_shared<GeneratedModel>();
	return *mModels[initialShapeIndex];
}
