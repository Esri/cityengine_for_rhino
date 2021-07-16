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

#include "MaterialAttribute.h"
#include "ReportAttribute.h"
#include "RhinoCallbacks.h"
#include "utils.h"

#include <vector>

const std::wstring INIT_SHAPE_ID_KEY = L"InitShapeIdx";

class RawInitialShape {
public:
	RawInitialShape() = default;
	RawInitialShape(const ON_Mesh& mesh);
	~RawInitialShape() = default;

	int getID() const;
	const double* getVertices() const;
	size_t getVertexCount() const;
	const uint32_t* getIndices() const;
	size_t getIndexCount() const;
	const uint32_t* getFaceCounts() const;
	size_t getFaceCountsCount() const;

private:
	int mID;
	std::vector<double> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaceCounts;
};
