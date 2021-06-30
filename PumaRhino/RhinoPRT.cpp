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

#include "RhinoPRT.h"

namespace RhinoPRT {

// Global PRT handle
RhinoPRTAPI& get() {
	static const std::unique_ptr<RhinoPRT::RhinoPRTAPI> myPRTAPI(new RhinoPRT::RhinoPRTAPI());
	return *myPRTAPI;
}

bool RhinoPRTAPI::InitializeRhinoPRT() {
	return PRTContext::get()->isAlive();
}

void RhinoPRTAPI::ShutdownRhinoPRT() {

	PRTContext::get().reset();
}

bool RhinoPRTAPI::IsPRTInitialized() {
	return PRTContext::get()->isAlive();
}

int RhinoPRTAPI::GetRuleAttributeCount() {
	return static_cast<int>(mModelGenerator->getRuleAttributes().size());
}

const RuleAttributes& RhinoPRTAPI::GetRuleAttributes() const {
	return mModelGenerator->getRuleAttributes();
}

void RhinoPRTAPI::SetRPKPath(const std::wstring& rpk_path) {

	mPackagePath = rpk_path;

	// initialize the resolve map and rule infos here. Create the vector of rule attributes.
	if (!mModelGenerator)
		mModelGenerator = std::unique_ptr<ModelGenerator>(new ModelGenerator());

	// This also creates the resolve map
	mModelGenerator->updateRuleFiles(mPackagePath); // might throw !

	// Also create the attribute map builder that will receive the rule attributes.
	mAttrBuilder.reset(prt::AttributeMapBuilder::create());
}

void RhinoPRTAPI::AddInitialShape(const std::vector<InitialShape>& shapes) {

	// get the shape attributes data from ModelGenerator
	std::wstring rulef = mModelGenerator->getRuleFile();
	std::wstring ruleN = mModelGenerator->getStartingRule();
	std::wstring shapeN = mModelGenerator->getDefaultShapeName();

	mShapes.reserve(shapes.size());
	mAttributes.reserve(shapes.size());

	mShapes.insert(mShapes.end(), shapes.begin(), shapes.end());
	mAttributes.resize(mShapes.size(), pcu::ShapeAttributes(rulef, ruleN, shapeN));

	// compute the default values of rule attributes for each initial shape
	mModelGenerator->evalDefaultAttributes(mShapes, mAttributes);

	// Initialise the attribute map builders for each initial shape.
	mAttrBuilders.resize(shapes.size());
	for (auto& it : mAttrBuilders) {
		it.reset(prt::AttributeMapBuilder::create());
	}
}

void RhinoPRTAPI::ClearInitialShapes() {
	mShapes.clear();
	mGeneratedModels.clear();
	mAttributes.clear();
	mGroupedReports.clear();
	mGeneratedModels.clear();
	mAttrBuilders.clear();
}

size_t RhinoPRTAPI::GenerateGeometry() {
	mGeneratedModels = mModelGenerator->generateModel(mShapes, mAttributes, options, mAttrBuilders);
	assert(mGeneratedModels.size() == mShapes.size());
	return mShapes.size();
}

const std::vector<GeneratedModelPtr>& RhinoPRTAPI::getGenModels() const {
	return mGeneratedModels;
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, double value,
                                        size_t /*count*/) {
	if (rule->mType == prt::AAT_FLOAT) {
		mAttrBuilders[initialShapeIndex]->setFloat(rule->mFullName.c_str(), value);
	}
	else {
		LOG_ERR << L"Trying to set a double value to an attribute of type " << rule->mType << std::endl;
	}
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, int value,
                                        size_t /*count*/) {
	if (rule->mType == prt::AAT_INT) {
		mAttrBuilders[initialShapeIndex]->setInt(rule->mFullName.c_str(), value);
	}
	else {
		LOG_ERR << L"Trying to set an int value to an attribute of type " << rule->mType << std::endl;
	}
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, bool value,
                                        size_t /*count*/) {
	if (rule->mType == prt::AAT_BOOL) {
		mAttrBuilders[initialShapeIndex]->setBool(rule->mFullName.c_str(), value);
	}
	else {
		LOG_ERR << L"Trying to set a boolean value to an attribute of type " << rule->mType << std::endl;
	}
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, std::wstring& value,
                                        size_t /*count*/) {
	if (rule->mType == prt::AAT_STR) {
		mAttrBuilders[initialShapeIndex]->setString(rule->mFullName.c_str(), value.c_str());
	}
	else {
		LOG_ERR << L"Trying to set a wstring to an attribute of type " << rule->mType << std::endl;
	}
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, const double* value,
                                        const size_t count) {
	if (rule->mType == prt::AAT_FLOAT_ARRAY) {
		mAttrBuilders[initialShapeIndex]->setFloatArray(rule->mFullName.c_str(), value, count);
	}
	else {
		LOG_ERR << L"Trying to set an array of double to an attribute of type " << rule->mType << std::endl;
	}
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, bool* value,
                                        const size_t count) {
	if (rule->mType == prt::AAT_BOOL_ARRAY) {
		mAttrBuilders[initialShapeIndex]->setBoolArray(rule->mFullName.c_str(), value, count);
	}
	else {
		LOG_ERR << L"Trying to set an array of bool to an attribute of type " << rule->mType << std::endl;
	}
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule,
                                        std::vector<const wchar_t*> value, const size_t /*count*/) {
	if (rule->mType == prt::AAT_STR_ARRAY) {
		mAttrBuilders[initialShapeIndex]->setStringArray(rule->mFullName.c_str(), value.data(), value.size());
	}
	else {
		LOG_ERR << L"Trying to set an array of wstring to an attribute of type " << rule->mType << std::endl;
	}
}

Reporting::ReportsVector RhinoPRTAPI::getReportsOfModel(int initialShapeIndex) {
	if ((mGeneratedModels.size() <= initialShapeIndex) || !mGeneratedModels[initialShapeIndex])
		return Reporting::EMPTY_REPORTS;

	const auto& reports = mGeneratedModels[initialShapeIndex]->getReports();
	return Reporting::ToReportsVector(reports);
}

void RhinoPRTAPI::setMaterialGeneration(bool emitMaterial) {
	options.emitMaterial = emitMaterial;
}

bool RhinoPRTAPI::getDefaultValueBoolean(const std::wstring key, bool* value) {
	return mModelGenerator->getDefaultValueBoolean(key, value);
}

bool RhinoPRTAPI::getDefaultValueNumber(const std::wstring key, double* value) {
	return mModelGenerator->getDefaultValueNumber(key, value);
}

bool RhinoPRTAPI::getDefaultValueText(const std::wstring key, ON_wString* pText) {
	return mModelGenerator->getDefaultValueText(key, pText);
}
} // namespace RhinoPRT
