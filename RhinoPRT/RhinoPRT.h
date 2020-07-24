#ifndef RHINOPRT
#define RHINOPRT

#include "PRTContext.h"

#include "ModelGenerator.h"

#include "Logger.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <map>

#pragma comment(lib, "ole32.lib") // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#define RHINOPRT_API __declspec(dllexport)

namespace RhinoPRT {

	using GroupedReportMap = std::map<std::wstring, std::vector<Reporting::ReportAttribute>>;

	struct RhinoPRTAPI {
	public:

		bool InitializeRhinoPRT() {
			return PRTContext::get().isAlive();
		}

		void ShutdownRhinoPRT() {
			PRTContext::get().~PRTContext();
		}

		bool IsPRTInitialized() {
			return PRTContext::get().isAlive();
		}

		void SetRPKPath(const std::wstring &rpk_path) {
			if (wcscmp(mPackagePath.c_str(), rpk_path.c_str()) == 0) return;

			mPackagePath = rpk_path;

			// initialize the resolve map and rule infos here. Create the vector of rule attributes.
			if (!mModelGenerator)
				mModelGenerator = std::unique_ptr<ModelGenerator>(new ModelGenerator());

			mRuleAttributes = mModelGenerator->updateRuleFiles(mPackagePath);

			// Also create the attribute map builder that will receive the rule attributes.
			mAttrBuilder.reset(prt::AttributeMapBuilder::create());
		}

		int GetRuleAttributeCount() {
			return mRuleAttributes.size();
		}

		RuleAttributes GetRuleAttributes() {
			return mRuleAttributes;
		}

		bool AddInitialShape(double* vertices, int vCount, int* indices, int iCount, int* faceCount, int faceCountCount) {

			mShapes.push_back(InitialShape(vertices, vCount, indices, iCount, faceCount, faceCountCount));

			if (mModelGenerator.get()) {
				// get the shape attributes data from ModelGenerator
				auto rulef = mModelGenerator->getRuleFile();
				auto ruleN = mModelGenerator->getStartingRule();
				auto shapeN = mModelGenerator->getDefaultShapeName();
				int seed = 555; // TODO: compute seed?
				mAttributes.push_back(pcu::ShapeAttributes(rulef, ruleN, shapeN, seed));
			}
			else {
				mAttributes.push_back(pcu::ShapeAttributes());
			}
			
			return true;
		}

		void AddInitialShape(std::vector<InitialShape>& shapes) {

			// get the shape attributes data from ModelGenerator
			auto rulef = mModelGenerator->getRuleFile();
			auto ruleN = mModelGenerator->getStartingRule();
			auto shapeN = mModelGenerator->getDefaultShapeName();
			int seed = 555; // TODO: compute seed?

			for (auto&shape : shapes) {
				mShapes.push_back(shape);
				mAttributes.push_back(pcu::ShapeAttributes(rulef, ruleN, shapeN, seed));
			}
		}

		void ClearInitialShapes() {
			mShapes.clear();
			mAttributes.clear();
		}

		std::vector<GeneratedModel> GenerateGeometry() {
			mGeneratedModels = mModelGenerator->generateModel(mShapes, mAttributes, ENCODER_ID_RHINO, options, mAttrBuilder);
			return mGeneratedModels;
		}

		/// NOT USED ANYMORE.
		/// This is the previous way of starting the geometry generation and passing back the results, before ON_SimpleArray<const ON_Mesh*> was used.
		bool GenerateGeometry(double** vertices, int* vCount, int** indices, int* iCount, int** faceCount, int* faceCountCount) {

			auto generated_models = GenerateGeometry();

			if (generated_models.size() == 0) return false;

			mGeneratedModels.clear();
			mGeneratedModels.reserve(generated_models.size());
			mGeneratedModels.insert(mGeneratedModels.begin(), generated_models.begin(), generated_models.end());

			// TODO check if array is big enough to contain all the generated vertices/indices.
			*vCount = generated_models[0].getVertices().size();
			int id = 0;
			for (double v : generated_models[0].getVertices()) {
				(*vertices)[id] = v;
				id++;
			}

			*iCount = generated_models[0].getVertices().size();
			id = 0;
			for (int i : generated_models[0].getIndices()) {
				(*indices)[id] = i;
				++id;
			}

			*faceCountCount = generated_models[0].getFaces().size();
			id = 0;
			for (int i : generated_models[0].getFaces()) {
				(*faceCount)[id] = i;
				++id;
			}

			return true;
		}

		const RuleAttribute RULE_NOT_FOUND{};

		template<typename T>
		void fillAttributeFromNode(const std::wstring& ruleName, const std::wstring& attrFullName, T value);

		template<typename T>
		void setRuleAttributeValue(const RuleAttribute& rule, T value);

		void groupReportsByKeys(int* strReportCount, int* boolReportCount, int* doubleReportCount);

		bool getReportKeys(ON_ClassArray<ON_wString>* pKeysArray, ON_SimpleArray<int>* pKeyTypeArray);

	private:

		std::vector<InitialShape> mShapes;
		std::wstring mPackagePath;
		std::vector<pcu::ShapeAttributes> mAttributes;

		RuleAttributes mRuleAttributes;
		pcu::AttributeMapBuilderPtr mAttrBuilder;

		pcu::EncoderOptions options;

