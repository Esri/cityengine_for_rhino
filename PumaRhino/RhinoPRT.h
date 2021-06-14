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

#define RHINOPRT_API __declspec(dllexport)

namespace RhinoPRT {

class RhinoPRTAPI {
public:
	const RuleAttributeUPtr RULE_NOT_FOUND{};

	bool InitializeRhinoPRT();
	void ShutdownRhinoPRT();
	bool IsPRTInitialized();

	void SetRPKPath(const std::wstring& rpk_path);

	int GetRuleAttributeCount();
	const RuleAttributes& GetRuleAttributes() const;

	void AddInitialShape(const std::vector<InitialShape>& shapes);
	void ClearInitialShapes();

	bool GenerateGeometry();

	void setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, double value,
	                           size_t /*count*/);
	void setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, int value, size_t /*count*/);
	void setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, bool value,
	                           size_t /*count*/);
	void setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, std::wstring& value,
	                           size_t /*count*/);
	void setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, const double* value,
	                           const size_t count);
	void setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, bool* value,
	                           const size_t count);
	void setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule,
	                           std::vector<const wchar_t*> value, const size_t /*count*/);

	const Reporting::GroupedReports& getReports() const {
		return mGroupedReports;
	}
	Reporting::ReportsVector getReportsOfModel(int initialShapeIndex);

	std::vector<GeneratedModel>& getGenModels();

	std::vector<int> getModelIds();

	void setMaterialGeneration(bool emitMaterial);

	bool getDefaultValueBoolean(const std::wstring key, bool* value);
	bool getDefaultValueNumber(const std::wstring key, double* value);
	bool getDefaultValueText(const std::wstring key, ON_wString* pText);

private:
	std::vector<InitialShape> mShapes;
	std::wstring mPackagePath;
	std::vector<pcu::ShapeAttributes> mAttributes;

	pcu::AttributeMapBuilderPtr mAttrBuilder;
	pcu::AttributeMapBuilderVector mAttrBuilders;

	pcu::EncoderOptions options;

	std::unique_ptr<ModelGenerator> mModelGenerator;
	std::vector<GeneratedModel> mGeneratedModels;

	Reporting::GroupedReports mGroupedReports;
};

// Global PRT handle
RhinoPRTAPI& get();

} // namespace RhinoPRT

// Define exposed functions here
extern "C" {

RHINOPRT_API void GetProductVersion(ON_wString* version_Str);

RHINOPRT_API bool InitializeRhinoPRT();

RHINOPRT_API void ShutdownRhinoPRT();

RHINOPRT_API void SetPackage(const wchar_t* rpk_path);

RHINOPRT_API bool AddInitialMesh(ON_SimpleArray<const ON_Mesh*>* pMesh);

RHINOPRT_API void ClearInitialShapes();

RHINOPRT_API bool Generate();

RHINOPRT_API bool GetMeshBundle(int initialShapeIndex, ON_SimpleArray<ON_Mesh*>* pMeshArray);

RHINOPRT_API void GetAllMeshIDs(ON_SimpleArray<int>* pMeshIDs);

RHINOPRT_API int GetMeshPartCount(int initialShapeIndex);

RHINOPRT_API int GetRuleAttributesCount();

RHINOPRT_API bool GetRuleAttribute(int attrIdx, ON_wString* pRule, ON_wString* pName, ON_wString* pNickname,
                                   prt::AnnotationArgumentType* type, ON_wString* pGroup);

RHINOPRT_API void SetRuleAttributeDouble(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName,
                                         double value);

RHINOPRT_API void SetRuleAttributeBoolean(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName,
                                          bool value);

RHINOPRT_API void SetRuleAttributeInteger(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName,
                                          int value);

RHINOPRT_API void SetRuleAttributeString(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName,
                                         const wchar_t* value);

RHINOPRT_API void SetRuleAttributeDoubleArray(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName,
                                              ON_SimpleArray<double>* pValueArray);

RHINOPRT_API void SetRuleAttributeBoolArray(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName,
                                            ON_SimpleArray<int>* pValueArray);

RHINOPRT_API void SetRuleAttributeStringArray(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName,
                                              ON_ClassArray<ON_wString>* pValueArray);

RHINOPRT_API void GetReports(int initialShapeIndex, ON_ClassArray<ON_wString>* pKeysArray,
                             ON_SimpleArray<double>* pDoubleReports, ON_SimpleArray<bool>* pBoolReports,
                             ON_ClassArray<ON_wString>* pStringReports);

RHINOPRT_API void GetAnnotationTypes(int ruleIdx, ON_SimpleArray<AttributeAnnotation>* pAnnotTypeArray);

RHINOPRT_API bool GetEnumType(int ruleIdx, int enumIdx, EnumAnnotationType* type);

RHINOPRT_API bool GetAnnotationEnumDouble(int ruleIdx, int enumIdx, ON_SimpleArray<double>* pArray, bool* restricted);

RHINOPRT_API bool GetAnnotationEnumString(int ruleIdx, int enumIdx, ON_ClassArray<ON_wString>* pArray,
                                          bool* restricted);

RHINOPRT_API bool GetAnnotationRange(int ruleIdx, int enumIdx, double* min, double* max, double* stepsize,
                                     bool* restricted);

RHINOPRT_API bool GetMaterial(int initialShapeIndex, int meshID, int* uvSet, ON_ClassArray<ON_wString>* pTexKeys,
                              ON_ClassArray<ON_wString>* pTexPaths, ON_SimpleArray<int>* pDiffuseColor,
                              ON_SimpleArray<int>* pAmbientColor, ON_SimpleArray<int>* pSpecularColor, double* opacity,
                              double* shininess);

RHINOPRT_API void SetMaterialGenerationOption(bool doGenerate);

RHINOPRT_API bool GetDefaultValueBoolean(const wchar_t* key, bool* value);

RHINOPRT_API bool GetDefaultValueNumber(const wchar_t* key, double* value);

RHINOPRT_API bool GetDefaultValueText(const wchar_t* key, ON_wString* pText);
}
