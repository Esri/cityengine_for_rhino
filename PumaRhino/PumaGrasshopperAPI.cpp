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

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "RhinoPRT.h"
#include "version.h"
#include "utils.h"

#define RHINOPRT_API __declspec(dllexport)

namespace {

template <typename T, typename T1>
T static_cast_fct(const T1& x) {
	return static_cast<T>(x);
}

} // namespace

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

/*
RHINOPRT_API void SetPackage(const wchar_t* rpk_path, ON_wString* errorMsg) {
	assert(rpk_path != nullptr); // guaranteed by managed call site
	try {
		RhinoPRT::get().SetRPKPath(rpk_path);
	}
	catch (std::exception& e) {
		*errorMsg += pcu::toUTF16FromOSNarrow(e.what()).c_str();
	}
}

RHINOPRT_API bool GetPackagePath(ON_wString* pRpk) {
	if (pRpk == nullptr)
		return false;

	const std::wstring rpk = RhinoPRT::get().GetRPKPath();
	if (rpk.empty())
		return false;

	*pRpk += rpk.c_str();
	return true;
}

inline RHINOPRT_API bool AddInitialMesh(ON_SimpleArray<const ON_Mesh*>* pMesh) {
	if (pMesh == nullptr)
		return false;

	std::vector<RawInitialShape> rawInitialShapes;
	rawInitialShapes.reserve(pMesh->Count());
	for (int i = 0; i < pMesh->Count(); ++i) {
		rawInitialShapes.emplace_back(**pMesh->At(i));
	}

	RhinoPRT::get().SetInitialShapes(rawInitialShapes);
	return true;
}
*/

