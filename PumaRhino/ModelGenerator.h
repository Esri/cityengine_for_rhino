/**
 * Puma - CityEngine Plugin for Rhinoceros
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

#pragma once

#include "GeneratedModel.h"
#include "PRTContext.h"
#include "RawInitialShape.h"
#include "ResolveMapCache.h"
#include "RhinoCallbacks.h"
#include "RuleAttributes.h"
#include "utils.h"

/**
 * Entry point of the PRT. Is given an initial shape and rpk package, gives them to the PRT and gets the results.
 */
class ModelGenerator {
public:
	ModelGenerator();

	pcu::ResolveMapSPtr getResolveMap(const std::wstring& rulePkg);

	std::vector<GeneratedModelPtr> generateModel(const std::wstring& rulePkg,
	                                             const std::vector<RawInitialShape>& rawInitialShapes,
	                                             const pcu::ShapeAttributes& shapeAttributes,
	                                             pcu::AttributeMapBuilderVector& aBuilders);

	pcu::AttributeMapPtrVector evalDefaultAttributes(const std::wstring& rulePkg,
	                                                 const std::vector<RawInitialShape>& rawInitialShapes,
	                           pcu::ShapeAttributes& shapeAttributes);

	pcu::ShapeAttributes getShapeAttributes(const std::wstring& rulePkg);

	void updateEncoderOptions(bool emitMaterials);

	const RuleAttributes getRuleAttributes(const std::wstring& rulePkg);

private:

	pcu::AttributeMapPtr mRhinoEncoderOptions;
	pcu::AttributeMapPtr mCGAErrorOptions;
	pcu::AttributeMapPtr mCGAPrintOptions;

	bool createInitialShapes(pcu::ResolveMapSPtr& resolveMap,
							 const std::vector<RawInitialShape>& rawInitialShapes,
	                         const pcu::ShapeAttributes& shapeAttributes,
	                         pcu::AttributeMapBuilderVector& aBuilders,
	                         std::vector<pcu::InitialShapePtr>& initialShapes,
	                         std::vector<pcu::AttributeMapPtr>& initialShapeAttributes) const;

	void extractMainShapeAttributes(pcu::AttributeMapBuilderPtr& aBuilder, const pcu::ShapeAttributes& shapeAttr,
	                                std::wstring& ruleFile, std::wstring& startRule, int32_t& seed,
	                                std::wstring& shapeName, pcu::AttributeMapPtr& convertShapeAttr) const;
};