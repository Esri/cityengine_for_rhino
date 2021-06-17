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

#include "ModelGenerator.h"

#include "AttrEvalCallbacks.h"
#include "Logger.h"

#include <cassert>
#include <filesystem>

namespace {

constexpr const wchar_t* ENCODER_ID_RHINO     = L"com.esri.rhinoprt.RhinoEncoder";
constexpr const wchar_t* ENCODER_ID_CGA_ERROR = L"com.esri.prt.core.CGAErrorEncoder";
constexpr const wchar_t* ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";

constexpr const wchar_t* RESOLVEMAP_EXTRACTION_PREFIX = L"rhino_prt";
constexpr const wchar_t* ENCODER_ID_CGA_EVALATTR = L"com.esri.prt.core.AttributeEvalEncoder";

pcu::AttributeMapPtr getAttrEvalEncoderInfo() {
	const pcu::EncoderInfoPtr encInfo(prt::createEncoderInfo(ENCODER_ID_CGA_EVALATTR));
	const prt::AttributeMap* encOpts = nullptr;
	encInfo->createValidatedOptionsAndStates(nullptr, &encOpts);
	return pcu::AttributeMapPtr(encOpts);
}

} // namespace

ResolveMap::ResolveMapCache::CacheStatus ModelGenerator::initResolveMap(const std::filesystem::path& rpk) {
	if (rpk.empty())
		return ResolveMap::ResolveMapCache::CacheStatus::FAILURE;

	// Get the resolvemap from the resolve map cache
	auto lookup = PRTContext::get()->getResolveMap(rpk);

	if (lookup.second != ResolveMap::ResolveMapCache::CacheStatus::FAILURE) {
		mResolveMap = lookup.first;
	}

	return lookup.second;
}

void ModelGenerator::updateRuleFiles(const std::wstring& rulePkg) {
	// Get the resolve map.
	auto cacheStatus = initResolveMap(rulePkg);
	if (cacheStatus == ResolveMap::ResolveMapCache::CacheStatus::FAILURE) {
		LOG_ERR << "Failed to create the resolve map from rule package " << mRulePkg << std::endl;
		mRuleAttributes.clear();
		return;
	}
	else if (cacheStatus == ResolveMap::ResolveMapCache::CacheStatus::HIT && rulePkg == mRulePkg) {
		// resolvemap already exists and the rule file was not changed, no need to update.
		return;
	}

	// Cache miss -> initialize everything
	// Reset the rule infos
	mRuleAttributes.clear();
	mRuleFile.clear();
	mStartRule.clear();
	mRulePkg = rulePkg;

	// Extract the rule package info.
	mRuleFile = pcu::getRuleFileEntry(mResolveMap);
	if (mRuleFile.empty()) {
		LOG_ERR << "Could not find rule file in rule package " << mRulePkg;
		return;
	}

	// To create the ruleFileInfo, we first need the ruleFileURI
	const wchar_t* ruleFileURI = mResolveMap->getString(mRuleFile.c_str());
	if (ruleFileURI == nullptr) {
		LOG_ERR << "could not find rule file URI in resolve map of rule package " << mRulePkg;
		return;
	}

	// Create RuleFileInfo
	prt::Status infoStatus = prt::STATUS_UNSPECIFIED_ERROR;
	mRuleFileInfo =
	        pcu::RuleFileInfoPtr(prt::createRuleFileInfo(ruleFileURI, PRTContext::get()->mPRTCache.get(), &infoStatus));
	if (!mRuleFileInfo || infoStatus != prt::STATUS_OK) {
		LOG_ERR << "could not get rule file info from rule file " << mRuleFile;
		return;
	}

	mStartRule = pcu::detectStartRule(mRuleFileInfo);

	// Fill the list of rule attributes
	createRuleAttributes(mRuleFile, *mRuleFileInfo.get(), mRuleAttributes);
}

