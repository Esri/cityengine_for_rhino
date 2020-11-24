#pragma once

#include "utils.h"
#include "RhinoCallbacks.h"
#include "RuleAttributes.h"
#include "PRTUtilityModels.h"
#include "ResolveMapCache.h"
#include "PRTContext.h"

/**
* Entry point of the PRT. Is given an initial shape and rpk package, gives them to the PRT and gets the results.
*/
class ModelGenerator {
public:

	void generateModel(const std::vector<InitialShape>& initial_geom,
		std::vector<pcu::ShapeAttributes>& shapeAttributes,
		const std::wstring& geometryEncoderName,
		const pcu::EncoderOptions& geometryEncoderOptions,
		pcu::AttributeMapBuilderPtr& aBuilder,
		std::vector<GeneratedModel>& generated_models);

	bool evalDefaultAttributes(const std::vector<InitialShape>& initial_geom,
		std::vector<pcu::ShapeAttributes>& shapeAttributes);

	ResolveMap::ResolveMapCache::CacheStatus initResolveMap(const std::experimental::filesystem::path& rpk);
	void updateRuleFiles(const std::wstring& rulePkg);

	RuleAttributes& getRuleAttributes() { return mRuleAttributes; }

	std::wstring getRuleFile(); 
	std::wstring getStartingRule(); 
	std::wstring getDefaultShapeName();
	inline const prt::ResolveMap* getResolveMap() { return mResolveMap.get(); };
	const pcu::AttributeMapPtrVector& getDefaultValueAttributeMap() { return mDefaultValuesMap; };

	bool getDefaultValueBoolean(const std::wstring key, bool* value);
	bool getDefaultValueNumber(const std::wstring key, double* value);
	bool getDefaultValueText(const std::wstring key, ON_wString* pText);

private:
	pcu::RuleFileInfoPtr mRuleFileInfo;
	pcu::ResolveMapSPtr mResolveMap;
	pcu::AttributeMapBuilderPtr mEncoderBuilder;
	std::vector<pcu::InitialShapeBuilderPtr> mInitialShapesBuilders;
	std::vector<std::wstring> mEncodersNames;
	std::vector<pcu::AttributeMapPtr> mEncodersOptionsPtr;
	RuleAttributes mRuleAttributes;

	// contains the rule attributes evaluated
	pcu::AttributeMapPtrVector mDefaultValuesMap;

	std::wstring mRulePkg;
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

	void setAndCreateInitialShape(pcu::AttributeMapBuilderVector& aBuilders,
		const std::vector<pcu::ShapeAttributes>& shapesAttr,
		std::vector<const prt::InitialShape*>& initShapes,
		std::vector<pcu::InitialShapePtr>& initShapesPtrs,
		std::vector<pcu::AttributeMapPtr>& convertedShapeAttr);

	void initializeEncoderData(const std::wstring& encName, const pcu::EncoderOptions& encOpt);

	void fillInitialShapeBuilder(const std::vector<InitialShape>& initial_geom);

	void createDefaultValueMaps(pcu::AttributeMapBuilderVector& ambv);

	void getRawEncoderDataPointers(std::vector<const wchar_t*>& allEnc,
		std::vector<const prt::AttributeMap*>& allEncOpt);

	void extractMainShapeAttributes(pcu::AttributeMapBuilderPtr& aBuilder, const pcu::ShapeAttributes& shapeAttr,
		std::wstring& ruleFile, std::wstring& startRule, int32_t& seed, std::wstring& shapeName,
		pcu::AttributeMapPtr& convertShapeAttr);
};