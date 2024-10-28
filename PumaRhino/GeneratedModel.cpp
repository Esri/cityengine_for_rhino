/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
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

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "GeneratedModel.h"
#include "RawInitialShape.h"
#include "Logger.h"

#include <cassert>

namespace {

ON_Mesh toON_Mesh(const ModelPart& modelPart, const std::wstring& idKey) {
	ON_Mesh mesh(static_cast<int>(modelPart.mFaces.size()), static_cast<int>(modelPart.mIndices.size()), true, true);

	for (size_t v_id = 0; v_id < modelPart.mIndices.size(); ++v_id) {
		auto index = modelPart.mIndices[v_id];
		mesh.SetVertex(static_cast<int>(v_id),
		               ON_3dPoint(modelPart.mVertices[index * 3], -modelPart.mVertices[index * 3 + 2],
		                          modelPart.mVertices[index * 3 + 1]));
		mesh.SetVertexNormal(static_cast<int>(v_id),
		                     ON_3dVector(modelPart.mNormals[index * 3], -modelPart.mNormals[index * 3 + 2],
		                                 modelPart.mNormals[index * 3 + 1]));
	}

	int faceid(0);
	int currindex(0);
	for (int faceVertexCount : modelPart.mFaces) {
		if (faceVertexCount == 3) {
			mesh.SetTriangle(faceid++, currindex, currindex + 1, currindex + 2);
		}
		else if (faceVertexCount == 4) {
			mesh.SetQuad(faceid++, currindex, currindex + 1, currindex + 2, currindex + 3);
		}
		else {
			ON_SimpleArray<int> triangleIndices((faceVertexCount - 2) * 3);
			triangleIndices.SetCount(triangleIndices.Capacity());
			int rc = RhinoTriangulate3dPolygon(faceVertexCount, 3, mesh.m_dV[currindex], 3, triangleIndices.Array());
			if (rc >= 0) {
				int triangle_count = triangleIndices.Count() / 3;
				for (int i = 0; i < triangle_count; i++) {
					mesh.SetTriangle(faceid++, currindex + triangleIndices[3 * i],
					                 currindex + triangleIndices[(3 * i) + 1],
					                 currindex + triangleIndices[(3 * i) + 2]);
				}
			}
			else {
				LOG_ERR << "Rhino failed to triangulate a generated polygon from shape " << idKey;
			}
		}
		currindex += faceVertexCount;
	}

	for (int i = 0; i < modelPart.mUVs.Count(); ++i) {
		mesh.SetTextureCoord(i, modelPart.mUVs[i].x, modelPart.mUVs[i].y);
	}

	mesh.Compact();
	mesh.SetUserString(INIT_SHAPE_ID_KEY.c_str(), idKey.c_str());

	// Printing a rhino error log if the created mesh is invalid
	ON_wString log_str;
	ON_TextLog log(log_str);
	if (!mesh.IsValid(&log)) {
		mesh.Dump(log);
		LOG_ERR << log_str;
	}

	return mesh;
}

} // namespace

ModelPart& GeneratedModel::addModelPart() {
	mModelParts.push_back(ModelPart());
	return mModelParts.back();
}

const std::vector<ModelPart>& GeneratedModel::getModelParts() const {
	return mModelParts;
}

int GeneratedModel::getMeshPartCount() const {
	return static_cast<int>(getModelParts().size());
}

ModelPart& GeneratedModel::getCurrentModelPart() {
	return mModelParts.back();
}

void GeneratedModel::addReport(const Reporting::ReportAttribute& ra) {
	mReports.emplace(ra.mReportName, ra);
}

const Reporting::ReportMap& GeneratedModel::getReports() const {
	return mReports;
}

const Materials::MaterialsMap& GeneratedModel::getMaterials() const {
	return mMaterials;
}

void GeneratedModel::addPrint(const std::wstring_view& message) {
	assert(message.length() > 0); // we expect at least the newline character added by PRT
	mPrints.emplace_back(message.substr(0, message.length() - 1)); // let's trim the newline away
}

const std::vector<std::wstring>& GeneratedModel::getPrints() const {
	return mPrints;
}

void GeneratedModel::addError(const std::wstring_view& error) {
	mErrors.emplace_back(error);
}

const std::vector<std::wstring>& GeneratedModel::getErrors() const {
	return mErrors;
}

void GeneratedModel::addMaterial(const Materials::MaterialAttribute& ma) {
	mMaterials.insert_or_assign(ma.mMatId, ma);
}

const GeneratedModel::MeshBundle GeneratedModel::createRhinoMeshes(size_t initialShapeIndex) const {
	if (mModelParts.empty())
		return {};
	
	const std::wstring idKey = std::to_wstring(initialShapeIndex);

	MeshBundle mesh;
	mesh.reserve(mModelParts.size());
	std::transform(mModelParts.begin(), mModelParts.end(), std::back_inserter(mesh),
	               [this, &idKey](const ModelPart& part) -> ON_Mesh { return toON_Mesh(part, idKey); });
	return mesh;
}