bool ModelGenerator::evalDefaultAttributes(const std::vector<InitialShape>& initial_geom,
                                           std::vector<pcu::ShapeAttributes>& shapeAttributes) {
	// setup encoder options for attribute evaluation encoder
	constexpr const wchar_t* encs[] = {ENCODER_ID_CGA_EVALATTR};
	constexpr size_t encsCount = sizeof(encs) / (sizeof(encs[0]));
	const pcu::AttributeMapPtr encOpts = getAttrEvalEncoderInfo();
	const prt::AttributeMap* encsOpts[] = {encOpts.get()};

	const size_t numShapes = initial_geom.size();

	fillInitialShapeBuilder(initial_geom);

	pcu::AttributeMapBuilderVector attribMapBuilders;
	attribMapBuilders.reserve(numShapes);

	for (size_t isIdx = 0; isIdx < numShapes; ++isIdx) {
		pcu::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());
		pcu::AttributeMapPtr ruleAttr(amb->createAttributeMap());
		attribMapBuilders.emplace_back(std::move(amb));
	}

	std::vector<const prt::InitialShape*> initShapes(numShapes);
	std::vector<pcu::InitialShapePtr> initialShapePtrs(numShapes);
	std::vector<pcu::AttributeMapPtr> convertedShapeAttrVec(numShapes);
	setAndCreateInitialShape(attribMapBuilders, shapeAttributes, initShapes, initialShapePtrs, convertedShapeAttrVec);

	assert(attribMapBuilders.size() == initShapes.size());
	assert(initShapes.size() == mInitialShapesBuilders.size());

	// run generate
	AttrEvalCallbacks aec(attribMapBuilders, mRuleFileInfo);
	const prt::Status status = prt::generate(initShapes.data(), initShapes.size(), nullptr, encs, encsCount, encsOpts,
	                                         &aec, PRTContext::get()->mPRTCache.get(), nullptr);
	if (status != prt::STATUS_OK) {
		LOG_ERR << "assign: prt::generate() failed with status: '" << prt::getStatusDescription(status) << "' ("
		        << status << ")";
		return false;
	}

	createDefaultValueMaps(attribMapBuilders);

	return true;
}

void ModelGenerator::createDefaultValueMaps(pcu::AttributeMapBuilderVector& ambv) {
	for each (auto& amb in ambv) {
		prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
		pcu::AttributeMapPtr am{amb->createAttributeMap(&status)};
		if (status == prt::STATUS_OK)
			mDefaultValuesMap.emplace_back(std::move(am));
	}
}

void ModelGenerator::fillInitialShapeBuilder(const std::vector<InitialShape>& initial_geom) {
	mInitialShapesBuilders.resize(initial_geom.size());

	// Initial shapes initializing
	for (size_t i = 0; i < initial_geom.size(); ++i) {
		pcu::InitialShapeBuilderPtr isb{prt::InitialShapeBuilder::create()};

		if (isb->setGeometry(initial_geom[i].getVertices(), initial_geom[i].getVertexCount(),
		                     initial_geom[i].getIndices(), initial_geom[i].getIndexCount(),
		                     initial_geom[i].getFaceCounts(), initial_geom[i].getFaceCountsCount()) != prt::STATUS_OK) {

			LOG_ERR << "invalid initial geometry";

			mValid = false;
		}

		if (mValid) {
			mInitialShapesBuilders[i] = std::move(isb);
		}
	}
}

