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

#include "RhinoPRT.h"

namespace RhinoPRT {

// Global PRT handle
RhinoPRTAPI& get() {
	static const std::unique_ptr<RhinoPRT::RhinoPRTAPI> myPRTAPI(new RhinoPRT::RhinoPRTAPI());
	return *myPRTAPI;
}

bool RhinoPRTAPI::InitializeRhinoPRT() {
	return !!PRTContext::get() && PRTContext::get()->isAlive();
}

void RhinoPRTAPI::ShutdownRhinoPRT() {
	PRTContext::get().reset();
}

bool RhinoPRTAPI::IsPRTInitialized() {
	return !!PRTContext::get() && PRTContext::get()->isAlive();
}

int RhinoPRTAPI::GetRuleAttributeCount() {
	return static_cast<int>(mModelGenerator->getRuleAttributes().size());
}

const RuleAttributes& RhinoPRTAPI::GetRuleAttributes() const {
	return mModelGenerator->getRuleAttributes();
}

void RhinoPRTAPI::SetInitialShapes(const std::vector<RawInitialShape>& shapes) {

	// get the shape attributes data from ModelGenerator
	std::wstring rulef = mModelGenerator->getRuleFile();
	std::wstring ruleN = mModelGenerator->getStartingRule();
	std::wstring shapeN = mModelGenerator->getDefaultShapeName();

	mShapes.reserve(shapes.size());
	mAttributes.reserve(shapes.size());

	mShapes.insert(mShapes.end(), shapes.begin(), shapes.end());
	//mAttributes.resize(mShapes.size(), pcu::ShapeAttributes(rulef, ruleN, shapeN));

	// compute the default values of rule attributes for each initial shape
	// mModelGenerator->evalDefaultAttributes(mShapes, mAttributes);

}

void RhinoPRTAPI::ClearInitialShapes() {
	mShapes.clear();
	mGeneratedModels.clear();
	mAttributes.clear();
	mGroupedReports.clear();
	mAttrBuilders.clear();
}

std::vector<GeneratedModelPtr> RhinoPRTAPI::GenerateGeometry(const std::wstring& rpk_path,
                                                             std::vector<RawInitialShape>& rawInitialShapes,
                                                             pcu::AttributeMapBuilderVector& aBuilders) {
	//Build ShapeAttributes
	pcu::ShapeAttributes attribute = mModelGenerator->getShapeAttributes(rpk_path);
	std::vector<pcu::ShapeAttributes> attributes(rawInitialShapes.size(), attribute);
	
	std::vector<GeneratedModelPtr> generatedModels = mModelGenerator->generateModel(rpk_path, rawInitialShapes, attributes, aBuilders);
	assert(generatedModels.size() == rawInitialShapes.size());
	return generatedModels;
}

const std::vector<GeneratedModelPtr>& RhinoPRTAPI::getGenModels() const {
	return mGeneratedModels;
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, double value,
                                        size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setFloat(name.c_str(), value);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, int value,
                                        size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setInt(name.c_str(), value);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, bool value,
                                        size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setBool(name.c_str(), value);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, std::wstring& value,
                                        size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setString(name.c_str(), value.c_str());
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, const double* value,
                                        const size_t count) {
	mAttrBuilders[initialShapeIndex]->setFloatArray(name.c_str(), value, count);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, bool* value,
                                        const size_t count) {
	mAttrBuilders[initialShapeIndex]->setBoolArray(name.c_str(), value, count);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name,
                                        std::vector<const wchar_t*> value, const size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setStringArray(name.c_str(), value.data(), value.size());
}

Reporting::ReportsVector RhinoPRTAPI::getReportsOfModel(int initialShapeIndex) {
	if ((mGeneratedModels.size() <= initialShapeIndex) || !mGeneratedModels[initialShapeIndex])
		return Reporting::EMPTY_REPORTS;

	const auto& reports = mGeneratedModels[initialShapeIndex]->getReports();
	return Reporting::ToReportsVector(reports);
}

void RhinoPRTAPI::setMaterialGeneration(bool emitMaterial) {
	mModelGenerator->updateEncoderOptions(emitMaterial);
}

bool RhinoPRTAPI::getDefaultValuesBoolean(const std::wstring key, ON_SimpleArray<int>* pValues) {
	return mModelGenerator->getDefaultValuesBoolean(key, pValues);
}

bool RhinoPRTAPI::getDefaultValuesNumber(const std::wstring key, ON_SimpleArray<double>* pValues) {
	return mModelGenerator->getDefaultValuesNumber(key, pValues);
}

bool RhinoPRTAPI::getDefaultValuesText(const std::wstring key, ON_ClassArray<ON_wString>* pTexts) {
	return mModelGenerator->getDefaultValuesText(key, pTexts);
}

bool RhinoPRTAPI::getDefaultValuesBooleanArray(const std::wstring key, ON_SimpleArray<int>* pValues, ON_SimpleArray<int>* pSizes) {
	return mModelGenerator->getDefaultValuesBooleanArray(key, pValues, pSizes);
}

bool RhinoPRTAPI::getDefaultValuesNumberArray(const std::wstring key, ON_SimpleArray<double>* pValues,
                                              ON_SimpleArray<int>* pSizes) {
	return mModelGenerator->getDefaultValuesNumberArray(key, pValues, pSizes);
}

bool RhinoPRTAPI::getDefaultValuesTextArray(const std::wstring key, ON_ClassArray<ON_wString>* pTexts,
                                            ON_SimpleArray<int>* pSizes) {
	return mModelGenerator->getDefaultValuesTextArray(key, pTexts, pSizes);
}
} // namespace RhinoPRT
