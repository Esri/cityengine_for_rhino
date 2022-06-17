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

const RuleAttributes RhinoPRTAPI::GetRuleAttributes(const std::wstring& rulePkg) {
	if (!mModelGenerator)
		mModelGenerator = std::unique_ptr<ModelGenerator>(new ModelGenerator());
	return mModelGenerator->getRuleAttributes(rulePkg);
}

const pcu::AttributeMapPtrVector RhinoPRTAPI::getDefaultAttributes(const std::wstring& rpk_path, 
																   std::vector<RawInitialShape>& rawInitialShapes) {
	if (!mModelGenerator)
		mModelGenerator = std::unique_ptr<ModelGenerator>(new ModelGenerator());

	pcu::ShapeAttributes attributes = mModelGenerator->getShapeAttributes(rpk_path);
	return mModelGenerator->evalDefaultAttributes(rpk_path, rawInitialShapes, attributes);
}

std::vector<GeneratedModelPtr> RhinoPRTAPI::GenerateGeometry(const std::wstring& rpk_path,
                                                             std::vector<RawInitialShape>& rawInitialShapes,
                                                             pcu::AttributeMapBuilderVector& aBuilders) {
	if (!mModelGenerator)
		mModelGenerator = std::unique_ptr<ModelGenerator>(new ModelGenerator());

	//Build ShapeAttributes
	pcu::ShapeAttributes attributes = mModelGenerator->getShapeAttributes(rpk_path);
	
	std::vector<GeneratedModelPtr> generatedModels = mModelGenerator->generateModel(rpk_path, rawInitialShapes, attributes, aBuilders);
	assert(generatedModels.size() == rawInitialShapes.size());
	return generatedModels;
}

void RhinoPRTAPI::setMaterialGeneration(bool emitMaterial) {
	mModelGenerator->updateEncoderOptions(emitMaterial);
}
} // namespace RhinoPRT
