#ifndef RHINOPRT
#define RHINOPRT

#include "PRTContext.h"

#include "ModelGenerator.h"

#include "Logger.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <string>

#pragma comment(lib, "ole32.lib")


// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the RHINOPRT_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// RHINOPRT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#define RHINOPRT_EXPORTS
#ifdef RHINOPRT_EXPORTS
#define RHINOPRT_API __declspec(dllexport)
#else
#define RHINOPRT_API __declspec(dllimport)
#endif

// Exposed interface
namespace RhinoPRT {

	struct RhinoPRTAPI {
	public:

		// Inherited via IRhinoPRTAPI
		bool InitializeRhinoPRT() {
			return PRTContext::get().isAlive();
		}

		void ReleaseRhinoPRT() {
			
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
			int seed = 555;

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
			return mModelGenerator->generateModel(mShapes, mAttributes, L"com.esri.rhinoprt.RhinoEncoder", options, mAttrBuilder);
		}

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

	private:

		std::vector<InitialShape> mShapes;
		std::wstring mPackagePath;
		std::vector<pcu::ShapeAttributes> mAttributes;

		RuleAttributes mRuleAttributes;
		pcu::AttributeMapBuilderPtr mAttrBuilder;

		pcu::EncoderOptions options;

		std::unique_ptr<ModelGenerator> mModelGenerator;
		std::vector<GeneratedModel> mGeneratedModels;
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
		RhinoPRT::myPRTAPI->ReleaseRhinoPRT();
	}

	inline RHINOPRT_API void SetPackage(const wchar_t* rpk_path) {
		std::wstring str(rpk_path);
		RhinoPRT::myPRTAPI->SetRPKPath(str);
	}

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

	inline RHINOPRT_API bool Generate(double** vertices, int* vCount, int** indices, int* iCount, int** faceCount, int* faceCountCount) {
		bool status = RhinoPRT::myPRTAPI->GenerateGeometry(vertices, vCount, indices, iCount, faceCount, faceCountCount);

		LOG_DBG << L"Generated geometry" << std::endl;

		/*for (int i = 0; i < vCount; i+=3) {
			LOG_DBG << " " << vertices[0][i] << " " << vertices[0][i+1] << " " << vertices[0][i+2] << std::endl;
		}*/

		return status;
	}

	inline RHINOPRT_API bool GenerateTest(ON_SimpleArray<ON_Mesh*>* pMeshArray) {
		if (pMeshArray == nullptr) return false;

		auto meshes = RhinoPRT::myPRTAPI->GenerateGeometry();

		if (meshes.size() == 0) return false;

		for (const auto& mesh : meshes) {
			const auto on_mesh = pcu::getMeshFromGenModel(mesh);
			pMeshArray->Append(new ON_Mesh(on_mesh)); // must be freed my the caller of this function.
		}

		return true;
	}

	inline RHINOPRT_API int GetRuleAttributesCount() {
		return RhinoPRT::myPRTAPI->GetRuleAttributeCount();
	}

	inline RHINOPRT_API bool GetRuleAttribute(int attrIdx, wchar_t* rule, int rule_size, wchar_t* name, int name_size, prt::AnnotationArgumentType* type) {
		RuleAttributes ruleAttributes = RhinoPRT::myPRTAPI->GetRuleAttributes();

		if (attrIdx >= ruleAttributes.size()) return false;

		wcscpy_s(rule, rule_size, ruleAttributes[attrIdx].mRuleFile.c_str());
		wcscpy_s(name, name_size, ruleAttributes[attrIdx].mFullName.c_str());
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
}

#endif RHINOPRT