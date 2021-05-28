#include "RhinoPRT.h"

namespace {
	template<typename T, typename T1>
	T static_cast_fct(const T1& x) { return static_cast<T>(x); }

	const wchar_t* to_wchar_array(const ON_wString& x) { return x.Array(); }
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
		return static_cast<int>(mModelGenerator->getRuleAttributes().size());
	}

	RuleAttributes& RhinoPRTAPI::GetRuleAttributes() {
		return mModelGenerator->getRuleAttributes();
	}

	void RhinoPRTAPI::SetRPKPath(const std::wstring &rpk_path) {
		
		mPackagePath = rpk_path;

		// initialize the resolve map and rule infos here. Create the vector of rule attributes.
		if (!mModelGenerator)
			mModelGenerator = std::unique_ptr<ModelGenerator>(new ModelGenerator());

		// This also creates the resolve map
		mModelGenerator->updateRuleFiles(mPackagePath);

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

		// compute the default values of rule attributes for each initial shape
		mModelGenerator->evalDefaultAttributes(mShapes, mAttributes);

		// Initialise the attribute map builders for each initial shape.
		mAttrBuilders.resize(shapes.size());
		for (auto& it : mAttrBuilders) {
			it.reset(prt::AttributeMapBuilder::create());
		}
	}

	void RhinoPRTAPI::ClearInitialShapes() {
		mShapes.clear();
		mGeneratedModels.clear();
		mAttributes.clear();
		mGroupedReports.clear();
		mGeneratedModels.clear();
		mAttrBuilders.clear();
	}

	bool RhinoPRTAPI::GenerateGeometry() {
		mGeneratedModels.clear();
		mModelGenerator->generateModel(mShapes, mAttributes, ENCODER_ID_RHINO, options, mAttrBuilders, mGeneratedModels);
		return mGeneratedModels.size() > 0;
	}

	std::vector<GeneratedModel>& RhinoPRTAPI::getGenModels()
	{
		return mGeneratedModels;
	}

	template<typename T>
	void RhinoPRTAPI::fillAttributeFromNode(const int initialShapeIndex, const std::wstring& /*ruleName*/, const std::wstring& attrFullName, T value, size_t count) {

		auto& ruleAttributes = mModelGenerator->getRuleAttributes();

		const RuleAttributes::iterator it = std::find_if(ruleAttributes.begin(), ruleAttributes.end(), [&attrFullName](const auto& ra)
															{return ra->mFullName == attrFullName; });

		if (it != ruleAttributes.end())
		{
			const RuleAttributeUPtr& rule = *it;
			assert(!rule->mFullName.empty()); // Check if the rule was found

			// If the attribute is found, register the value in the attribute map builder
			//TODO: check for difference with default value, only add the attribute if it is the case.
			setRuleAttributeValue(initialShapeIndex, rule, value, count);
		}
	}

	void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, double value, size_t /*count*/) {
		if (rule->mType == prt::AAT_FLOAT) {
			mAttrBuilders[initialShapeIndex]->setFloat(rule->mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set a double value to an attribute of type " << rule->mType << std::endl;
		}
	}

	void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, int value, size_t /*count*/) {
		if (rule->mType == prt::AAT_INT) {
			mAttrBuilders[initialShapeIndex]->setInt(rule->mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set an int value to an attribute of type " << rule->mType << std::endl;
		}
	}

	void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, bool value, size_t /*count*/) {
		if (rule->mType == prt::AAT_BOOL) {
			mAttrBuilders[initialShapeIndex]->setBool(rule->mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set a boolean value to an attribute of type " << rule->mType << std::endl;
		}
	}

	void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, std::wstring& value, size_t /*count*/) {
		if (rule->mType == prt::AAT_STR) {
			mAttrBuilders[initialShapeIndex]->setString(rule->mFullName.c_str(), value.c_str());
		}
		else {
			LOG_ERR << L"Trying to set a wstring to an attribute of type " << rule->mType << std::endl;
		}
	}

	void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, const double* value, const size_t count)
	{
		if (rule->mType == prt::AAT_FLOAT_ARRAY)
		{
			mAttrBuilders[initialShapeIndex]->setFloatArray(rule->mFullName.c_str(), value, count);
		}
		else
		{
			LOG_ERR << L"Trying to set an array of double to an attribute of type " << rule->mType << std::endl;
		}
	}

	void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, bool* value, const size_t count)
	{
		if (rule->mType == prt::AAT_BOOL_ARRAY)
		{
			mAttrBuilders[initialShapeIndex]->setBoolArray(rule->mFullName.c_str(), value, count);
		}
		else
		{
			LOG_ERR << L"Trying to set an array of bool to an attribute of type " << rule->mType << std::endl;
		}
	}

	void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const RuleAttributeUPtr& rule, std::vector<const wchar_t *> value, const size_t /*count*/)
	{
		if (rule->mType == prt::AAT_STR_ARRAY)
		{
			mAttrBuilders[initialShapeIndex]->setStringArray(rule->mFullName.c_str(), value.data(), value.size());
		}
		else
		{
			LOG_ERR << L"Trying to set an array of wstring to an attribute of type " << rule->mType << std::endl;
		}
	}

	Reporting::ReportsVector RhinoPRTAPI::getReportsOfModel(int initialShapeIndex)
	{
		// find the report with given shape id.
		const auto found_reports = std::find_if(mGeneratedModels.begin(), mGeneratedModels.end(), 
			[&initialShapeIndex](const GeneratedModel& model) { return model.getInitialShapeIndex() == initialShapeIndex; });

		if (found_reports != mGeneratedModels.end()) {
			const auto& reports = found_reports->getReport();
			return Reporting::ToReportsVector(reports);
		}

		return Reporting::EMPTY_REPORTS;
	}

	std::vector<int> RhinoPRTAPI::getModelIds()
	{
		std::vector<int> ids;

		std::for_each(mGeneratedModels.begin(), mGeneratedModels.end(), [&ids](const GeneratedModel& model) { ids.push_back(static_cast<int>(model.getInitialShapeIndex())); });

		return ids;
	}

	void RhinoPRTAPI::setMaterialGeneration(bool emitMaterial)
	{
		options.emitMaterial = emitMaterial;
	}

	bool RhinoPRTAPI::getDefaultValueBoolean(const std::wstring key, bool* value)
	{
		return mModelGenerator->getDefaultValueBoolean(key, value);
	}

	bool RhinoPRTAPI::getDefaultValueNumber(const std::wstring key, double* value)
	{
		return mModelGenerator->getDefaultValueNumber(key, value);
	}

	bool RhinoPRTAPI::getDefaultValueText(const std::wstring key, ON_wString* pText)
	{
		return mModelGenerator->getDefaultValueText(key, pText);
	}
}

