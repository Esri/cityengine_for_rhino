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

#include <algorithm>
#include <array>
#include <cassert>
#include <filesystem>
#include <future>

namespace {

constexpr const wchar_t* ENCODER_ID_RHINO = L"com.esri.rhinoprt.RhinoEncoder";
constexpr const wchar_t* ENCODER_ID_CGA_ERROR = L"com.esri.prt.core.CGAErrorEncoder";
constexpr const wchar_t* ENCODER_ID_CGA_PRINT = L"com.esri.prt.core.CGAPrintEncoder";

constexpr std::array<const wchar_t*, 3> ALL_ENCODER_IDS = {ENCODER_ID_RHINO, ENCODER_ID_CGA_ERROR,
                                                           ENCODER_ID_CGA_PRINT};

constexpr const wchar_t* FILE_CGA_ERROR = L"CGAErrors.txt";
constexpr const wchar_t* FILE_CGA_PRINT = L"CGAPrint.txt";

constexpr const wchar_t* RESOLVEMAP_EXTRACTION_PREFIX = L"rhino_prt";
constexpr const wchar_t* ENCODER_ID_CGA_EVALATTR = L"com.esri.prt.core.AttributeEvalEncoder";

pcu::AttributeMapPtr getAttrEvalEncoderInfo() {
	const pcu::EncoderInfoPtr encInfo(prt::createEncoderInfo(ENCODER_ID_CGA_EVALATTR));
	const prt::AttributeMap* encOpts = nullptr;
	encInfo->createValidatedOptionsAndStates(nullptr, &encOpts);
	return pcu::AttributeMapPtr(encOpts);
}

template <typename T, typename D>
std::vector<T*> toRawPtrs(const std::vector<std::unique_ptr<T, D>>& smartPtrs) {
	std::vector<T*> rawPtrs(smartPtrs.size());
	std::transform(smartPtrs.begin(), smartPtrs.end(), rawPtrs.begin(), [](const auto& o) { return o.get(); });
	return rawPtrs;
}

std::vector<const wchar_t*> toRawPtrs(const std::vector<std::wstring>& strings) {
	std::vector<const wchar_t*> rawPtrs(strings.size());
	std::transform(strings.begin(), strings.end(), rawPtrs.begin(), [](const auto& s) { return s.c_str(); });
	return rawPtrs;
}

std::vector<GeneratedModelPtr> batchGenerate(const std::vector<pcu::InitialShapePtr>& initialShapes,
                                             const std::vector<const prt::AttributeMap*>& encoderOptions,
                                             prt::Cache* prtCache) {
	const size_t nThreads = std::min<size_t>(std::thread::hardware_concurrency(), initialShapes.size());
	const size_t isRangeSize = static_cast<size_t>(std::ceil(initialShapes.size() / nThreads));
	// TODO: if nThreads is smaller than cpu cores we can enable multi-threaded generation within a shape

	std::vector<prt::InitialShape const*> rawInitialShapes = toRawPtrs<const prt::InitialShape>(initialShapes);

	std::vector<pcu::RhinoCallbacksPtr> callbacks(nThreads); // one callback per thread
	std::generate(callbacks.begin(), callbacks.end(),
	              [&isRangeSize]() { return std::make_unique<RhinoCallbacks>(isRangeSize); });
	std::vector<RhinoCallbacks*> rawCallbacks = toRawPtrs<RhinoCallbacks>(callbacks);

	std::vector<std::future<void>> futures;
	futures.reserve(nThreads);
	for (int8_t ti = 0; ti < nThreads; ti++) {
		auto f = std::async(std::launch::async, [&, ti] {
			const size_t isStartPos = ti * isRangeSize;
			const size_t isPastEndPos = (ti < nThreads - 1) ? (ti + 1) * isRangeSize : initialShapes.size();
			const size_t isActualRangeSize = isPastEndPos - isStartPos;
			prt::InitialShape const* const* isRangeStart = &rawInitialShapes[isStartPos];

			LOG_DBG << "thread " << ti << ": #is = " << isActualRangeSize;

			const prt::Status generateStatus =
			        prt::generate(isRangeStart, isActualRangeSize, nullptr, ALL_ENCODER_IDS.data(),
			                      ALL_ENCODER_IDS.size(), encoderOptions.data(), rawCallbacks[ti], prtCache, nullptr);

			if (generateStatus != prt::STATUS_OK) {
				LOG_WRN << "generation (batch " << ti << ") failed with status: '"
				        << prt::getStatusDescription(generateStatus) << "' (" << generateStatus << ")";
			}
		});
		futures.emplace_back(std::move(f));
	}
	std::for_each(futures.begin(), futures.end(), [](std::future<void>& f) { f.wait(); });

	std::vector<GeneratedModelPtr> generatedModels(initialShapes.size());
	for (size_t ri = 0; ri < callbacks.size(); ri++) {
		const std::vector<GeneratedModelPtr>& models = callbacks[ri]->getModels();
		for (size_t mi = 0; mi < models.size(); mi++) {
			generatedModels[ri * isRangeSize + mi] = models[mi];
		}
	}

	return generatedModels;
}

} // namespace

