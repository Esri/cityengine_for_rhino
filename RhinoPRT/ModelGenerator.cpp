#include "ModelGenerator.h"

#include "Logger.h"

ModelGenerator::ModelGenerator() {

	// Cache initialization
	mCache = (pcu::CachePtr)prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);
}

bool ModelGenerator::initResolveMap() {
	if (!mRulePkg.empty()) {
		const std::string rpkURI = pcu::toFileURI(mRulePkg);
		prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
		try {
			auto converted_str = pcu::toUTF16FromUTF8(rpkURI).c_str();
			mResolveMap.reset(prt::createResolveMap(converted_str, nullptr, &status));

			return status == prt::STATUS_OK;
		}
		catch (std::exception& e) {
			LOG_ERR << L"CAUGHT EXCEPTION: " << e.what() << std::endl;
		}
	}
	return false;
}

RuleAttributes ModelGenerator::updateRuleFiles(const std::wstring rulePkg) {
	mRulePkg = rulePkg;

	// Reset the rule infos
	mRuleAttributes.clear();
	mRuleFile.clear();
	mStartRule.clear();

	// Get the resolve map
	bool status = initResolveMap();
	if (!status) {
		LOG_ERR << "failed to get resolve map from rule package " << mRulePkg << std::endl;
		return {};
	}

	// Extract the rule package info.
	mRuleFile = pcu::getRuleFileEntry(mResolveMap);
	if (mRuleFile.empty()) {
		LOG_ERR << "Could not find rule file in rule package " << mRulePkg;
		return {};
	}

	// To create the ruleFileInfo, we first need the ruleFileURI
	const wchar_t* ruleFileURI = mResolveMap->getString(mRuleFile.c_str());
	if (ruleFileURI == nullptr) {
		LOG_ERR << "could not find rule file URI in resolve map of rule package " << mRulePkg;
		return {};
	}

	// Create RuleFileInfo
	prt::Status infoStatus = prt::STATUS_UNSPECIFIED_ERROR;
	mRuleFileInfo = pcu::RuleFileInfoPtr(prt::createRuleFileInfo(ruleFileURI, mCache.get(), &infoStatus));
	if (!mRuleFileInfo || infoStatus != prt::STATUS_OK) {
		LOG_ERR << "could not get rule file info from rule file " << mRuleFile;
		return {};
	}

	mStartRule = pcu::detectStartRule(mRuleFileInfo);

	// Fill the list of rule attributes
	mRuleAttributes = getRuleAttributes(mRuleFile, *mRuleFileInfo.get());
	return mRuleAttributes;
}

std::vector<GeneratedModel> ModelGenerator::generateModel(const std::vector<InitialShape>& initial_geom,
	std::vector<pcu::ShapeAttributes>& shapeAttributes,
	const std::wstring& geometryEncoderName,
	const pcu::EncoderOptions& geometryEncoderOptions,
	pcu::AttributeMapBuilderPtr& aBuilder) {
	mInitialShapesBuilders.resize(initial_geom.size());

	// Initial shapes initializing
	for (size_t i = 0; i < initial_geom.size(); ++i) {
		pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

		if (isb->setGeometry(initial_geom[i].getVertices(), initial_geom[i].getVertexCount(), initial_geom[i].getIndices(),
			initial_geom[i].getIndexCount(), initial_geom[i].getFaceCounts(), initial_geom[i].getFaceCountsCount()) != prt::STATUS_OK) {

			LOG_ERR << "invalid initial geometry";

			mValid = false;
		}

		if (mValid) {
			mInitialShapesBuilders[i] = std::move(isb);
		}
	}

	if (!mValid) {
		LOG_ERR << "invalid ModelGenerator instance.";
		return {};
	}

	if ((shapeAttributes.size() != 1) && (shapeAttributes.size() < mInitialShapesBuilders.size())) {
		LOG_ERR << "not enough shape attributes dictionaries defined.";
		return {};
	}
	else if (shapeAttributes.size() > mInitialShapesBuilders.size()) {
		LOG_WRN << "number of shape attributes dictionaries defined greater than number of initial shapes given."
			<< std::endl;
	}

	std::vector<GeneratedModel> new_geometry;
	new_geometry.reserve(mInitialShapesBuilders.size());

	try {


		if (!mRulePkg.empty()) {
			LOG_INF << "using rule package " << mRulePkg << std::endl;

			if (!mResolveMap || mRuleFile.empty() || !mRuleFileInfo) {
				LOG_ERR << "Rule package not processed correcty." << std::endl;
				return {};
			}
		}

		// Initial shapes
		std::vector<const prt::InitialShape*> initialShapes(mInitialShapesBuilders.size());
		std::vector<pcu::InitialShapePtr> initialShapePtrs(mInitialShapesBuilders.size());
		std::vector<pcu::AttributeMapPtr> convertedShapeAttrVec(mInitialShapesBuilders.size());
		setAndCreateInitialShape(aBuilder, shapeAttributes, initialShapes, initialShapePtrs, convertedShapeAttrVec);

		if (!mEncoderBuilder)
			mEncoderBuilder.reset(prt::AttributeMapBuilder::create());

		if (!geometryEncoderName.empty())
			initializeEncoderData(geometryEncoderName, geometryEncoderOptions);

		std::vector<const wchar_t*> encoders;
		encoders.reserve(3);
		std::vector<const prt::AttributeMap*> encodersOptions;
		encodersOptions.reserve(3);

		getRawEncoderDataPointers(encoders, encodersOptions);

		if (mEncodersNames[0] == ENCODER_ID_RHINO) {
			pcu::RhinoCallbacksPtr roc{ std::make_unique<RhinoCallbacks>(mInitialShapesBuilders.size()) };

			// GENERATE!
			const prt::Status genStat =
				prt::generate(initialShapes.data(), initialShapes.size(), nullptr, encoders.data(), encoders.size(),
					encodersOptions.data(), roc.get(), mCache.get(), nullptr);

			if (genStat != prt::STATUS_OK) {
				LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' ("
					<< genStat << ")";
				return {};
			}

			for (size_t idx = 0; idx < mInitialShapesBuilders.size(); ++idx) {
				new_geometry.emplace_back(idx, roc->getVertices(idx), roc->getIndices(idx), roc->getFaces(idx),
					roc->getReport(idx));
			}
		}
		else {
			//TODO
		}
	}
	catch (const std::exception& e) {
		LOG_ERR << "caught exception: " << e.what();
		return {};
	}
	catch (...) {
		LOG_ERR << "caught unknown exception.";
		return {};
	}

	return new_geometry;
}

