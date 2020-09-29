#include "RhinoPRT.h"

namespace {
	template <typename T>
	struct static_cast_func
	{
		template<typename T1>
		T operator()(const T1& x) const { return static_cast<T>(x); }

		template<>
		T operator()(const ON_wString& x) const { return static_cast<T>(x.Array()); }
	};
}

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

	RuleAttributes& RhinoPRTAPI::GetRuleAttributes() {
		return mRuleAttributes;
	}

	void RhinoPRTAPI::SetRPKPath(const std::wstring &rpk_path) {
		
		mPackagePath = rpk_path;

		// initialize the resolve map and rule infos here. Create the vector of rule attributes.
		if (!mModelGenerator)
			mModelGenerator = std::unique_ptr<ModelGenerator>(new ModelGenerator());

		// This also creates the resolve map
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
		mGeneratedModels.clear();
	}

	bool RhinoPRTAPI::GenerateGeometry() {
		mGeneratedModels.clear();
		mModelGenerator->generateModel(mShapes, mAttributes, ENCODER_ID_RHINO, options, mAttrBuilder, mGeneratedModels);
		return mGeneratedModels.size() > 0;
	}

	std::vector<GeneratedModel>& RhinoPRTAPI::getGenModels()
	{
		return mGeneratedModels;
	}

	template<typename T>
	void RhinoPRTAPI::fillAttributeFromNode(const std::wstring& ruleName, const std::wstring& attrFullName, T value, size_t count) {

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
		setRuleAttributeValue(rule, value, count);
	}

	template<typename T>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, T value, size_t /*count*/) {
		LOG_WRN << L"Ignored unsupported attribute '" << rule.mFullName << "' type: " << rule.mType << " ---- " << rule.mType << " of rule file: '" << rule.mRuleFile << "'" << std::endl;
	}

	// Specific instantiations of setRuleAttributeValue.

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, double value, size_t /*count*/) {
		if (rule.mType == prt::AAT_FLOAT) {
			mAttrBuilder->setFloat(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set a double value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, int value, size_t /*count*/) {
		if (rule.mType == prt::AAT_INT) {
			mAttrBuilder->setInt(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set an int value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, bool value, size_t /*count*/) {
		if (rule.mType == prt::AAT_BOOL) {
			mAttrBuilder->setBool(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set a boolean value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, std::wstring value, size_t /*count*/) {
		if (rule.mType == prt::AAT_STR) {
			mAttrBuilder->setString(rule.mFullName.c_str(), value.c_str());
		}
		else {
			LOG_ERR << L"Trying to set a wstring to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, const double* value, const size_t count)
	{
		if (rule.mType == prt::AAT_FLOAT_ARRAY)
		{
			mAttrBuilder->setFloatArray(rule.mFullName.c_str(), value, count);
		}
		else
		{
			LOG_ERR << L"Trying to set an array of double to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, bool* value, const size_t count)
	{
		if (rule.mType == prt::AAT_BOOL_ARRAY)
		{
			mAttrBuilder->setBoolArray(rule.mFullName.c_str(), value, count);
		}
		else
		{
			LOG_ERR << L"Trying to set an array of bool to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, const wchar_t**value, const size_t count)
	{
		if (rule.mType == prt::AAT_STR_ARRAY)
		{
			mAttrBuilder->setStringArray(rule.mFullName.c_str(), value, count);
		}
		else
		{
			LOG_ERR << L"Trying to set an array of wstring to an attribute of type " << rule.mType << std::endl;
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

	std::vector<int> RhinoPRTAPI::getModelIds()
	{
		std::vector<int> ids;

		std::for_each(mGeneratedModels.begin(), mGeneratedModels.end(), [&ids](const GeneratedModel& model) { ids.push_back(model.getInitialShapeIndex()); });

		return ids;
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

	inline RHINOPRT_API bool AddInitialMesh(ON_SimpleArray<const ON_Mesh*>* pMesh)
	{
		if (pMesh == nullptr) return false;

		std::vector<InitialShape> initShapes;
		initShapes.reserve(pMesh->Count());
		for (int i = 0; i < pMesh->Count(); ++i) {
			initShapes.emplace_back(**pMesh->At(i));
		}

		RhinoPRT::get().AddInitialShape(initShapes);
		return true;
	}

	RHINOPRT_API void ClearInitialShapes()
	{
		RhinoPRT::get().ClearInitialShapes();
	}

	inline RHINOPRT_API bool Generate()
	{
		return RhinoPRT::get().GenerateGeometry();
	}

	RHINOPRT_API void GetAllMeshIDs(ON_SimpleArray<int>* pMeshIDs)
	{
		auto ids = RhinoPRT::get().getModelIds();

		for (int id : ids) pMeshIDs->Append(id);
	}

	RHINOPRT_API int GetMeshPartCount(int initShapeId)
	{
		const auto& models = RhinoPRT::get().getGenModels();

		const auto& modelIt = std::find_if(models.begin(), models.end(), [&initShapeId](GeneratedModel m) { return m.getInitialShapeIndex() == initShapeId; });
		if (modelIt == models.end())
		{
			LOG_ERR << L"No generated model with initial shape ID " << initShapeId << " was found. The generation of this model has probably failed.";
			return 0;
		}

		return modelIt->getMeshPartCount();
	}

	RHINOPRT_API bool GetMeshBundle(int initShapeID, ON_SimpleArray<ON_Mesh*>* pMeshArray)
	{
		auto models = RhinoPRT::get().getGenModels();
		
		const auto& modelIt = std::find_if(models.begin(), models.end(), [&initShapeID](GeneratedModel m) { return m.getInitialShapeIndex() == initShapeID; });

		if (modelIt == models.end()) 
		{
			LOG_ERR << L"No generated model with the given initial shape ID was found. The generation of this model has probably failed.";
			return false;
		}

		const auto meshBundle = (*modelIt).getMeshesFromGenModel();

		for (const auto& meshPart : meshBundle) {
			
			pMeshArray->Append(new ON_Mesh(meshPart));
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

	RHINOPRT_API void SetRuleAttributeDoubleArray(const wchar_t* rule, const wchar_t* fullName, ON_SimpleArray<double>* pValueArray)
	{
		const double* valueArray = pValueArray->Array();
		const size_t size = pValueArray->Count();
		
		RhinoPRT::get().fillAttributeFromNode(std::wstring(rule), std::wstring(fullName), valueArray, size);
	}

	RHINOPRT_API void SetRuleAttributeBoolArray(const wchar_t* rule, const wchar_t* fullName, ON_SimpleArray<int>* pValueArray)
	{
		const int* valueArray = pValueArray->Array();
		const size_t size = pValueArray->Count();

		// convert int array to boolean array
		bool* boolArray = new bool[size];
		std::transform(valueArray, valueArray + size, boolArray, static_cast_func<bool>());

		RhinoPRT::get().fillAttributeFromNode(std::wstring(rule), std::wstring(fullName), boolArray, size);

		delete[] boolArray;
	}

	RHINOPRT_API void SetRuleAttributeStringArray(const wchar_t* rule, const wchar_t* fullName, ON_ClassArray<ON_wString>* pValueArray)
	{
		const ON_wString* valueArray = pValueArray->Array();
		const size_t size = pValueArray->Count();

		//convert the array of ON_wString to const wchar_t* const *value
		const wchar_t** strArray = new const wchar_t*[size];
		std::transform(valueArray, valueArray + size, strArray, static_cast_func<const wchar_t*>());

		RhinoPRT::get().fillAttributeFromNode(std::wstring(rule), std::wstring(fullName), strArray, size);

		delete[] strArray;
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

	RHINOPRT_API bool GetMaterial(int initialShapeId, int meshID, int* uvSet,
		ON_ClassArray<ON_wString>* pTexKeys,
		ON_ClassArray<ON_wString>* pTexPaths,
		ON_SimpleArray<int>* pDiffuseColor,
		ON_SimpleArray<int>* pAmbientColor,
		ON_SimpleArray<int>* pSpecularColor,
		double* opacity,
		double* shininess)
	{
		auto& genModels = RhinoPRT::get().getGenModels();

		if (initialShapeId >= genModels.size()) {
			LOG_ERR << L"Initial shape ID out of range";
			return false;
		}

		auto& currModel = genModels[initialShapeId];
		auto& material = currModel.getMaterials();

		if (meshID >= material.size()) 
		{
			LOG_ERR << L"Mesh ID is out of range";
			return false;
		}
		auto& mat = material.at(meshID);

		for (auto& texture: mat.mTexturePaths) {

#ifdef DEBUG
			LOG_DBG << L"texture: [ " << texture.first << " : " << texture.second << "]";
#endif // DEBUG

			const wchar_t* fullTexPath = texture.second.c_str();
			
			pTexKeys->Append(ON_wString(texture.first.c_str()));
			pTexPaths->Append(ON_wString(fullTexPath));
		}

		auto diffuse = mat.mDiffuseCol;
		pDiffuseColor->Append(diffuse.Red());
		pDiffuseColor->Append(diffuse.Green());
		pDiffuseColor->Append(diffuse.Blue());

		auto ambient = mat.mAmbientCol;
		pAmbientColor->Append(ambient.Red());
		pAmbientColor->Append(ambient.Green());
		pAmbientColor->Append(ambient.Blue());

		auto specular = mat.mSpecularCol;
		pSpecularColor->Append(specular.Red());
		pSpecularColor->Append(specular.Green());
		pSpecularColor->Append(specular.Blue());

		*opacity = mat.mOpacity;
		*shininess = mat.mShininess;

		return true;
	}
}