extern "C" {

	RHINOPRT_API void GetProductVersion(ON_wString* version_str)
	{
		std::string str(VER_FILE_VERSION_STR);
		pcu::appendToRhinoString(*version_str, str);
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
		if (!rpk_path) return;
		RhinoPRT::get().SetRPKPath(rpk_path);
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

	RHINOPRT_API int GetMeshPartCount(int initialShapeIndex)
	{
		const auto& models = RhinoPRT::get().getGenModels();

		const auto& modelIt = std::find_if(models.begin(), models.end(), [&initialShapeIndex](GeneratedModel m) { return m.getInitialShapeIndex() == initialShapeIndex; });
		if (modelIt == models.end())
		{
			LOG_ERR << L"No generated model with initial shape ID " << initialShapeIndex << " was found. The generation of this model has probably failed.";
			return 0;
		}

		return modelIt->getMeshPartCount();
	}

	RHINOPRT_API bool GetMeshBundle(int initialShapeIndex, ON_SimpleArray<ON_Mesh*>* pMeshArray)
	{
		const auto& models = RhinoPRT::get().getGenModels();
		
		const auto& modelIt = std::find_if(models.begin(), models.end(), [&initialShapeIndex](GeneratedModel m) { return m.getInitialShapeIndex() == initialShapeIndex; });

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

	RHINOPRT_API bool GetRuleAttribute(int attrIdx, ON_wString* pRule, ON_wString* pName, ON_wString* pNickname, prt::AnnotationArgumentType* type, ON_wString* pGroup)
	{
		RuleAttributes& ruleAttributes = RhinoPRT::get().GetRuleAttributes();

		if (attrIdx >= ruleAttributes.size()) return false;

		const RuleAttributeUPtr& ruleAttr = ruleAttributes[attrIdx];
		pcu::appendToRhinoString(*pRule, ruleAttr->mRuleFile);
		pcu::appendToRhinoString(*pName, ruleAttr->mFullName);
		pcu::appendToRhinoString(*pNickname, ruleAttr->mNickname);
		*type = ruleAttr->mType;

		if(ruleAttr->groups.size() > 0)
			*pGroup += ON_wString(ruleAttr->groups.front().c_str());

		return true;
	}

	RHINOPRT_API void SetRuleAttributeDouble(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName, double value)
	{
		if (!rule || !fullName) return;
		RhinoPRT::get().fillAttributeFromNode<double>(initialShapeIndex, rule, fullName, value);
	}

	RHINOPRT_API void SetRuleAttributeBoolean(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName, bool value)
	{
		if (!rule || !fullName) return;
		RhinoPRT::get().fillAttributeFromNode<bool>(initialShapeIndex, rule, fullName, value);
	}

	RHINOPRT_API void SetRuleAttributeInteger(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName, int value)
	{
		if (!rule || !fullName) return;
		RhinoPRT::get().fillAttributeFromNode<int>(initialShapeIndex, rule, fullName, value);
	}

	RHINOPRT_API void SetRuleAttributeString(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName, const wchar_t* value)
	{
		if (!rule || !fullName || !value) return;
		RhinoPRT::get().fillAttributeFromNode<std::wstring>(initialShapeIndex, rule, fullName, value);
	}

	RHINOPRT_API void SetRuleAttributeDoubleArray(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName, ON_SimpleArray<double>* pValueArray)
	{
		if (!rule || !fullName || !pValueArray) return;

		const double* valueArray = pValueArray->Array();
		const size_t size = pValueArray->Count();
		
		RhinoPRT::get().fillAttributeFromNode(initialShapeIndex, rule, fullName, valueArray, size);
	}

	RHINOPRT_API void SetRuleAttributeBoolArray(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName, ON_SimpleArray<int>* pValueArray)
	{
		if (!rule || !fullName || !pValueArray) return;

		const int* valueArray = pValueArray->Array();
		const size_t size = pValueArray->Count();

		// convert int array to boolean array
		std::unique_ptr<bool[]> boolArray(new bool[size]);
		std::transform(valueArray, valueArray + size, boolArray.get(), static_cast_fct<bool, int>);

		RhinoPRT::get().fillAttributeFromNode(initialShapeIndex, rule, fullName, boolArray.get(), size);
	}

	RHINOPRT_API void SetRuleAttributeStringArray(const int initialShapeIndex, const wchar_t* rule, const wchar_t* fullName, ON_ClassArray<ON_wString>* pValueArray)
	{
		if (!rule || !fullName || !pValueArray) return;

		const ON_wString* valueArray = pValueArray->Array();
		const size_t size = pValueArray->Count();

		//convert the array of ON_wString to a std::vector of wstring.
		std::vector<const wchar_t*> strVector(size);
		std::transform(valueArray, valueArray + size, strVector.begin(), to_wchar_array);

		RhinoPRT::get().fillAttributeFromNode(initialShapeIndex, rule, fullName, strVector, size);
	}

	RHINOPRT_API void GetReports(int initialShapeIndex, ON_ClassArray<ON_wString>* pKeysArray,
		ON_SimpleArray<double>* pDoubleReports, 
		ON_SimpleArray<bool>* pBoolReports,
		ON_ClassArray<ON_wString>* pStringReports)
	{
		auto reports = RhinoPRT::get().getReportsOfModel(initialShapeIndex);

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

	RHINOPRT_API void GetAnnotationTypes(int ruleIdx, ON_SimpleArray<AttributeAnnotation>* pAnnotTypeArray)
	{
		auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
		if (ruleIdx < ruleAttributes.size())
		{
			RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
			std::for_each(attrib->mAnnotations.begin(), attrib->mAnnotations.end(), [pAnnotTypeArray](const AnnotationUPtr& p) 
				{ pAnnotTypeArray->Append(p->getType()); });
		}
	}

	RHINOPRT_API bool GetEnumType(int ruleIdx, int enumIdx, EnumAnnotationType* type)
	{
		auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
		if (ruleIdx < ruleAttributes.size())
		{
			RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
			if (enumIdx < attrib->mAnnotations.size())
			{
				const AnnotationUPtr& annot = attrib->mAnnotations[enumIdx];
				if (annot->getType() == AttributeAnnotation::ENUM)
				{
					*type = annot->getEnumType();
					return true;
				}
			}
		}

		return false;
	}

	RHINOPRT_API bool GetAnnotationEnumDouble(int ruleIdx, int enumIdx, ON_SimpleArray<double>* pArray, bool* restricted)
	{
		auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
		if (ruleIdx < ruleAttributes.size())
		{
			RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
			if (enumIdx < attrib->mAnnotations.size())
			{
				AnnotationUPtr& annot = attrib->mAnnotations[enumIdx];
				if (annot->getType() == AttributeAnnotation::ENUM && annot->getEnumType() == EnumAnnotationType::DOUBLE)
				{
					*restricted = dynamic_cast<AnnotationEnum<double>*>(annot.get())->isRestricted();
					auto& enumList = dynamic_cast<AnnotationEnum<double>*>(annot.get())->getAnnotArguments();

					std::for_each(enumList.begin(), enumList.end(), [&pArray](double& v) {pArray->Append(v); });
					return true;
				}
			}
		}

		return false;
	}

	RHINOPRT_API bool GetAnnotationEnumString(int ruleIdx, int enumIdx, ON_ClassArray<ON_wString>* pArray, bool* restricted)
	{
		auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
		if (ruleIdx < ruleAttributes.size())
		{
			RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
			if (enumIdx < attrib->mAnnotations.size())
			{
				AnnotationUPtr& annot = attrib->mAnnotations[enumIdx];
				if (annot->getType() == AttributeAnnotation::ENUM && annot->getEnumType() == EnumAnnotationType::STRING)
				{
					*restricted = dynamic_cast<AnnotationEnum<std::wstring>*>(annot.get())->isRestricted();
					std::vector<std::wstring> enumList = dynamic_cast<AnnotationEnum<std::wstring>*>(annot.get())->getAnnotArguments();

					std::for_each(enumList.begin(), enumList.end(), [&pArray](std::wstring& v) {pArray->Append(ON_wString(v.c_str())); });
					return true;
				}
			}
		}

		return false;
	}

	RHINOPRT_API bool GetAnnotationRange(int ruleIdx, int enumIdx, double* min, double* max, double* stepsize, bool* restricted)
	{
		auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
		if (ruleIdx < ruleAttributes.size())
		{
			RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
			if (enumIdx < attrib->mAnnotations.size())
			{
				AnnotationUPtr& annot = attrib->mAnnotations[enumIdx];
				if (annot->getType() == AttributeAnnotation::RANGE)
				{
					RangeAttributes range = dynamic_cast<AnnotationRange*>(annot.get())->getAnnotArguments();
					*min = range.mMin;
					*max = range.mMax;
					*stepsize = range.mStepSize;
					*restricted = range.mRestricted;
					return true;
				}
			}
		}
		return false;
	}


	RHINOPRT_API bool GetMaterial(int initialShapeIndex, int meshID, int* /*uvSet*/,
		ON_ClassArray<ON_wString>* pTexKeys,
		ON_ClassArray<ON_wString>* pTexPaths,
		ON_SimpleArray<int>* pDiffuseColor,
		ON_SimpleArray<int>* pAmbientColor,
		ON_SimpleArray<int>* pSpecularColor,
		double* opacity,
		double* shininess)
	{
		auto& genModels = RhinoPRT::get().getGenModels();

		if (initialShapeIndex >= genModels.size()) {
			LOG_ERR << L"Initial shape ID out of range";
			return false;
		}

		auto& currModel = genModels[initialShapeIndex];
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

	RHINOPRT_API void SetMaterialGenerationOption(bool doGenerate)
	{
		RhinoPRT::get().setMaterialGeneration(doGenerate);
	}

	RHINOPRT_API bool GetDefaultValueBoolean(const wchar_t* key, bool* value)
	{
		return RhinoPRT::get().getDefaultValueBoolean(key, value);
	}

	RHINOPRT_API bool GetDefaultValueNumber(const wchar_t* key, double* value)
	{
		return RhinoPRT::get().getDefaultValueNumber(key, value);
	}

	RHINOPRT_API bool GetDefaultValueText(const wchar_t* key, ON_wString* pText)
	{
		return RhinoPRT::get().getDefaultValueText(key, pText);
	}
}