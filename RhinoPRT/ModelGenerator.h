#pragma once

#include "utils.h"
#include "RhinoCallbacks.h"
#include "RuleAttributes.h"
#include "PRTUtilityModels.h"

constexpr wchar_t RESOLVEMAP_EXTRACTION_PREFIX[] = L"rhino_prt";

/**
* Entry point of the PRT. Is given an initial shape and rpk package, gives them to the PRT and gets the results.
*/
class ModelGenerator {
public:
	ModelGenerator();
	~ModelGenerator();

	std::vector<GeneratedModel> generateModel(const std::vector<InitialShape>& initial_geom,
		std::vector<pcu::ShapeAttributes>& shapeAttributes,
		const std::wstring& geometryEncoderName,
		const pcu::EncoderOptions& geometryEncoderOptions,
		pcu::AttributeMapBuilderPtr& aBuilder);

	bool initResolveMap();
	RuleAttributes updateRuleFiles(const std::wstring rulePkg);

	std::wstring getRuleFile(); 
	std::wstring getStartingRule(); 
	std::wstring getDefaultShapeName();
	inline const prt::ResolveMap* getResolveMap() { return mResolveMap.get(); };

private:
	pcu::CachePtr mCache;
	pcu::RuleFileInfoPtr mRuleFileInfo;
	pcu::ResolveMapPtr mResolveMap;
	pcu::AttributeMapBuilderPtr mEncoderBuilder;
	std::vector<pcu::InitialShapeBuilderPtr> mInitialShapesBuilders;
	std::vector<std::wstring> mEncodersNames;
	std::vector<pcu::AttributeMapPtr> mEncodersOptionsPtr;
	RuleAttributes mRuleAttributes;

	std::wstring mRulePkg;
	std::wstring mUnpackPath;
	std::wstring mRuleFile = L"bin/rule.cgb";
	std::wstring mStartRule = L"default$Lot";
	int32_t mSeed = 0;
	std::wstring mShapeName = L"Lot";

	bool mValid = true;

	void setAndCreateInitialShape(pcu::AttributeMapBuilderPtr& aBuilder,
		const std::vector<pcu::ShapeAttributes>& shapesAttr,
		std::vector<const prt::InitialShape*>& initShapes,
		std::vector<pcu::InitialShapePtr>& initShapesPtrs,
		std::vector<pcu::AttributeMapPtr>& convertedShapeAttr);

	void initializeEncoderData(const std::wstring& encName, const pcu::EncoderOptions& encOpt);

	void getRawEncoderDataPointers(std::vector<const wchar_t*>& allEnc,
		std::vector<const prt::AttributeMap*>& allEncOpt);

	void extractMainShapeAttributes(pcu::AttributeMapBuilderPtr& aBuilder, const pcu::ShapeAttributes& shapeAttr,
		std::wstring& ruleFile, std::wstring& startRule, int32_t& seed, std::wstring& shapeName,
		pcu::AttributeMapPtr& convertShapeAttr);
};