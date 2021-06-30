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

#include "RhinoCallbacks.h"

class GeneratedModel final {
public:
	GeneratedModel() = default;
	GeneratedModel(const ModelPtr& model);
	~GeneratedModel() = default;

	using MeshBundle = std::vector<ON_Mesh>;
	const MeshBundle getMeshesFromGenModel(size_t initialShapeIndex) const;
	int getMeshPartCount() const;
	const Reporting::ReportMap& getReport() const;
	const Materials::MaterialsMap& getMaterials() const;
	const std::vector<std::wstring>& getPrintOutput() const;
	const std::vector<std::wstring>& getErrorOutput() const;

private:
	ModelPtr mModel;

	/// Creates an ON_Mesh and setup its uv coordinates, for a given ModelPart.
	ON_Mesh toON_Mesh(const ModelPart& modelPart, const std::wstring& idKey) const;
};

using GeneratedModelPtr = std::shared_ptr<GeneratedModel>;