/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021-2024 Esri R&D Center Zurich
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

#include "prt/Callbacks.h"
#include "prtx/PRTUtils.h"

class IRhinoCallbacks : public prt::Callbacks {
public:
	virtual ~IRhinoCallbacks() override = default;

	///< summary>
	/// Adds a geometry and corresponding materials.
	///</summary>
	///< param name='faceRanges'>The range of the reports</param>
	///< param name='faceRangeSize'>The number of ranges</param>
	///< param name='reports'>An array of reports</param>
	virtual void add(const size_t initialShapeIndex, const size_t instanceIndex, const double* vertexCoords,
	                 const size_t vertexCoordsCount, const double* normals, const size_t normalsCount,
	                 const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
	                 const size_t faceCountsCount, double const* const* uvs, size_t const* uvsSizes,
	                 uint32_t const* const* uvCounts, size_t const* uvCountsSizes, uint32_t const* const* uvIndices,
	                 size_t const* uvIndicesSizes, uint32_t uvSets, const uint32_t* faceRanges, size_t faceRangesSize,
	                 prt::AttributeMap const* const* materials, size_t matCount) = 0;

	virtual void addReport(const size_t initialShapeIndex, const prtx::PRTUtils::AttributeMapPtr reports) = 0;

	/**
	 * Writes an asset (e.g. in-memory texture) to an implementation-defined path. Assets with same uri will be assumed
	 * to contain identical data.
	 * 
	 * @param uri the original asset within the RPK
	 * @param fileName local fileName derived from the URI by the asset encoder. can be used to cache the asset.
	 * @param [out] result file system path of the locally cached asset. Expected to be valid for the whole process
	 * life-time.
	 */
	virtual void addAsset(const wchar_t* uri, const wchar_t* fileName, const uint8_t* buffer, size_t size,
	                      wchar_t* result, size_t& resultSize) = 0;
};