void ModelGenerator::generateModel(const std::vector<InitialShape>& initial_geom,
                                   std::vector<pcu::ShapeAttributes>& shapeAttributes,
                                   const pcu::EncoderOptions& geometryEncoderOptions,
                                   pcu::AttributeMapBuilderVector& aBuilders,
                                   std::vector<GeneratedModel>& generated_models) {
	fillInitialShapeBuilder(initial_geom);

	if (!mValid) {
		LOG_ERR << "invalid ModelGenerator instance.";
		return;
	}

	if ((shapeAttributes.size() != 1) && (shapeAttributes.size() < mInitialShapesBuilders.size())) {
		LOG_ERR << "not enough shape attributes dictionaries defined.";
		return;
	}
	else if (shapeAttributes.size() > mInitialShapesBuilders.size()) {
		LOG_WRN << "number of shape attributes dictionaries defined greater than number of initial shapes given."
		        << std::endl;
	}

	generated_models.reserve(mInitialShapesBuilders.size());

	try {

		if (!mRulePkg.empty()) {
			LOG_INF << "using rule package " << mRulePkg << std::endl;

			if (!mResolveMap || mRuleFile.empty() || !mRuleFileInfo) {
				LOG_ERR << "Rule package not processed correcty." << std::endl;
				return;
			}
		}

		std::vector<const prt::InitialShape*> initialShapes(mInitialShapesBuilders.size());
		std::vector<pcu::InitialShapePtr> initialShapePtrs(mInitialShapesBuilders.size());
		std::vector<pcu::AttributeMapPtr> convertedShapeAttrVec(mInitialShapesBuilders.size());
		setAndCreateInitialShape(aBuilders, shapeAttributes, initialShapes, initialShapePtrs, convertedShapeAttrVec);

		if (!mEncoderBuilder)
			mEncoderBuilder.reset(prt::AttributeMapBuilder::create());

		initializeEncoderData(geometryEncoderOptions);

		std::vector<const wchar_t*> encoders;
		encoders.reserve(3);
		std::vector<const prt::AttributeMap*> encodersOptions;
		encodersOptions.reserve(3);

		getRawEncoderDataPointers(encoders, encodersOptions);

		pcu::RhinoCallbacksPtr roc{std::make_unique<RhinoCallbacks>(mInitialShapesBuilders.size())};

		// GENERATE!
		const prt::Status genStat =
		        prt::generate(initialShapes.data(), initialShapes.size(), nullptr, encoders.data(), encoders.size(),
		                      encodersOptions.data(), roc.get(), PRTContext::get()->mPRTCache.get(), nullptr);

		if (genStat != prt::STATUS_OK) {
			LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' (" << genStat
			        << ")";
			return;
		}

		for (size_t idx = 0; idx < mInitialShapesBuilders.size(); ++idx) {
			generated_models.emplace_back(idx, roc->getModel(idx));
		}
	}
	catch (const std::exception& e) {
		LOG_ERR << "caught exception: " << e.what();
		return;
	}
	catch (...) {
		LOG_ERR << "caught unknown exception.";
		return;
	}
}

void ModelGenerator::setAndCreateInitialShape(pcu::AttributeMapBuilderVector& aBuilders,
                                              const std::vector<pcu::ShapeAttributes>& shapesAttr,
                                              std::vector<const prt::InitialShape*>& initShapes,
                                              std::vector<pcu::InitialShapePtr>& initShapesPtrs,
                                              std::vector<pcu::AttributeMapPtr>& convertedShapeAttr) {
	for (size_t i = 0; i < mInitialShapesBuilders.size(); ++i) {
		pcu::ShapeAttributes shapeAttr = shapesAttr[0];
		if (shapesAttr.size() > i) {
			shapeAttr = shapesAttr[i];
		}

		// Set to default values
		std::wstring ruleF = mRuleFile;
		std::wstring startR = mStartRule;
		int32_t randomS = mSeed;
		std::wstring shapeN = mShapeName;
		extractMainShapeAttributes(aBuilders[i], shapeAttr, ruleF, startR, randomS, shapeN, convertedShapeAttr[i]);

		mInitialShapesBuilders[i]->setAttributes(ruleF.c_str(), startR.c_str(), randomS, shapeN.c_str(),
		                                         convertedShapeAttr[i].get(), mResolveMap.get());

		initShapesPtrs[i].reset(mInitialShapesBuilders[i]->createInitialShape());
		initShapes[i] = initShapesPtrs[i].get();
	}
}

void ModelGenerator::initializeEncoderData(const pcu::EncoderOptions& encOpt) {
	mEncodersNames.clear();
	mEncodersOptionsPtr.clear();

	mEncodersNames.push_back(ENCODER_ID_RHINO);
	const pcu::AttributeMapPtr encOptions{pcu::createAttributeMapForEncoder(encOpt, *mEncoderBuilder)};
	mEncodersOptionsPtr.push_back(createValidatedOptions(ENCODER_ID_RHINO, encOptions));
}

void ModelGenerator::getRawEncoderDataPointers(std::vector<const wchar_t*>& allEnc,
                                               std::vector<const prt::AttributeMap*>& allEncOpt) {
	allEnc.clear();
	for (const std::wstring& encID : mEncodersNames)
		allEnc.push_back(encID.c_str());

	allEncOpt.clear();
	for (const auto& encOpts : mEncodersOptionsPtr)
		allEncOpt.push_back(encOpts.get());
}

