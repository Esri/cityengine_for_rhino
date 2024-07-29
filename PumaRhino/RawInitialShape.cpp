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

#include "RawInitialShape.h"
#include "Logger.h"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

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

	// 1:1 copy of all mesh coordinates
	const bool useDoublePrecision = (mesh.m_V.Count() > 0) && mesh.HasSynchronizedDoubleAndSinglePrecisionVertices();
	mVertices.reserve(static_cast<std::vector<double>::size_type>(mesh.m_V.Count()) * 3u);
	auto flipAndAdd = [this](double x, double y, double z) {
		mVertices.push_back(x);
		mVertices.push_back(z);
		mVertices.push_back(-y);	
	};
	if (useDoublePrecision) {
		for (int i = 0; i < mesh.m_dV.Count(); ++i) {
			const ON_3dPoint* vertex = mesh.m_dV.At(i);
			flipAndAdd(vertex->x, vertex->y, vertex->z);
		}
	}
	else {
		for (int i = 0; i < mesh.m_V.Count(); ++i) {
			const ON_3fPoint* vertex = mesh.m_V.At(i);
			flipAndAdd(vertex->x, vertex->y, vertex->z); // implicit conversion to double
		}
	}

	// handle Ngons (= "curated" set of quads/tris to form initial shape faces for PRT)
	const unsigned int ngonCount = mesh.NgonUnsignedCount();
	if (ngonCount > 0) {
		mFaceCounts.reserve(ngonCount);
		for (unsigned int ngonIdx = 0; ngonIdx < ngonCount; ngonIdx++) {
			const ON_MeshNgon* ngon = mesh.Ngon(ngonIdx);
			mIndices.reserve(mIndices.size() + ngon->m_Vcount);
			for (unsigned int vi = 0; vi < ngon->m_Vcount; ++vi) {
				mIndices.push_back(ngon->m_vi[vi]);
			}
			mFaceCounts.push_back(ngon->m_Vcount);
		}
	}
	
	// search for lonely faces not part of a Ngon
	const unsigned int faceCount = mesh.FaceCount();
	std::vector<unsigned int> lonelyFaces;
	lonelyFaces.reserve(faceCount); // pessimistic
	if (ngonCount > 0) {
		for (unsigned int fi = 0; fi < faceCount; fi++) {
			if (static_cast<int>(fi) >= mesh.m_NgonMap.Count() || mesh.m_NgonMap[fi] == ON_UNSET_UINT_INDEX)
				lonelyFaces.push_back(fi);
		}
	}
	// short cut: no ngons -> add all faces
	else {
		lonelyFaces.resize(faceCount);
		std::iota(lonelyFaces.begin(), lonelyFaces.end(), 0);
	}

	// create initial shape faces from the lonely ones
	const size_t lonelyFacesCount = lonelyFaces.size();
	if (lonelyFacesCount > 0) {
		mFaceCounts.reserve(mFaceCounts.size() + lonelyFacesCount);
		for (unsigned int lonelyFaceIndex : lonelyFaces) {
			const ON_MeshFace& face = mesh.m_F[lonelyFaceIndex];
			mIndices.push_back(face.vi[0]);
			mIndices.push_back(face.vi[1]);
			mIndices.push_back(face.vi[2]);
			if (face.IsQuad()) {
				mIndices.push_back(face.vi[3]);
				mFaceCounts.push_back(4);
			}
			else {
				mFaceCounts.push_back(3);
			}
		}
	}
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
