#include "RhinoPRT.h"

namespace RhinoPRT {

	// Global PRT handle
	RhinoPRTAPI& get() 
	{ 
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
		return mRuleAttributes.size();
	}

	RuleAttributes RhinoPRTAPI::GetRuleAttributes() {
		return mRuleAttributes;
	}

	void RhinoPRTAPI::SetRPKPath(const std::wstring &rpk_path) {
		if (mPackagePath == rpk_path) return;
		
		mPackagePath = rpk_path;

		// initialize the resolve map and rule infos here. Create the vector of rule attributes.
		if (!mModelGenerator)
			mModelGenerator = std::unique_ptr<ModelGenerator>(new ModelGenerator());

		mRuleAttributes = mModelGenerator->updateRuleFiles(mPackagePath);

		// Also create the attribute map builder that will receive the rule attributes.
		mAttrBuilder.reset(prt::AttributeMapBuilder::create());
	}

	void RhinoPRTAPI::AddInitialShape(const std::vector<InitialShape>& shapes) {

		// get the shape attributes data from ModelGenerator
		std::wstring rulef = mModelGenerator->getRuleFile();
		std::wstring ruleN = mModelGenerator->getStartingRule();
		std::wstring shapeN = mModelGenerator->getDefaultShapeName();
		int seed = 555; // TODO: compute seed?

		mShapes.reserve(shapes.size());
		mAttributes.reserve(shapes.size());

		mShapes.insert(mShapes.end(), shapes.begin(), shapes.end());

		auto shapeAttr = pcu::ShapeAttributes(rulef, ruleN, shapeN, seed);
		mAttributes.resize(shapes.size(), shapeAttr);
	}

	void RhinoPRTAPI::ClearInitialShapes() {
		mShapes.clear();
		mGeneratedModels.clear();
		mAttributes.clear();

		mGroupedReports.clear();
	}

	std::vector<GeneratedModel> RhinoPRTAPI::GenerateGeometry() {
		mGeneratedModels = mModelGenerator->generateModel(mShapes, mAttributes, ENCODER_ID_RHINO, options, mAttrBuilder);
		return mGeneratedModels;
	}

	template<typename T>
	void RhinoPRTAPI::fillAttributeFromNode(const std::wstring& ruleName, const std::wstring& attrFullName, T value) {

		// Find the RuleAttribute object corresponding to the given attribute name.
		auto reverseLookupAttribute = [this](const std::wstring gh_attrFullName) {
			auto it = std::find_if(mRuleAttributes.begin(), mRuleAttributes.end(), [&gh_attrFullName](const auto& ra) {return ra.mFullName == gh_attrFullName; });
			if (it != mRuleAttributes.end()) return *it;
			return RULE_NOT_FOUND;
		};

		const RuleAttribute rule = reverseLookupAttribute(attrFullName);
		assert(!rule.mFullName.empty()); // Check if the rule was found

		// If the attribute is found, register the value in the attribute map builder
		//TODO: check for difference with default value, only add the attribute if it is the case.
		setRuleAttributeValue(rule, value);
	}