void ModelGenerator::extractMainShapeAttributes(pcu::AttributeMapBuilderPtr& aBuilder,
                                                const pcu::ShapeAttributes& shapeAttr, std::wstring& ruleFile,
                                                std::wstring& startRule, int32_t& seed, std::wstring& shapeName,
                                                pcu::AttributeMapPtr& convertShapeAttr) {
	convertShapeAttr = pcu::createAttributeMapForShape(shapeAttr, *aBuilder.get());

	if (convertShapeAttr) {
		if (convertShapeAttr->hasKey(L"ruleFile") &&
		    convertShapeAttr->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
			ruleFile = convertShapeAttr->getString(L"ruleFile");
		if (convertShapeAttr->hasKey(L"startRule") &&
		    convertShapeAttr->getType(L"startRule") == prt::AttributeMap::PT_STRING)
			startRule = convertShapeAttr->getString(L"startRule");
		if (convertShapeAttr->hasKey(SEED_KEY) && convertShapeAttr->getType(SEED_KEY) == prt::AttributeMap::PT_INT)
			seed = convertShapeAttr->getInt(SEED_KEY);
		if (convertShapeAttr->hasKey(L"shapeName") &&
		    convertShapeAttr->getType(L"shapeName") == prt::AttributeMap::PT_STRING)
			shapeName = convertShapeAttr->getString(L"shapeName");
	}
}

std::wstring ModelGenerator::getRuleFile() {
	return this->mRuleFile;
}
std::wstring ModelGenerator::getStartingRule() {
	return this->mStartRule;
};
std::wstring ModelGenerator::getDefaultShapeName() {
	return this->mShapeName;
};

bool ModelGenerator::getDefaultValueBoolean(const std::wstring key, bool* value) {
	if (mDefaultValuesMap.empty())
		return false;

	for each (const auto& am in mDefaultValuesMap) {
		if (am->hasKey(key.c_str()) && am->getType(key.c_str()) == prt::AttributeMap::PrimitiveType::PT_BOOL) {
			prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
			*value = am->getBool(key.c_str(), &status);
			if (status == prt::STATUS_OK)
				return true;
			else {
				LOG_ERR << "Impossible to get default value for rule attribute: " << key
				        << " with error: " << prt::getStatusDescription(status);
			}
		}
	}

	return false;
}

bool ModelGenerator::getDefaultValueNumber(const std::wstring key, double* value) {
	if (mDefaultValuesMap.empty())
		return false;

	for each (const auto& am in mDefaultValuesMap) {
		if (am->hasKey(key.c_str())) {
			prt::Status status = prt::STATUS_OK;

			if (am->getType(key.c_str()) == prt::AttributeMap::PrimitiveType::PT_FLOAT) {
				*value = am->getFloat(key.c_str(), &status);
				if (status == prt::STATUS_OK)
					return true;
			}
			else if (am->getType(key.c_str()) == prt::AttributeMap::PrimitiveType::PT_INT) {
				*value = am->getInt(key.c_str(), &status);
				if (status == prt::STATUS_OK)
					return true;
			}

			if (status != prt::STATUS_OK)
				LOG_ERR << "Impossible to get default value for rule attribute: " << key
				        << " with error: " << prt::getStatusDescription(status);
		}
	}

	return false;
}

bool ModelGenerator::getDefaultValueText(const std::wstring key, ON_wString* pText) {
	if (mDefaultValuesMap.empty())
		return false;

	for each (const auto& am in mDefaultValuesMap) {
		if (am->hasKey(key.c_str()) && am->getType(key.c_str()) == prt::AttributeMap::PrimitiveType::PT_STRING) {
			prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
			std::wstring valueStr(am->getString(key.c_str(), &status));
			if (status == prt::STATUS_OK) {
				pcu::appendToRhinoString(*pText, valueStr);
				if (status == prt::STATUS_OK)
					return true;
				else {
					LOG_ERR << "Impossible to get default value for rule attribute: " << key
					        << " with error: " << prt::getStatusDescription(status);
				}
			}
		}
	}

	return false;
}
