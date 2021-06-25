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

#include "RhinoPRT.h"

namespace {

template <typename T, typename T1>
T static_cast_fct(const T1& x) {
	return static_cast<T>(x);
}

const wchar_t* to_wchar_array(const ON_wString& x) {
	return x.Array();
}

template <typename T>
void fillAttributeFromNode(RhinoPRT::RhinoPRTAPI& prtApi, const int initialShapeIndex, const std::wstring& attrFullName,
                           T value, size_t count = 1) {
	prtApi.setRuleAttributeValue(initialShapeIndex, attrFullName, value, count);
}

} // namespace

namespace RhinoPRT {

// Global PRT handle
RhinoPRTAPI& get() {
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

const RuleAttributes& RhinoPRTAPI::GetRuleAttributes() const {
	return mModelGenerator->getRuleAttributes();
}

void RhinoPRTAPI::SetRPKPath(const std::wstring& rpk_path) {

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

	mShapes.reserve(shapes.size());
	mAttributes.reserve(shapes.size());

	mShapes.insert(mShapes.end(), shapes.begin(), shapes.end());
	mAttributes.resize(mShapes.size(), pcu::ShapeAttributes(rulef, ruleN, shapeN));

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

std::vector<GeneratedModel>& RhinoPRTAPI::getGenModels() {
	return mGeneratedModels;
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, double value,
                                        size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setFloat(name.c_str(), value);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, int value,
                                        size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setInt(name.c_str(), value);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, bool value,
                                        size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setBool(name.c_str(), value);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, std::wstring& value,
                                        size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setString(name.c_str(), value.c_str());
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, const double* value,
                                        const size_t count) {
	mAttrBuilders[initialShapeIndex]->setFloatArray(name.c_str(), value, count);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name, bool* value,
                                        const size_t count) {
	mAttrBuilders[initialShapeIndex]->setBoolArray(name.c_str(), value, count);
}

void RhinoPRTAPI::setRuleAttributeValue(const int initialShapeIndex, const std::wstring& name,
                                        std::vector<const wchar_t*> value, const size_t /*count*/) {
	mAttrBuilders[initialShapeIndex]->setStringArray(name.c_str(), value.data(), value.size());
}

Reporting::ReportsVector RhinoPRTAPI::getReportsOfModel(int initialShapeIndex) {
	// find the report with given shape id.
	const auto found_reports = std::find_if(mGeneratedModels.begin(), mGeneratedModels.end(),
	                                        [&initialShapeIndex](const GeneratedModel& model) {
		                                        return model.getInitialShapeIndex() == initialShapeIndex;
	                                        });

	if (found_reports != mGeneratedModels.end()) {
		const auto& reports = found_reports->getReport();
		return Reporting::ToReportsVector(reports);
	}

	return Reporting::EMPTY_REPORTS;
}

std::vector<int> RhinoPRTAPI::getModelIds() {
	std::vector<int> ids;

	std::for_each(mGeneratedModels.begin(), mGeneratedModels.end(), [&ids](const GeneratedModel& model) {
		ids.push_back(static_cast<int>(model.getInitialShapeIndex()));
	});

	return ids;
}

void RhinoPRTAPI::setMaterialGeneration(bool emitMaterial) {
	options.emitMaterial = emitMaterial;
}

bool RhinoPRTAPI::getDefaultValueBoolean(const std::wstring key, bool* value) {
	return mModelGenerator->getDefaultValueBoolean(key, value);
}

bool RhinoPRTAPI::getDefaultValueNumber(const std::wstring key, double* value) {
	return mModelGenerator->getDefaultValueNumber(key, value);
}

bool RhinoPRTAPI::getDefaultValueText(const std::wstring key, ON_wString* pText) {
	return mModelGenerator->getDefaultValueText(key, pText);
}
} // namespace RhinoPRT

extern "C" {

RHINOPRT_API void GetProductVersion(ON_wString* version_str) {
	*version_str = VER_FILE_VERSION_STR;
}

RHINOPRT_API bool InitializeRhinoPRT() {
	return RhinoPRT::get().InitializeRhinoPRT();
}

RHINOPRT_API void ShutdownRhinoPRT() {
	RhinoPRT::get().ShutdownRhinoPRT();
}

RHINOPRT_API void SetPackage(const wchar_t* rpk_path) {
	if (!rpk_path)
		return;
	RhinoPRT::get().SetRPKPath(rpk_path);
}

inline RHINOPRT_API bool AddInitialMesh(ON_SimpleArray<const ON_Mesh*>* pMesh) {
	if (pMesh == nullptr)
		return false;

	std::vector<InitialShape> initShapes;
	initShapes.reserve(pMesh->Count());
	for (int i = 0; i < pMesh->Count(); ++i) {
		initShapes.emplace_back(**pMesh->At(i));
	}

	RhinoPRT::get().AddInitialShape(initShapes);
	return true;
}

RHINOPRT_API void ClearInitialShapes() {
	RhinoPRT::get().ClearInitialShapes();
}

inline RHINOPRT_API bool Generate() {
	return RhinoPRT::get().GenerateGeometry();
}

RHINOPRT_API void GetAllMeshIDs(ON_SimpleArray<int>* pMeshIDs) {
	auto ids = RhinoPRT::get().getModelIds();
	for (int id : ids)
		pMeshIDs->Append(id);
}

RHINOPRT_API int GetMeshPartCount(int initialShapeIndex) {
	const auto& models = RhinoPRT::get().getGenModels();

	const auto& modelIt = std::find_if(models.begin(), models.end(), [&initialShapeIndex](GeneratedModel m) {
		return m.getInitialShapeIndex() == initialShapeIndex;
	});
	if (modelIt == models.end()) {
		LOG_ERR << L"No generated model with initial shape ID " << initialShapeIndex
		        << " was found. The generation of this model has probably failed.";
		return 0;
	}

	return modelIt->getMeshPartCount();
}

RHINOPRT_API bool GetMeshBundle(int initialShapeIndex, ON_SimpleArray<ON_Mesh*>* pMeshArray) {
	const auto& models = RhinoPRT::get().getGenModels();

	const auto& modelIt = std::find_if(models.begin(), models.end(), [&initialShapeIndex](GeneratedModel m) {
		return m.getInitialShapeIndex() == initialShapeIndex;
	});

	if (modelIt == models.end()) {
		LOG_ERR << L"No generated model with the given initial shape ID was found. The generation of this model has "
		           L"probably failed.";
		return false;
	}

	const auto meshBundle = (*modelIt).getMeshesFromGenModel();

	for (const auto& meshPart : meshBundle) {

		pMeshArray->Append(new ON_Mesh(meshPart));
	}

	return true;
}

RHINOPRT_API int GetRuleAttributesCount() {
	return RhinoPRT::get().GetRuleAttributeCount();
}

RHINOPRT_API bool GetRuleAttribute(int attrIdx, ON_wString* pRule, ON_wString* pName, ON_wString* pNickname,
                                   prt::AnnotationArgumentType* type, ON_wString* pGroup) {
	const RuleAttributes& ruleAttributes = RhinoPRT::get().GetRuleAttributes();

	if (attrIdx >= ruleAttributes.size())
		return false;

	const RuleAttributeUPtr& ruleAttr = ruleAttributes[attrIdx];
	pcu::appendToRhinoString(*pRule, ruleAttr->mRuleFile);
	pcu::appendToRhinoString(*pName, ruleAttr->mFullName);
	pcu::appendToRhinoString(*pNickname, ruleAttr->mNickname);
	*type = ruleAttr->mType;

	if (ruleAttr->groups.size() > 0)
		*pGroup += ON_wString(ruleAttr->groups.front().c_str());

	return true;
}

RHINOPRT_API void SetRuleAttributeDouble(const int initialShapeIndex, const wchar_t* fullName, double value) {
	if (!fullName)
		return;
	fillAttributeFromNode<double>(RhinoPRT::get(), initialShapeIndex, fullName, value);
}

RHINOPRT_API void SetRuleAttributeBoolean(const int initialShapeIndex, const wchar_t* fullName, bool value) {
	if (!fullName)
		return;
	fillAttributeFromNode<bool>(RhinoPRT::get(), initialShapeIndex, fullName, value);
}

RHINOPRT_API void SetRuleAttributeInteger(const int initialShapeIndex, const wchar_t* fullName, int value) {
	if (!fullName)
		return;
	fillAttributeFromNode<int>(RhinoPRT::get(), initialShapeIndex, fullName, value);
}

RHINOPRT_API void SetRuleAttributeString(const int initialShapeIndex, const wchar_t* fullName, const wchar_t* value) {
	if (!fullName || !value)
		return;
	fillAttributeFromNode<std::wstring>(RhinoPRT::get(), initialShapeIndex, fullName, value);
}

RHINOPRT_API void SetRuleAttributeDoubleArray(const int initialShapeIndex, const wchar_t* fullName,
                                              ON_SimpleArray<double>* pValueArray) {
	if (!fullName || !pValueArray)
		return;

	const double* valueArray = pValueArray->Array();
	const size_t size = pValueArray->Count();

	fillAttributeFromNode(RhinoPRT::get(), initialShapeIndex, fullName, valueArray, size);
}

RHINOPRT_API void SetRuleAttributeBoolArray(const int initialShapeIndex, const wchar_t* fullName,
                                            ON_SimpleArray<int>* pValueArray) {
	if (!fullName || !pValueArray)
		return;

	const int* valueArray = pValueArray->Array();
	const size_t size = pValueArray->Count();

	// convert int array to boolean array
	std::unique_ptr<bool[]> boolArray(new bool[size]);
	std::transform(valueArray, valueArray + size, boolArray.get(), static_cast_fct<bool, int>);

	fillAttributeFromNode(RhinoPRT::get(), initialShapeIndex, fullName, boolArray.get(), size);
}

RHINOPRT_API void SetRuleAttributeStringArray(const int initialShapeIndex, const wchar_t* fullName,
                                              ON_ClassArray<ON_wString>* pValueArray) {
	if (!fullName || !pValueArray)
		return;

	const ON_wString* valueArray = pValueArray->Array();
	const size_t size = pValueArray->Count();

	// convert the array of ON_wString to a std::vector of wstring.
	std::vector<const wchar_t*> strVector(size);
	std::transform(valueArray, valueArray + size, strVector.begin(), to_wchar_array);

	fillAttributeFromNode(RhinoPRT::get(), initialShapeIndex, fullName, strVector, size);
}

RHINOPRT_API void GetReports(int initialShapeIndex, ON_ClassArray<ON_wString>* pKeysArray,
                             ON_SimpleArray<double>* pDoubleReports, ON_SimpleArray<bool>* pBoolReports,
                             ON_ClassArray<ON_wString>* pStringReports) {
	auto reports = RhinoPRT::get().getReportsOfModel(initialShapeIndex);

	/*
	left.float	-> right.all OK
	left.bool	-> right.float OK
	            -> right.bool OK
	            -> right.string OK
	left.string -> right.all OK
	*/
	// Sort the reports by Type. The order is Double -> Bool -> String
	std::sort(reports.begin(), reports.end(),
	          [](Reporting::ReportAttribute& left, Reporting::ReportAttribute& right) -> bool {
		          if (left.mType == right.mType)
			          return left.mReportName.compare(right.mReportName) <
			                 0; // assuming case sensitivity. assuming two reports can't have the same name.
		          if (left.mType == prt::AttributeMap::PrimitiveType::PT_FLOAT)
			          return true;
		          if (right.mType == prt::AttributeMap::PrimitiveType::PT_FLOAT)
			          return false;
		          if (left.mType == prt::AttributeMap::PrimitiveType::PT_STRING)
			          return false;
		          if (left.mType == prt::AttributeMap::PrimitiveType::PT_BOOL &&
		              right.mType == prt::AttributeMap::PrimitiveType::PT_STRING)
			          return true;
		          return false;
	          });

	for (const auto& report : reports) {
		pKeysArray->Append(ON_wString(report.mReportName.c_str()));

		switch (report.mType) {
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
				// REMOVE LAST KEY
				pKeysArray->Remove(pKeysArray->Count() - 1);
		}
	}
}

RHINOPRT_API void GetAnnotationTypes(int ruleIdx, ON_SimpleArray<AttributeAnnotation>* pAnnotTypeArray) {
	auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
	if (ruleIdx < ruleAttributes.size()) {
		const RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
		std::for_each(attrib->mAnnotations.begin(), attrib->mAnnotations.end(),
		              [pAnnotTypeArray](const AnnotationUPtr& p) { pAnnotTypeArray->Append(p->getType()); });
	}
}

RHINOPRT_API bool GetEnumType(int ruleIdx, int enumIdx, EnumAnnotationType* type) {
	auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
	if (ruleIdx < ruleAttributes.size()) {
		const RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
		if (enumIdx < attrib->mAnnotations.size()) {
			const AnnotationUPtr& annot = attrib->mAnnotations[enumIdx];
			if (annot->getType() == AttributeAnnotation::ENUM) {
				*type = annot->getEnumType();
				return true;
			}
		}
	}

	return false;
}

RHINOPRT_API bool GetAnnotationEnumDouble(int ruleIdx, int enumIdx, ON_SimpleArray<double>* pArray, bool* restricted) {
	const auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
	if (ruleIdx < ruleAttributes.size()) {
		const RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
		if (enumIdx < attrib->mAnnotations.size()) {
			const AnnotationBase& annot = *attrib->mAnnotations[enumIdx];
			if (annot.getType() == AttributeAnnotation::ENUM && annot.getEnumType() == EnumAnnotationType::DOUBLE) {
				const AnnotationEnum<double>& annotEnum = static_cast<const AnnotationEnum<double>&>(annot);
				for (const double& v : annotEnum.getAnnotArguments())
					pArray->Append(v);
				*restricted = annotEnum.isRestricted();

				return true;
			}
		}
	}

	return false;
}

RHINOPRT_API bool GetAnnotationEnumString(int ruleIdx, int enumIdx, ON_ClassArray<ON_wString>* pArray,
                                          bool* restricted) {
	auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
	if (ruleIdx < ruleAttributes.size()) {
		const RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
		if (enumIdx < attrib->mAnnotations.size()) {
			AnnotationUPtr& annot = attrib->mAnnotations[enumIdx];
			if (annot->getType() == AttributeAnnotation::ENUM && annot->getEnumType() == EnumAnnotationType::STRING) {
				*restricted = dynamic_cast<AnnotationEnum<std::wstring>*>(annot.get())->isRestricted();
				std::vector<std::wstring> enumList =
				        dynamic_cast<AnnotationEnum<std::wstring>*>(annot.get())->getAnnotArguments();

				std::for_each(enumList.begin(), enumList.end(),
				              [&pArray](std::wstring& v) { pArray->Append(ON_wString(v.c_str())); });
				return true;
			}
		}
	}

	return false;
}

RHINOPRT_API bool GetAnnotationRange(int ruleIdx, int enumIdx, double* min, double* max, double* stepsize,
                                     bool* restricted) {
	auto& ruleAttributes = RhinoPRT::get().GetRuleAttributes();
	if (ruleIdx < ruleAttributes.size()) {
		const RuleAttributeUPtr& attrib = ruleAttributes[ruleIdx];
		if (enumIdx < attrib->mAnnotations.size()) {
			AnnotationUPtr& annot = attrib->mAnnotations[enumIdx];
			if (annot->getType() == AttributeAnnotation::RANGE) {
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

RHINOPRT_API bool GetMaterial(int initialShapeIndex, int meshID, int* /*uvSet*/, ON_ClassArray<ON_wString>* pTexKeys,
                              ON_ClassArray<ON_wString>* pTexPaths, ON_SimpleArray<int>* pDiffuseColor,
                              ON_SimpleArray<int>* pAmbientColor, ON_SimpleArray<int>* pSpecularColor, double* opacity,
                              double* shininess) {
	auto& genModels = RhinoPRT::get().getGenModels();

	if (initialShapeIndex >= genModels.size()) {
		LOG_ERR << L"Initial shape ID out of range";
		return false;
	}

	auto& currModel = genModels[initialShapeIndex];
	auto& material = currModel.getMaterials();

	if (meshID >= material.size()) {
		LOG_ERR << L"Mesh ID is out of range";
		return false;
	}
	auto& mat = material.at(meshID);

	for (auto& texture : mat.mTexturePaths) {

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

RHINOPRT_API void SetMaterialGenerationOption(bool doGenerate) {
	RhinoPRT::get().setMaterialGeneration(doGenerate);
}

RHINOPRT_API bool GetDefaultValueBoolean(const wchar_t* key, bool* value) {
	return RhinoPRT::get().getDefaultValueBoolean(key, value);
}

RHINOPRT_API bool GetDefaultValueNumber(const wchar_t* key, double* value) {
	return RhinoPRT::get().getDefaultValueNumber(key, value);
}

RHINOPRT_API bool GetDefaultValueText(const wchar_t* key, ON_wString* pText) {
	return RhinoPRT::get().getDefaultValueText(key, pText);
}
}