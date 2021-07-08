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

	void SetRPKPath(const std::wstring& rpk_path); // might throw!

	int GetRuleAttributeCount();
	const RuleAttributes& GetRuleAttributes() const;

	void SetInitialShapes(const std::vector<RawInitialShape>& shapes);
	void ClearInitialShapes();

	size_t GenerateGeometry();

	void setRuleAttributeValue(const int initialShapeIndex, const std::wstring& rule, double value, size_t /*count*/);
	void setRuleAttributeValue(const int initialShapeIndex, const std::wstring& rule, int value, size_t /*count*/);
	void setRuleAttributeValue(const int initialShapeIndex, const std::wstring& rule, bool value, size_t /*count*/);
	void setRuleAttributeValue(const int initialShapeIndex, const std::wstring& rule, std::wstring& value,
	                           size_t /*count*/);
	void setRuleAttributeValue(const int initialShapeIndex, const std::wstring& rule, const double* value,
	                           const size_t count);
	void setRuleAttributeValue(const int initialShapeIndex, const std::wstring& rule, bool* value, const size_t count);
	void setRuleAttributeValue(const int initialShapeIndex, const std::wstring& rule, std::vector<const wchar_t*> value,
	                           const size_t /*count*/);

	const Reporting::GroupedReports& getReports() const {
		return mGroupedReports;
	}
	Reporting::ReportsVector getReportsOfModel(int initialShapeIndex);

	const std::vector<GeneratedModelPtr>& getGenModels() const;

	void setMaterialGeneration(bool emitMaterial);

	bool getDefaultValueBoolean(const std::wstring key, bool* value);
	bool getDefaultValueNumber(const std::wstring key, double* value);
	bool getDefaultValueText(const std::wstring key, ON_wString* pText);

private:
	std::vector<RawInitialShape> mShapes;
	std::wstring mPackagePath;
	std::vector<pcu::ShapeAttributes> mAttributes;

	pcu::AttributeMapBuilderPtr mAttrBuilder;
	pcu::AttributeMapBuilderVector mAttrBuilders;

	pcu::EncoderOptions options;

	std::unique_ptr<ModelGenerator> mModelGenerator;
	std::vector<GeneratedModelPtr> mGeneratedModels;

	Reporting::GroupedReports mGroupedReports;
};

// Global PRT handle
RhinoPRTAPI& get();

} // namespace RhinoPRT