		std::unique_ptr<ModelGenerator> mModelGenerator;
		std::vector<GeneratedModel> mGeneratedModels;
		GroupedReportMap mGroupedStringReports;
		GroupedReportMap mGroupedBoolReports;
		GroupedReportMap mGroupedDoubleReports;
	};

	// Global PRT handle
	const std::unique_ptr<RhinoPRT::RhinoPRTAPI> myPRTAPI(new RhinoPRT::RhinoPRTAPI());
}

// Define exposed functions here
extern "C" {

	inline RHINOPRT_API bool InitializeRhinoPRT() {
		return RhinoPRT::myPRTAPI->InitializeRhinoPRT();
	}

	inline RHINOPRT_API void ShutdownRhinoPRT() {
		RhinoPRT::myPRTAPI->ShutdownRhinoPRT();
	}

	inline RHINOPRT_API void SetPackage(const wchar_t* rpk_path) {
		std::wstring str(rpk_path);
		RhinoPRT::myPRTAPI->SetRPKPath(str);
	}

	/// Not used anymore
	inline RHINOPRT_API bool AddShape(double* vertices, int vCount, int* indices, int iCount, int* faceCount, int faceCountCount) {
		return RhinoPRT::myPRTAPI->AddInitialShape(vertices, vCount, indices, iCount, faceCount, faceCountCount);
	}

	inline RHINOPRT_API bool AddMeshTest(ON_SimpleArray<const ON_Mesh*>* pMesh) {
		if (pMesh == nullptr) return false;
		
		std::vector<InitialShape> initShapes;
		for (int i = 0; i < pMesh->Count(); ++i) {
			initShapes.push_back(InitialShape(**pMesh->At(i)));
		}

		RhinoPRT::myPRTAPI->AddInitialShape(initShapes);
		return true;
	}

	inline RHINOPRT_API void ClearInitialShapes() {
		RhinoPRT::myPRTAPI->ClearInitialShapes();
	}

	/// Not used anymore
	inline RHINOPRT_API bool Generate(double** vertices, int* vCount, int** indices, int* iCount, int** faceCount, int* faceCountCount) {
		bool status = RhinoPRT::myPRTAPI->GenerateGeometry(vertices, vCount, indices, iCount, faceCount, faceCountCount);

		LOG_DBG << L"Generated geometry" << std::endl;

		/*for (int i = 0; i < vCount; i+=3) {
			LOG_DBG << " " << vertices[0][i] << " " << vertices[0][i+1] << " " << vertices[0][i+2] << std::endl;
		}*/

		return status;
	}

	inline RHINOPRT_API bool GenerateTest(ON_SimpleArray<ON_Mesh*>* pMeshArray) {
		if (pMeshArray == nullptr) { 
			LOG_ERR << L"Aborting generation, given a null Mesh array.";
			return false;
		}

		auto meshes = RhinoPRT::myPRTAPI->GenerateGeometry();

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

	inline RHINOPRT_API int GetRuleAttributesCount() {
		return RhinoPRT::myPRTAPI->GetRuleAttributeCount();
	}

	inline RHINOPRT_API bool GetRuleAttribute(int attrIdx, wchar_t* rule, int rule_size, wchar_t* name, int name_size, wchar_t* nickname, int nickname_size, prt::AnnotationArgumentType* type) {
		RuleAttributes ruleAttributes = RhinoPRT::myPRTAPI->GetRuleAttributes();

		if (attrIdx >= ruleAttributes.size()) return false;

		wcscpy_s(rule, rule_size, ruleAttributes[attrIdx].mRuleFile.c_str());
		wcscpy_s(name, name_size, ruleAttributes[attrIdx].mFullName.c_str());
		wcscpy_s(nickname, nickname_size, ruleAttributes[attrIdx].mNickname.c_str());
		*type = ruleAttributes[attrIdx].mType;

		return true;
	}

	inline RHINOPRT_API void SetRuleAttributeDouble(const wchar_t* rule, const wchar_t* fullName, double value) {
		RhinoPRT::myPRTAPI->fillAttributeFromNode<double>(std::wstring(rule), std::wstring(fullName), value);
	}

	inline RHINOPRT_API void SetRuleAttributeBoolean(const wchar_t* rule, const wchar_t* fullName, bool value) {
		RhinoPRT::myPRTAPI->fillAttributeFromNode<bool>(std::wstring(rule), std::wstring(fullName), value);
	}

	inline RHINOPRT_API void SetRuleAttributeInteger(const wchar_t* rule, const wchar_t* fullName, int value) {
		RhinoPRT::myPRTAPI->fillAttributeFromNode<int>(std::wstring(rule), std::wstring(fullName), value);
	}

	inline RHINOPRT_API void SetRuleAttributeString(const wchar_t* rule, const wchar_t* fullName, const wchar_t* value) {
		RhinoPRT::myPRTAPI->fillAttributeFromNode<std::wstring>(std::wstring(rule), std::wstring(fullName), std::wstring(value));
	}

	inline RHINOPRT_API void GroupeReportsByKeys(int* strReportCount, int* boolReportCount, int* doubleReportCount) {
		RhinoPRT::myPRTAPI->groupReportsByKeys(strReportCount, boolReportCount, doubleReportCount);
	}

	inline RHINOPRT_API bool GetReportKeys(ON_ClassArray<ON_wString>* pKeysArray,  ON_SimpleArray<int>* pKeyTypeArray) {
		return RhinoPRT::myPRTAPI->getReportKeys(pKeysArray, pKeyTypeArray);
	}
}

#endif RHINOPRT