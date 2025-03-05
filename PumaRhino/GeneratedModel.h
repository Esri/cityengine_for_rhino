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

#pragma once

#include "MaterialAttribute.h"
#include "ReportAttribute.h"

#include <vector>
#include <string>

struct ModelPart {
	std::vector<double> mVertices;
	std::vector<double> mNormals;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaces;
	ON_2fPointArray mUVs;
	std::vector<uint32_t> mUVIndices;
	std::vector<uint32_t> mUVCounts;
};

class GeneratedModel final {
public:
	GeneratedModel() = default;
	~GeneratedModel() = default;

	ModelPart& addModelPart();
	const std::vector<ModelPart>& getModelParts() const;
	int getMeshPartCount() const;
	ModelPart& getCurrentModelPart();

	void addMaterial(const Materials::MaterialAttribute& ma);
	const Materials::MaterialsMap& getMaterials() const;

	void addReport(const Reporting::ReportAttribute& ra);
	const Reporting::ReportMap& getReports() const;

	void addPrint(const std::wstring_view& message);
	const std::vector<std::wstring>& getPrints() const;

	void addError(const std::wstring_view& error);
	const std::vector<std::wstring>& getErrors() const;

	using MeshBundle = std::vector<ON_Mesh>;
	const MeshBundle createRhinoMeshes(size_t initialShapeIndex) const;

private:
	std::vector<ModelPart> mModelParts;
	Reporting::ReportMap mReports;
	Materials::MaterialsMap mMaterials;
	std::vector<std::wstring> mPrints;
	std::vector<std::wstring> mErrors;
};

using GeneratedModelPtr = std::shared_ptr<GeneratedModel>;