RHINOPRT_API bool Generate(const wchar_t* rpk_path, ON_wString* errorMsg,
						   // rule attributes
						   const int shapeCount,
						   ON_SimpleArray<int>* pBoolStarts, const int boolCount,
						   ON_ClassArray<ON_wString>* pBoolKeys, ON_SimpleArray<int>* pBoolVals,

						   ON_SimpleArray<int>* pDoubleStarts, const int doubleCount,
						   ON_ClassArray<ON_wString>* pDoubleKeys, ON_SimpleArray<double>* pDoubleVals,

						   ON_SimpleArray<int>* pStringStarts, const int stringCount, 
						   ON_ClassArray<ON_wString>* pStringKeys, ON_ClassArray<ON_wString>* pStringVals,

						   ON_SimpleArray<int>* pBoolArrayStarts, const int boolArrayCount,
                           ON_ClassArray<ON_wString>* pBoolArrayKeys, ON_ClassArray<ON_wString>* pBoolArrayVals,

						   ON_SimpleArray<int>* pDoubleArrayStarts, const int doubleArrayCount,
                           ON_ClassArray<ON_wString>* pDoubleArrayKeys, ON_ClassArray<ON_wString>* pDoubleArrayVals,

						   ON_SimpleArray<int>* pStringArrayStarts, const int stringArrayCount,
                           ON_ClassArray<ON_wString>* pStringArrayKeys, ON_ClassArray<ON_wString>* pStringArrayVals,

						   // Initial geometry
                           ON_SimpleArray<const ON_Mesh*>* pMesh,

						   // Resulting geometry
						   ON_SimpleArray<int>* pMeshCounts,
                           ON_SimpleArray<ON_Mesh*>* pMeshArray,
							
						   // Materials,
                           ON_SimpleArray<int>* pColorsArray, ON_SimpleArray<int>* pMatIndices,
                           ON_ClassArray<ON_wString>* pTexKeys, ON_ClassArray<ON_wString>* pTexPaths,
	
						   // Reports
						   ON_SimpleArray<int>* pReportsCountArray,
                           ON_ClassArray<ON_wString>* pKeysArray, ON_SimpleArray<double>* pDoubleReports,
                           ON_SimpleArray<bool>* pBoolReports, ON_ClassArray<ON_wString>* pStringReports) {

	if (pMesh == nullptr)
		return false;

	std::vector<RawInitialShape> rawInitialShapes;
	rawInitialShapes.reserve(pMesh->Count());
	for (int i = 0; i < pMesh->Count(); ++i) {
		rawInitialShapes.emplace_back(**pMesh->At(i));
	}

	// Initialise the attribute map builders for each initial shape.
	pcu::AttributeMapBuilderVector aBuilders(shapeCount);
	for (auto& it : aBuilders) {
		it.reset(prt::AttributeMapBuilder::create());
	}
	
	for (int i = 0; i < shapeCount; ++i) {
		// by initial shape
		int indexStartBool = *pBoolStarts->At(i);
		int indexStartDouble = *pDoubleStarts->At(i);
		int indexStartString = *pStringStarts->At(i);
		int indexStartBoolArray = *pBoolArrayStarts->At(i);
		int indexStartDoubleArray = *pDoubleArrayStarts->At(i);
		int indexStartStringArray = *pStringArrayStarts->At(i);

		int boolAttrCount = (i < boolCount - 1 ? *pBoolStarts->At(i + 1) : boolCount) - indexStartBool;
		int doubleAttrCount = (i < doubleCount - 1 ? *pDoubleStarts->At(i + 1): doubleCount) - indexStartDouble;
		int stringAttrCount = (i < stringCount - 1 ? *pStringStarts->At(i + 1): stringCount) - indexStartString;
		int boolAttrArrayCount = (i < boolArrayCount - 1 ? *pBoolArrayStarts->At(i + 1): boolArrayCount) - indexStartBoolArray;
		int doubleAttrArrayCount = (i < doubleArrayCount - 1 ? *pDoubleArrayStarts->At(i + 1): doubleArrayCount) - indexStartDoubleArray;
		int stringAttrArrayCount = (i < stringArrayCount - 1 ? *pStringArrayStarts->At(i + 1): stringArrayCount) - indexStartStringArray;

		pcu::unpackBoolAttributes(indexStartBool, boolAttrCount, pBoolKeys, pBoolVals, aBuilders[i]);
		pcu::unpackDoubleAttributes(indexStartDouble, doubleAttrCount, pDoubleKeys, pDoubleVals, aBuilders[i]);
		pcu::unpackStringAttributes(indexStartString, stringAttrCount, pStringKeys, pStringVals,
		                      aBuilders[i], false);
		pcu::unpackBoolArrayAttributes(indexStartBoolArray, boolAttrArrayCount, pBoolArrayKeys, pBoolArrayVals,
		                           aBuilders[i]);
		pcu::unpackDoubleArrayAttributes(indexStartDoubleArray, doubleAttrArrayCount, pDoubleArrayKeys, pDoubleArrayVals, aBuilders[i]);
		pcu::unpackStringAttributes(indexStartStringArray, stringAttrArrayCount, pStringArrayKeys, pStringArrayVals,
		                            aBuilders[i], true);

		indexStartBool += boolAttrCount;
		indexStartDouble += doubleAttrCount;
		indexStartString += stringAttrCount;
		indexStartBoolArray += boolAttrArrayCount;
		indexStartDoubleArray += doubleAttrArrayCount;
		indexStartStringArray += stringAttrArrayCount;
	}

	const auto& models = RhinoPRT::get().GenerateGeometry(std::wstring(rpk_path), rawInitialShapes, aBuilders);

	for (size_t i = 0; i < models.size(); i++) {
		if (models[i]) {
			const GeneratedModel::MeshBundle meshBundle =
			        models[i]->createRhinoMeshes(i);
			pMeshCounts->Append(static_cast<int>(meshBundle.size()));
			for (const auto& meshPart : meshBundle) {
				pMeshArray->Append(new ON_Mesh(meshPart));
			}

			// Materials
			pMatIndices->Append(meshBundle.size());

			const auto& materials = models[i]->getMaterials();
			for (const auto& material : materials) {
				const auto& matAttributes = material.second;
				pcu::appendColor(matAttributes.mDiffuseCol, pColorsArray);
				pcu::appendColor(matAttributes.mAmbientCol, pColorsArray);
				pcu::appendColor(matAttributes.mSpecularCol, pColorsArray);
				pColorsArray->Append(matAttributes.mOpacity);
				pColorsArray->Append(matAttributes.mShininess);

				pMatIndices->Append(matAttributes.mTexturePaths.size());

				for (auto& texture : matAttributes.mTexturePaths) {

#ifdef DEBUG
					LOG_DBG << L"texture: [ " << texture.first << " : " << texture.second << "]";
#endif // DEBUG

					pTexKeys->Append(ON_wString(texture.first.c_str()));
					pTexPaths->Append(ON_wString(texture.second.c_str()));
				}
			}

			// Reports
			auto reports = Reporting::ToReportsVector(models[i]->getReports());

			/*
			left.float	-> right.all OK
			left.bool	-> right.float OK
						-> right.bool OK
						-> right.string OK
			left.string -> right.all OK
			*/
			// Sort the reports by Type. The order is Double -> Bool -> String
			std::sort(reports.begin(), reports.end(),
			          [](Reporting::ReportAttribute left, Reporting::ReportAttribute right) -> bool {
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

			int doubleReportsCount = 0;
			int boolReportsCount = 0;
			int stringReportsCount = 0;

			for (const auto& report : reports) {
				pKeysArray->Append(ON_wString(report.mReportName.c_str()));

				switch (report.mType) {
					case prt::AttributeMap::PrimitiveType::PT_FLOAT:
						pDoubleReports->Append(report.mDoubleReport);
						doubleReportsCount++;
						break;
					case prt::AttributeMap::PrimitiveType::PT_BOOL:
						pBoolReports->Append(report.mBoolReport);
						boolReportsCount++;
						break;
					case prt::AttributeMap::PrimitiveType::PT_STRING:
						pStringReports->Append(ON_wString(report.mStringReport.c_str()));
						stringReportsCount++;
						break;
					default:
						// REMOVE LAST KEY
						pKeysArray->Remove(pKeysArray->Count() - 1);
				}
			}

			pReportsCountArray->Append(doubleReportsCount);
			pReportsCountArray->Append(boolReportsCount);
			pReportsCountArray->Append(stringReportsCount);
		}
		else {
			pMeshCounts->Append(0);
		}
	}

	return !models.empty();
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

RHINOPRT_API void GetCGAPrintOutput(int initialShapeIndex, ON_ClassArray<ON_wString>* pPrintOutput) {
	const std::vector<GeneratedModelPtr> models; // = RhinoPRT::get().getGenModels();

	if ((models.size() <= initialShapeIndex) || !models[initialShapeIndex])
		return;

	const std::vector<std::wstring>& printOutput = models[initialShapeIndex]->getPrintOutput();
	for (const std::wstring& item : printOutput)
		pPrintOutput->Append(ON_wString(item.c_str()));
}

RHINOPRT_API void GetCGAErrorOutput(int initialShapeIndex, ON_ClassArray<ON_wString>* pErrorOutput) {
	const std::vector<GeneratedModelPtr> models; // = RhinoPRT::get().getGenModels();

	if ((models.size() <= initialShapeIndex) || !models[initialShapeIndex])
		return;

	const std::vector<std::wstring>& errorOutput = models[initialShapeIndex]->getErrorOutput();
	for (const std::wstring& item : errorOutput)
		pErrorOutput->Append(ON_wString(item.c_str()));
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

RHINOPRT_API void SetMaterialGenerationOption(bool doGenerate) {
	RhinoPRT::get().setMaterialGeneration(doGenerate);
}

RHINOPRT_API bool GetDefaultValuesBoolean(const wchar_t* key, ON_SimpleArray<int>* pValue) {
	return RhinoPRT::get().getDefaultValuesBoolean(key, pValue);
}

RHINOPRT_API bool GetDefaultValuesNumber(const wchar_t* key, ON_SimpleArray<double>* pValue) {
	return RhinoPRT::get().getDefaultValuesNumber(key, pValue);
}

RHINOPRT_API bool GetDefaultValuesText(const wchar_t* key, ON_ClassArray<ON_wString>* pTexts) {
	return RhinoPRT::get().getDefaultValuesText(key, pTexts);
}

RHINOPRT_API bool GetDefaultValuesBooleanArray(const wchar_t* key, ON_SimpleArray<int>* pValues, ON_SimpleArray<int>* pSizes) {
	return RhinoPRT::get().getDefaultValuesBooleanArray(key, pValues, pSizes);
}

RHINOPRT_API bool GetDefaultValuesNumberArray(const wchar_t* key, ON_SimpleArray<double>* pValues, ON_SimpleArray<int>* pSizes) {
	return RhinoPRT::get().getDefaultValuesNumberArray(key, pValues, pSizes);
}

RHINOPRT_API bool GetDefaultValuesTextArray(const wchar_t* key, ON_ClassArray<ON_wString>* pTexts, ON_SimpleArray<int>* pSizes) {
	return RhinoPRT::get().getDefaultValuesTextArray(key, pTexts, pSizes);
}
}