	template<typename T>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, T value) {
		LOG_WRN << L"Ignored unsupported attribute '" << rule.mFullName << "' type: " << T << " ---- " << rule.mType << " of rule file: '" << rule.mRuleFile << "'" << std::endl;
	}

	// Specific instantiations of setRuleAttributeValue.

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, double value) {
		if (rule.mType == prt::AAT_FLOAT) {
			mAttrBuilder->setFloat(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set a double value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, int value) {
		if (rule.mType == prt::AAT_INT) {
			mAttrBuilder->setInt(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set an int value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, bool value) {
		if (rule.mType == prt::AAT_BOOL) {
			mAttrBuilder->setBool(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set a boolean value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, std::wstring value) {
		if (rule.mType == prt::AAT_STR) {
			mAttrBuilder->setString(rule.mFullName.c_str(), value.c_str());
		}
		else {
			LOG_ERR << L"Trying to set a wstring to an attribute of type " << rule.mType << std::endl;
		}
	}

	///<summary>
	/// NOT USED ANYMORE BUT WE KEEP IT FOR NOW IN CASE IT IS USEFUL IN THE FUTURE.
	///The reports of all generated models will be grouped together by keys. 
	///Meaning that all reports with the same key will be added to a vector.
	///</summary>
	///<return>The total number of reports.</return>
	int RhinoPRTAPI::groupReportsByKeys() {
		if (mGeneratedModels.empty()) return 0;

		mGroupedReports.clear();

		for (const auto& model : mGeneratedModels) {
			for (const auto& report : model.getReport()) {
				mGroupedReports.add(report.second, model.getInitialShapeIndex());
			}
		}

		return mGroupedReports.getReportCount();
	}

	Reporting::ReportsVector RhinoPRTAPI::getReportsOfModel(int initialShapeID)
	{
		// find the report with given shape id.
		const auto found_reports = std::find_if(mGeneratedModels.begin(), mGeneratedModels.end(), 
			[&initialShapeID](const GeneratedModel& model) { return model.getInitialShapeIndex() == initialShapeID; });

		if (found_reports != mGeneratedModels.end()) {
			const auto& reports = found_reports->getReport();
			return Reporting::ToReportsVector(reports);
		}

		return Reporting::EMPTY_REPORTS;
	}

}

extern "C" {

	RHINOPRT_API void GetProductVersion(ON_wString* version_str)
	{
		std::string str(VER_FILE_VERSION_STR);
		version_str->Append(str.c_str(), str.size());
	}

	RHINOPRT_API bool InitializeRhinoPRT()
	{
		return RhinoPRT::get().InitializeRhinoPRT();
	}

	RHINOPRT_API void ShutdownRhinoPRT()
	{
		RhinoPRT::get().ShutdownRhinoPRT();
	}

	RHINOPRT_API void SetPackage(const wchar_t* rpk_path)
	{
		std::wstring str(rpk_path);
		RhinoPRT::get().SetRPKPath(str);
	}

	RHINOPRT_API bool AddMeshTest(ON_SimpleArray<const ON_Mesh*>* pMesh)
	{
		if (pMesh == nullptr) return false;

		std::vector<InitialShape> initShapes;
		for (int i = 0; i < pMesh->Count(); ++i) {
			initShapes.push_back(InitialShape(**pMesh->At(i))); // emplace_back
		}

		RhinoPRT::get().AddInitialShape(initShapes);
		return true;
	}

	RHINOPRT_API void ClearInitialShapes()
	{
		RhinoPRT::get().ClearInitialShapes();
	}

	RHINOPRT_API bool GenerateTest(ON_SimpleArray<ON_Mesh*>* pMeshArray)
	{
		if (pMeshArray == nullptr) {
			LOG_ERR << L"Aborting generation, given a null Mesh array.";
			return false;
		}

		auto meshes = RhinoPRT::get().GenerateGeometry();

		if (meshes.size() == 0) {
			LOG_ERR << L"Generation failed, returned an empty models array.";
			return false;
		}

		for (const auto& mesh : meshes) {
			const auto on_mesh = mesh.getMeshFromGenModel();
			pMeshArray->Append(new ON_Mesh(on_mesh)); // must be freed my the caller of this function.
		}

		return true;
	}

	RHINOPRT_API int GetRuleAttributesCount()
	{
		return RhinoPRT::get().GetRuleAttributeCount();
	}

	RHINOPRT_API bool GetRuleAttribute(int attrIdx, wchar_t* rule, int rule_size, wchar_t* name, int name_size, wchar_t* nickname, int nickname_size, prt::AnnotationArgumentType* type)
	{
		RuleAttributes ruleAttributes = RhinoPRT::get().GetRuleAttributes();

		if (attrIdx >= ruleAttributes.size()) return false;

		wcscpy_s(rule, rule_size, ruleAttributes[attrIdx].mRuleFile.c_str());
		wcscpy_s(name, name_size, ruleAttributes[attrIdx].mFullName.c_str());
		wcscpy_s(nickname, nickname_size, ruleAttributes[attrIdx].mNickname.c_str());
		*type = ruleAttributes[attrIdx].mType;

		return true;
	}

	RHINOPRT_API void SetRuleAttributeDouble(const wchar_t* rule, const wchar_t* fullName, double value)
	{
		RhinoPRT::get().fillAttributeFromNode<double>(std::wstring(rule), std::wstring(fullName), value);
	}

	RHINOPRT_API void SetRuleAttributeBoolean(const wchar_t* rule, const wchar_t* fullName, bool value)
	{
		RhinoPRT::get().fillAttributeFromNode<bool>(std::wstring(rule), std::wstring(fullName), value);
	}

	RHINOPRT_API void SetRuleAttributeInteger(const wchar_t* rule, const wchar_t* fullName, int value)
	{
		RhinoPRT::get().fillAttributeFromNode<int>(std::wstring(rule), std::wstring(fullName), value);
	}

	RHINOPRT_API void SetRuleAttributeString(const wchar_t* rule, const wchar_t* fullName, const wchar_t* value)
	{
		RhinoPRT::get().fillAttributeFromNode<std::wstring>(std::wstring(rule), std::wstring(fullName), std::wstring(value));
	}

	RHINOPRT_API void GetReports(int initialShapeId, ON_ClassArray<ON_wString>* pKeysArray, 
		ON_SimpleArray<double>* pDoubleReports, 
		ON_SimpleArray<bool>* pBoolReports,
		ON_ClassArray<ON_wString>* pStringReports)
	{
		auto reports = RhinoPRT::get().getReportsOfModel(initialShapeId);

		/*
		left.float	-> right.all OK
		left.bool	-> right.float OK
					-> right.bool OK
					-> right.string OK
		left.string -> right.all OK
		*/
		//Sort the reports by Type. The order is Double -> Bool -> String
		std::sort(reports.begin(), reports.end(), [](Reporting::ReportAttribute& left, Reporting::ReportAttribute& right) -> bool {
			if (left.mType == right.mType) return left.mReportName.compare(right.mReportName) < 0; // assuming case sensitivity. assuming two reports can't have the same name.
			if (left.mType == prt::AttributeMap::PrimitiveType::PT_FLOAT) return true;
			if (right.mType == prt::AttributeMap::PrimitiveType::PT_FLOAT) return false;
			if (left.mType == prt::AttributeMap::PrimitiveType::PT_STRING) return false;
			if(left.mType == prt::AttributeMap::PrimitiveType::PT_BOOL && right.mType == prt::AttributeMap::PrimitiveType::PT_STRING) return true;
			return false;
		});

		for (const auto& report : reports)
		{
			pKeysArray->Append(ON_wString(report.mReportName.c_str()));

			switch (report.mType)
			{
			case prt::AttributeMap::PrimitiveType::PT_FLOAT:
				pDoubleReports->Append(report.mDoubleReport);
				break;
			case prt::AttributeMap::PrimitiveType::PT_BOOL:
				pBoolReports->Append(report.mBoolReport);
				break;
			case prt::AttributeMap::PrimitiveType::PT_STRING:
				pStringReports->Append(ON_wString(report.mStringReport.c_str()));
				break;
			default:
				//REMOVE LAST KEY
				pKeysArray->Remove(pKeysArray->Count() - 1);
			}
		}
	}
}