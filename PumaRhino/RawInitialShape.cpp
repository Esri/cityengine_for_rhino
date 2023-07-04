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

#include "Logger.h"
#include "RawInitialShape.h"

#include <algorithm>
#include <numeric>
#include <vector>

RawInitialShape::RawInitialShape(const ON_Mesh& mesh) {
	ON_wString shapeIdxStr;
	if (!mesh.GetUserString(INIT_SHAPE_ID_KEY.c_str(), shapeIdxStr)) {
		LOG_WRN << L"InitialShapeID not found in given mesh";
		mID = -1;
	}
	else {
		std::wstring str(shapeIdxStr.Array());

		// cast to int
		try {
			mID = std::stoi(str);
		}
		catch (std::invalid_argument) {
			LOG_ERR << "Mesh id string was not a number, so it could not be parsed -> setting initial shape id to -1.";
			mID = -1;
		}
		catch (std::out_of_range) {
			LOG_ERR << "Mesh id could not be parsed, setting initial shape id to 0.";
			mID = -1;
		}
	}

	ON_Mesh temp(mesh);
	int planarNgonsCount = temp.AddPlanarNgons(nullptr, 0.5, 3, 1, false);
	if (planarNgonsCount == 0)
		throw std::exception("Input mesh is not planar");

	ON_3dPointListRef refs = ON_3dPointListRef(&temp);
	ON_SimpleArray<ON_3dPoint> boundaryPoints(0);
	int boundaryPointsCount = temp.Ngon(0)->GetOuterBoundaryPoints(refs, false, boundaryPoints);
	if (boundaryPointsCount == 0)
		throw std::exception("Could not get boundary points");

	mVertices.reserve(static_cast<size_t>(boundaryPointsCount * 3));
	mIndices.reserve(static_cast<size_t>(boundaryPointsCount));
	mFaceCounts.reserve(1);

	for (int i = 0; i < boundaryPoints.Count(); ++i) {
		ON_3dPoint* vertex = boundaryPoints.At(i);
		mVertices.push_back(vertex->x);
		mVertices.push_back(vertex->z);
		mVertices.push_back(-vertex->y);

		mIndices.push_back(i);	
	}
	mIndices.push_back(0);
	mFaceCounts.push_back(boundaryPointsCount);
}

int RawInitialShape::getID() const {
	return mID;
}

const double* RawInitialShape::getVertices() const {
	return mVertices.data();
}

size_t RawInitialShape::getVertexCount() const {
	return mVertices.size();
}

const uint32_t* RawInitialShape::getIndices() const {
	return mIndices.data();
}

size_t RawInitialShape::getIndexCount() const {
	return mIndices.size();
}

const uint32_t* RawInitialShape::getFaceCounts() const {
	return mFaceCounts.data();
}

size_t RawInitialShape::getFaceCountsCount() const {
	return mFaceCounts.size();
}