ModelGenerator::ModelGenerator() {
	pcu::AttributeMapBuilderPtr optionsBuilder(prt::AttributeMapBuilder::create());

	optionsBuilder->setString(L"name", FILE_CGA_ERROR);
	const pcu::AttributeMapPtr errOptions(optionsBuilder->createAttributeMapAndReset());
	mCGAErrorOptions = createValidatedOptions(ENCODER_ID_CGA_ERROR, errOptions);

	optionsBuilder->setString(L"name", FILE_CGA_PRINT);
	const pcu::AttributeMapPtr printOptions(optionsBuilder->createAttributeMapAndReset());
	mCGAPrintOptions = createValidatedOptions(ENCODER_ID_CGA_PRINT, printOptions);
}

void ModelGenerator::updateRuleFiles(const std::wstring& rulePkg) {
	try {
		const ResolveMap::ResolveMapCache::LookupResult lookup = PRTContext::get()->getResolveMap(rulePkg);
		if (lookup.second == ResolveMap::ResolveMapCache::CacheStatus::HIT && rulePkg == mRulePkg) {
			// resolvemap already exists and the rule file was not changed, no need to update.
			return;
		}
		mResolveMap = lookup.first;
	}
	catch (std::exception&) {
		mResolveMap.reset();
		mRuleFile.clear();
		mStartRule.clear();
		mRuleAttributes.clear();
		throw;
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
		LOG_ERR << "Could not find rule file URI in resolve map of rule package " << mRulePkg;
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

bool ModelGenerator::evalDefaultAttributes(const std::vector<RawInitialShape>& rawInitialShapes,
                                           std::vector<pcu::ShapeAttributes>& shapeAttributes) {
	// setup encoder options for attribute evaluation encoder
	constexpr const wchar_t* encs[] = {ENCODER_ID_CGA_EVALATTR};
	constexpr size_t encsCount = sizeof(encs) / sizeof(encs[0]);
	const pcu::AttributeMapPtr encOpts = getAttrEvalEncoderInfo();
	const prt::AttributeMap* encsOpts[] = {encOpts.get()};

	const size_t numShapes = rawInitialShapes.size();

	pcu::AttributeMapBuilderVector attribMapBuilders;
	attribMapBuilders.reserve(numShapes);

	for (size_t isIdx = 0; isIdx < numShapes; ++isIdx) {
		pcu::AttributeMapBuilderPtr amb(prt::AttributeMapBuilder::create());
		attribMapBuilders.emplace_back(std::move(amb));
	}

	std::vector<pcu::InitialShapePtr> initialShapes;
	std::vector<pcu::AttributeMapPtr> initialShapeAttributes; // put here to ensure same life time as initialShapes
	if (!createInitialShapes(rawInitialShapes, shapeAttributes, attribMapBuilders, initialShapes,
	                         initialShapeAttributes))
		return false;

	// run generate
	AttrEvalCallbacks aec(attribMapBuilders, mRuleFileInfo);
	const std::vector<prt::InitialShape const*> rawInitialShapePtrs = toRawPtrs<const prt::InitialShape>(initialShapes);
	const prt::Status status = prt::generate(rawInitialShapePtrs.data(), rawInitialShapePtrs.size(), nullptr, encs,
	                                         encsCount, encsOpts, &aec, PRTContext::get()->mPRTCache.get(), nullptr);
	if (status != prt::STATUS_OK) {
		LOG_ERR << "Failed to get default rule attributes: '" << prt::getStatusDescription(status) << "' (" << status
		        << ")";
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

bool ModelGenerator::createInitialShapes(const std::vector<RawInitialShape>& rawInitialShapes,
                                         const std::vector<pcu::ShapeAttributes>& shapeAttributes,
                                         pcu::AttributeMapBuilderVector& aBuilders,
                                         std::vector<pcu::InitialShapePtr>& initialShapes,
                                         std::vector<pcu::AttributeMapPtr>& initialShapesAttributes) const {

	pcu::InitialShapeBuilderPtr isb(prt::InitialShapeBuilder::create());
	initialShapes.reserve(rawInitialShapes.size());
	initialShapesAttributes.reserve(rawInitialShapes.size());

	for (size_t i = 0; i < rawInitialShapes.size(); ++i) {
		const RawInitialShape& ris = rawInitialShapes[i];

		const prt::Status geometryStatus =
		        isb->setGeometry(ris.getVertices(), ris.getVertexCount(), ris.getIndices(), ris.getIndexCount(),
		                         ris.getFaceCounts(), ris.getFaceCountsCount());
		if (geometryStatus != prt::STATUS_OK) {
			LOG_ERR << "Encountered invalid initial shape geometry: " << prt::getStatusDescription(geometryStatus);
			return false;
		}

		// TODO: is this check necessary?
		pcu::ShapeAttributes shapeAttr = (shapeAttributes.size() > i) ? shapeAttributes[0] : shapeAttributes[i];

		// Set to default values
		std::wstring ruleF = mRuleFile;
		std::wstring startR = mStartRule;
		int32_t randomS = mSeed;
		std::wstring shapeN = mShapeName;

		pcu::AttributeMapPtr initialShapeAttributes;
		extractMainShapeAttributes(aBuilders[i], shapeAttr, ruleF, startR, randomS, shapeN, initialShapeAttributes);

		const prt::Status attributeStatus = isb->setAttributes(ruleF.c_str(), startR.c_str(), randomS, shapeN.c_str(),
		                                                       initialShapeAttributes.get(), mResolveMap.get());
		if (attributeStatus != prt::STATUS_OK) {
			LOG_ERR << "Failed to set initial shape attributes: " << prt::getStatusDescription(attributeStatus);
			return false;
		}

		prt::Status creationStatus = prt::STATUS_UNSPECIFIED_ERROR;
		pcu::InitialShapePtr initialShape(isb->createInitialShapeAndReset(&creationStatus));
		if (creationStatus != prt::STATUS_OK) {
			LOG_ERR << "Failed to create initial shape: " << prt::getStatusDescription(creationStatus);
			return false;
		}

		initialShapes.emplace_back(std::move(initialShape));
		initialShapesAttributes.emplace_back(std::move(initialShapeAttributes));
	}

	return true;
}

std::vector<GeneratedModelPtr> ModelGenerator::generateModel(const std::vector<RawInitialShape>& rawInitialShapes,
                                                             const std::vector<pcu::ShapeAttributes>& shapeAttributes,
                                                             pcu::AttributeMapBuilderVector& aBuilders) {
	if ((shapeAttributes.size() != 1) && (shapeAttributes.size() < rawInitialShapes.size())) {
		LOG_ERR << "Not enough shape attributes dictionaries defined.";
		return {};
	}
	else if (shapeAttributes.size() > rawInitialShapes.size()) {
		LOG_WRN << "Number of shape attributes dictionaries defined greater than number of initial shapes given.";
	}

	if (!mRulePkg.empty()) {
		LOG_INF << "using rule package " << mRulePkg;
		if (!mResolveMap || mRuleFile.empty() || !mRuleFileInfo) {
			LOG_ERR << "Rule package not processed correcty.";
			return {};
		}
	}

	try {
		std::vector<pcu::InitialShapePtr> initialShapes;
		std::vector<pcu::AttributeMapPtr> initialShapeAttributes; // put here to ensure same life time as initialShapes
		if (!createInitialShapes(rawInitialShapes, shapeAttributes, aBuilders, initialShapes, initialShapeAttributes))
			return {};

		const std::vector<const prt::AttributeMap*> encoderOptions = {mRhinoEncoderOptions.get(),
		                                                              mCGAErrorOptions.get(), mCGAPrintOptions.get()};

		const std::vector<GeneratedModelPtr> generatedModels =
		        batchGenerate(initialShapes, encoderOptions, PRTContext::get()->mPRTCache.get());

		return generatedModels;
	}
	catch (const std::exception& e) {
		LOG_ERR << "caught exception: " << e.what();
	}
	catch (...) {
		LOG_ERR << "caught unknown exception.";
	}

	return {};
}

void ModelGenerator::updateEncoderOptions(bool emitMaterials) {
	pcu::AttributeMapBuilderPtr optionsBuilder(prt::AttributeMapBuilder::create());
	optionsBuilder->setBool(L"emitMaterials", emitMaterials);
	pcu::AttributeMapPtr rawOptions(optionsBuilder->createAttributeMap());
	mRhinoEncoderOptions = pcu::createValidatedOptions(ENCODER_ID_RHINO, rawOptions);
}

void ModelGenerator::extractMainShapeAttributes(pcu::AttributeMapBuilderPtr& aBuilder,
                                                const pcu::ShapeAttributes& shapeAttr, std::wstring& ruleFile,
                                                std::wstring& startRule, int32_t& seed, std::wstring& shapeName,
                                                pcu::AttributeMapPtr& convertShapeAttr) const {
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