void ModelGenerator::setAndCreateInitialShape(pcu::AttributeMapBuilderPtr& aBuilder,
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
		extractMainShapeAttributes(aBuilder, shapeAttr, ruleF, startR, randomS, shapeN, convertedShapeAttr[i]);

		mInitialShapesBuilders[i]->setAttributes(ruleF.c_str(), startR.c_str(), randomS, shapeN.c_str(), convertedShapeAttr[i].get(),
			mResolveMap.get());

		initShapesPtrs[i].reset(mInitialShapesBuilders[i]->createInitialShape());
		initShapes[i] = initShapesPtrs[i].get();
	}
}

void ModelGenerator::initializeEncoderData(const std::wstring & encName, const pcu::EncoderOptions & encOpt) {
	mEncodersNames.clear();
	mEncodersOptionsPtr.clear();

	mEncodersNames.push_back(encName);
	const pcu::AttributeMapPtr encOptions{ pcu::createAttributeMapForEncoder(encOpt, *mEncoderBuilder) };
	mEncodersOptionsPtr.push_back(createValidatedOptions(encName.c_str(), encOptions));

	// TODO: initialise print and error encoder data?
}

void ModelGenerator::getRawEncoderDataPointers(std::vector<const wchar_t*>& allEnc, std::vector<const prt::AttributeMap*>& allEncOpt) {
	if (mEncodersNames[0] == ENCODER_ID_RHINO) {
		allEnc.clear();
		allEncOpt.clear();

		allEnc.push_back(mEncodersNames[0].c_str());
		allEncOpt.push_back(mEncodersOptionsPtr[0].get());
	}
	else {
		// TODO: implement for other encoders
	}
}

void ModelGenerator::extractMainShapeAttributes(pcu::AttributeMapBuilderPtr& aBuilder, const pcu::ShapeAttributes& shapeAttr, std::wstring & ruleFile, std::wstring & startRule,
	int32_t & seed, std::wstring & shapeName, pcu::AttributeMapPtr & convertShapeAttr)
{
	//convertShapeAttr = pcu::createAttributeMapForShape(shapeAttr, *(pcu::AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())));
	convertShapeAttr = pcu::createAttributeMapForShape(shapeAttr, *aBuilder.get());

	if (convertShapeAttr) {
		if (convertShapeAttr->hasKey(L"ruleFile") &&
			convertShapeAttr->getType(L"ruleFile") == prt::AttributeMap::PT_STRING)
			ruleFile = convertShapeAttr->getString(L"ruleFile");
		if (convertShapeAttr->hasKey(L"startRule") &&
			convertShapeAttr->getType(L"startRule") == prt::AttributeMap::PT_STRING)
			startRule = convertShapeAttr->getString(L"startRule");
		if (convertShapeAttr->hasKey(L"seed") && convertShapeAttr->getType(L"seed") == prt::AttributeMap::PT_INT)
			seed = convertShapeAttr->getInt(L"seed");
		if (convertShapeAttr->hasKey(L"shapeName") &&
			convertShapeAttr->getType(L"shapeName") == prt::AttributeMap::PT_STRING)
			shapeName = convertShapeAttr->getString(L"shapeName");
	}
}
