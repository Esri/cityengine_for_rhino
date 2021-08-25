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

#pragma once

#include "IRhinoCallbacks.h"

#include "GeneratedModel.h"
#include "Logger.h"
#include "MaterialAttribute.h"
#include "ReportAttribute.h"
#include "utils.h"

#include "prt/Callbacks.h"

#include <iostream>
#include <vector>

class RhinoCallbacks : public IRhinoCallbacks {
public:
	RhinoCallbacks() = delete;
	explicit RhinoCallbacks(const size_t initialShapeCount);
	virtual ~RhinoCallbacks() = default;

	// functions from IRhinoCallbacks

	void add(const size_t initialShapeIndex, const size_t instanceIndex, const double* vertexCoords,
	         const size_t vertexCoordsCount, const double* normals, const size_t normalsCount,
	         const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
	         const size_t faceCountsCount, double const* const* uvs, size_t const* uvsSizes,
	         uint32_t const* const* uvCounts, size_t const* uvCountsSizes, uint32_t const* const* uvIndices,
	         size_t const* uvIndicesSizes, uint32_t uvSets, const uint32_t* faceRanges, size_t faceRangesSize,
	         prt::AttributeMap const* const* materials, size_t matCount) override;

	void addReport(const size_t initialShapeIndex, const prtx::PRTUtils::AttributeMapPtr reports) override;

	void addAsset(const wchar_t* uri, const wchar_t* fileName, const uint8_t* buffer, size_t size, wchar_t* result,
	              size_t& resultSize) override;

	// local helper functions

	const std::vector<GeneratedModelPtr>& getModels() const;
	const Reporting::ReportMap& getReport(const size_t initialShapeIdx) const;

	// functions from prt::Callbacks

	prt::Status generateError(size_t isIndex, prt::Status status, const wchar_t* message) override;

	prt::Status assetError(size_t isIndex, prt::CGAErrorLevel level, const wchar_t* key, const wchar_t* uri,
	                       const wchar_t* message) override;

	prt::Status cgaError(size_t isIndex, int32_t shapeID, prt::CGAErrorLevel level, int32_t methodId, int32_t pc,
	                     const wchar_t* message) override;

	prt::Status cgaPrint(size_t isIndex, int32_t shapeID, const wchar_t* txt) override;

	prt::Status cgaReportBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, bool /*value*/) override;

	prt::Status cgaReportFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
	                           double /*value*/) override;

	prt::Status cgaReportString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
	                            const wchar_t* /*value*/) override;

	prt::Status attrBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, bool /*value*/) override;

	prt::Status attrFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, double /*value*/) override;

	prt::Status attrString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
	                       const wchar_t* /*value*/) override;

	prt::Status attrBoolArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const bool* /*ptr*/,
	                          size_t /*size*/, size_t nRows) override;

	prt::Status attrFloatArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const double* /*ptr*/,
	                           size_t /*size*/, size_t nRows) override;

	prt::Status attrStringArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
	                            const wchar_t* const* /*ptr*/, size_t /*size*/, size_t nRows) override;

private:
	GeneratedModel& getOrCreateModel(size_t initialShapeIndex);

	bool addGeometry(const size_t initialShapeIndex, const double* vertexCoords, const size_t vertexCoordsCount,
	                 const double* normals, const size_t normalsCount, const uint32_t* faceIndices,
	                 const size_t faceIndicesCount, const uint32_t* faceCounts, const size_t faceCountsCount);

	void addUVCoordinates(const size_t initialShapeIndex, double const* const* uvs, size_t const* uvsSizes,
	                      uint32_t const* const* uvCounts, size_t const* uvCountsSizes,
	                      uint32_t const* const* uvIndices, size_t const* uvIndicesSizes, uint32_t uvSets);

private:
	std::vector<GeneratedModelPtr> mModels;
};
