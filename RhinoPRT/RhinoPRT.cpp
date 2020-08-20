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

	const Reporting::ReportsVector RhinoPRTAPI::getReportsOfModel(int initialShapeID)
	{
		const auto& reports = mGeneratedModels.at(initialShapeID).getReport();

		return Reporting::ToReportsVector(reports);
	}

}

extern "C" {

	RHINOPRT_API bool InitializeRhinoPRT()
	{
		return RhinoPRT::get().InitializeRhinoPRT();
	}

	inline RHINOPRT_API void ShutdownRhinoPRT()
	{
		RhinoPRT::get().ShutdownRhinoPRT();
	}

	inline RHINOPRT_API void SetPackage(const wchar_t* rpk_path)
	{
		std::wstring str(rpk_path);
		RhinoPRT::get().SetRPKPath(str);
	}

	inline RHINOPRT_API bool AddMeshTest(ON_SimpleArray<const ON_Mesh*>* pMesh)
	{
		if (pMesh == nullptr) return false;

		std::vector<InitialShape> initShapes;
		for (int i = 0; i < pMesh->Count(); ++i) {
			initShapes.push_back(InitialShape(**pMesh->At(i))); // emplace_back
		}

		RhinoPRT::get().AddInitialShape(initShapes);
		return true;
	}

	inline RHINOPRT_API void ClearInitialShapes()
	{
		RhinoPRT::get().ClearInitialShapes();
	}

	inline RHINOPRT_API bool GenerateTest(ON_SimpleArray<ON_Mesh*>* pMeshArray)
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

	inline RHINOPRT_API int GetRuleAttributesCount()
	{
		return RhinoPRT::get().GetRuleAttributeCount();
	}

	inline RHINOPRT_API bool GetRuleAttribute(int attrIdx, wchar_t* rule, int rule_size, wchar_t* name, int name_size, wchar_t* nickname, int nickname_size, prt::AnnotationArgumentType* type)
	{
		RuleAttributes ruleAttributes = RhinoPRT::get().GetRuleAttributes();

		if (attrIdx >= ruleAttributes.size()) return false;

		wcscpy_s(rule, rule_size, ruleAttributes[attrIdx].mRuleFile.c_str());
		wcscpy_s(name, name_size, ruleAttributes[attrIdx].mFullName.c_str());
		wcscpy_s(nickname, nickname_size, ruleAttributes[attrIdx].mNickname.c_str());
		*type = ruleAttributes[attrIdx].mType;

		return true;
	}

	inline RHINOPRT_API void SetRuleAttributeDouble(const wchar_t* rule, const wchar_t* fullName, double value)
	{
		RhinoPRT::get().fillAttributeFromNode<double>(std::wstring(rule), std::wstring(fullName), value);
	}

	inline RHINOPRT_API void SetRuleAttributeBoolean(const wchar_t* rule, const wchar_t* fullName, bool value)
	{
		RhinoPRT::get().fillAttributeFromNode<bool>(std::wstring(rule), std::wstring(fullName), value);
	}

	inline RHINOPRT_API void SetRuleAttributeInteger(const wchar_t* rule, const wchar_t* fullName, int value)
	{
		RhinoPRT::get().fillAttributeFromNode<int>(std::wstring(rule), std::wstring(fullName), value);
	}

	inline RHINOPRT_API void SetRuleAttributeString(const wchar_t* rule, const wchar_t* fullName, const wchar_t* value)
	{
		RhinoPRT::get().fillAttributeFromNode<std::wstring>(std::wstring(rule), std::wstring(fullName), std::wstring(value));
	}

	inline RHINOPRT_API int GroupeReportsByKeys()
	{
		return RhinoPRT::get().groupReportsByKeys();
	}

	inline RHINOPRT_API bool GetReportKeys(ON_ClassArray<ON_wString>* pKeysArray, ON_SimpleArray<int>* pKeyTypeArray)
	{
		return RhinoPRT::get().getReports().getReportKeys(pKeysArray, pKeyTypeArray);
	}

	inline RHINOPRT_API void GetDoubleReports(const wchar_t* key, ON_SimpleArray<double>* pReportsArr)
	{
		auto reports = RhinoPRT::get().getReports().getDoubleReports(std::wstring(key));

		for (auto report : reports)
		{
			if (report.mType == prt::AttributeMap::PrimitiveType::PT_UNDEFINED)
				pReportsArr->Append(std::numeric_limits<double>::quiet_NaN());
			else
				pReportsArr->Append(report.mDoubleReport);
		}
	}

	inline RHINOPRT_API void GetStringReports(const wchar_t* key, ON_ClassArray<ON_wString>* pReportsArr)
	{
		auto reports = RhinoPRT::get().getReports().getStringReports(std::wstring(key));

		for (auto report : reports)
		{
			if (report.mType == prt::AttributeMap::PrimitiveType::PT_UNDEFINED)
				pReportsArr->Append(ON_wString(Reporting::EMPTY_REPORT_STRING.c_str()));
			else
				pReportsArr->Append(ON_wString(report.mStringReport.c_str()));
		}
	}

	inline RHINOPRT_API void GetBoolReports(const wchar_t* key, ON_SimpleArray<int>* pReportsArr)
	{
		auto reports = RhinoPRT::get().getReports().getBoolReports(std::wstring(key));

		for (auto report : reports)
		{
			if (report.mType == prt::AttributeMap::PrimitiveType::PT_UNDEFINED)
				pReportsArr->Append(-1); // This -1 is checked in the Grasshopper component and the corresponding output is set to null in case it is found.
			else
				pReportsArr->Append((int)report.mBoolReport);
		}
	}

	RHINOPRT_API void GetReports(int initialShapeId, ON_ClassArray<ON_wString>* pKeysArray, 
		ON_SimpleArray<double>* pDoubleReports, 
		ON_SimpleArray<bool>* pBoolReports,
		ON_ClassArray<ON_wString>* pStringReports)
	{
		const auto reports = RhinoPRT::myPRTAPI->getReportsOfModel(initialShapeId);

		//Sort the reports by Type.

	}

}