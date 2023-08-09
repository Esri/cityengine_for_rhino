/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021-2023 Esri R&D Center Zurich
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

#include "PRTContext.h"
#include "version.h"

#include "ModelGenerator.h"

#include "Logger.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <map>
#include <memory>
#include <string>

#pragma comment(lib, "ole32.lib") // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was
                                  // unexpected here" when using /permissive-

namespace RhinoPRT {

class RhinoPRTAPI {
public:
	const RuleAttributeUPtr RULE_NOT_FOUND{};

	bool InitializeRhinoPRT();
	void ShutdownRhinoPRT();
	bool IsPRTInitialized();

	const RuleAttributes GetRuleAttributes(const std::wstring& rulePkg);

	const pcu::AttributeMapPtrVector getDefaultAttributes(const std::wstring& rpk_path,
	                                                  std::vector<RawInitialShape>& rawInitialShapes);

	std::vector<GeneratedModelPtr> GenerateGeometry(const std::wstring& rpk_path,
	                                                std::vector<RawInitialShape>& rawInitialShapes,
	                                                pcu::AttributeMapBuilderVector& aBuilders);

	void setMaterialGeneration(bool emitMaterial);

private:
	std::vector<RawInitialShape> mShapes;
	std::vector<pcu::ShapeAttributes> mAttributes;

	pcu::AttributeMapBuilderVector mAttrBuilders;

	std::unique_ptr<ModelGenerator> mModelGenerator;
};

// Global PRT handle
RhinoPRTAPI& get();

} // namespace RhinoPRT
