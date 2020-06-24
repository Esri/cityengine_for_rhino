#include "wrap.h"

#include <memory>
#include <vector>
#include <numeric>
#include <iostream>

/**
 * commonly used constants
 */
const std::wstring ENCODER_ID_RHINO = L"com.esri.rhinoprt.RhinoEncoder";

InitialShape::InitialShape() {}
InitialShape::InitialShape(const std::vector<double> &vertices): mVertices(vertices) {
	mIndices.resize(vertices.size() / 3);
	std::iota(std::begin(mIndices), std::end(mIndices), 0);

	mIndices[1] = 2;
	mIndices[2] = 3;
	mIndices[3] = 1;

	mFaceCounts.resize(1, (uint32_t)mIndices.size());
}

GeneratedModel::GeneratedModel(const size_t& initialShapeIdx, const std::vector<double>& vert, const std::vector<uint32_t>& indices,
							   const std::vector<uint32_t>& face, const std::map<std::string, std::string>& rep): 
	mInitialShapeIndex(initialShapeIdx), mVertices(vert), mIndices(indices), mFaces(face), mReport(rep) {}

ModelGenerator::ModelGenerator(const std::vector<InitialShape>& initial_geom) {
	mInitialShapesBuilders.resize(initial_geom.size());

	// Cache initialization
	mCache = (pcu::CachePtr)prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT);

	// Initial shapes initializing
	for (size_t i = 0; i < initial_geom.size(); ++i) {
		pcu::InitialShapeBuilderPtr isb{ prt::InitialShapeBuilder::create() };

		if (isb->setGeometry(initial_geom[i].getVertices(), initial_geom[i].getVertexCount(), initial_geom[i].getIndices(),
			initial_geom[i].getIndexCount(), initial_geom[i].getFaceCounts(), initial_geom[i].getFaceCountsCount()) != prt::STATUS_OK) {

			//TODO: add logger
			//LOG_ERR << "invalid initial geometry";
			std::cout << "[ModelGenerator][Error] invalid initial geometry" << std::endl;

			mValid = false;
		}

		if (mValid) {
			mInitialShapesBuilders[i] = std::move(isb);
		}
	}
}

std::vector<GeneratedModel> ModelGenerator::generateModel(std::vector<pcu::ShapeAttributes>& shapeAttributes,
														  const std::string& rulePackagePath,
														  const std::wstring& geometryEncoderName,
														  const pcu::EncoderOptions& geometryEncoderOptions) {
	if (!mValid) {
		std::cout << "[ModelGenerator][ERROR] invalid ModelGenerator instance." << std::endl;
		//LOG_ERR << "invalid ModelGenerator instance.";
		return {};
	}

	if ((shapeAttributes.size() != 1) && (shapeAttributes.size() < mInitialShapesBuilders.size())) {
		std::cout << "[ModelGenerator][ERROR] not enough shape attributes dictionaries defined." << std::endl;
		//LOG_ERR << "not enough shape attributes dictionaries defined.";
		return {};
	}
	else if (shapeAttributes.size() > mInitialShapesBuilders.size()) {
		std::cout << "[ModelGenerator][WARNING] number of shape attributes dictionaries defined greater than number of initial shapes given."
			<< std::endl;
		//LOG_WRN << "number of shape attributes dictionaries defined greater than number of initial shapes given."
		//	<< std::endl;
	}

	std::vector<GeneratedModel> new_geometry;
	new_geometry.reserve(mInitialShapesBuilders.size());

	try {
		// TODO: check that prt is initialized

		// TODO: Resolve map thing
		if (!rulePackagePath.empty()) {
			//LOG_INF << "using rule package " << rulePackagePath << std::endl;

			const std::string rpkURI = pcu::toFileURI(rulePackagePath);
			prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
			try {
				mResolveMap.reset(prt::createResolveMap(pcu::toUTF16FromUTF8(rpkURI).c_str(), nullptr, &status));
			}
			catch (std::exception& e) {
				std::cout << "CAUGHT EXCEPTION: " << e.what() << std::endl;
			}

			if (!mResolveMap || (status != prt::STATUS_OK)) {
				return {};
			}
		}

		// Initial shapes
		std::vector<const prt::InitialShape*> initialShapes(mInitialShapesBuilders.size());
		std::vector<pcu::InitialShapePtr> initialShapePtrs(mInitialShapesBuilders.size());
		std::vector<pcu::AttributeMapPtr> convertedShapeAttrVec(mInitialShapesBuilders.size());
		setAndCreateInitialShape(shapeAttributes, initialShapes, initialShapePtrs, convertedShapeAttrVec);

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

			// DEBUG
			auto startRule = initialShapes[0]->getStartRule();
			auto ruleFile = initialShapes[0]->getRuleFile();
			auto attrMap = initialShapes[0]->getAttributeMap();

			// GENERATE!
			const prt::Status genStat =
				prt::generate(initialShapes.data(), initialShapes.size(), nullptr, encoders.data(), encoders.size(),
					encodersOptions.data(), roc.get(), mCache.get(), nullptr);

			if (genStat != prt::STATUS_OK) {
				//LOG_ERR << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' ("
				//	<< genStat << ")";
				std::cout << "prt::generate() failed with status: '" << prt::getStatusDescription(genStat) << "' ("
					<< genStat << ")" << std::endl;
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
		//LOG_ERR << "caught exception: " << e.what();
		std::cout << "caught exception: " << e.what() << std::endl;
		return {};
	}
	catch (...) {
		//LOG_ERR << "caught unknown exception.";
		std::cout << "caught unknown exception." << std::endl;
		return {};
	}

	return new_geometry;
}

void ModelGenerator::setAndCreateInitialShape(const std::vector<pcu::ShapeAttributes>& shapesAttr,
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
		extractMainShapeAttributes(shapeAttr, ruleF, startR, randomS, shapeN, convertedShapeAttr[i]);

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

void ModelGenerator::extractMainShapeAttributes(const pcu::ShapeAttributes& shapeAttr, std::wstring & ruleFile, std::wstring & startRule,
												int32_t & seed, std::wstring & shapeName, pcu::AttributeMapPtr & convertShapeAttr)
{
	convertShapeAttr = pcu::createAttributeMapForShape(shapeAttr, *(pcu::AttributeMapBuilderPtr(prt::AttributeMapBuilder::create())));

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
