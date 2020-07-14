#ifndef RHINOPRT
#define RHINOPRT

#include "wrap.h"

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
	/*
	struct IRhinoPRTAPI {
		virtual bool InitializeRhinoPRT() = 0;
		virtual void ReleaseRhinoPRT() = 0;
		virtual bool IsPRTInitialized() = 0;
		virtual void SetRPKPath(const std::wstring &rpk_path) = 0;
		virtual bool AddInitialShape(double* vertices, int vCount, int* indices, int iCount, int* faceCount, int faceCountCount) = 0;
		virtual bool GenerateGeometry(double** vertices, int* vCount, int** indices, int* iCount, int** faceCount, int* faceCountCount) = 0;
		virtual int GetRuleAttributeCount() = 0;
	};
	*/
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
			mPackagePath = pcu::toOSNarrowFromUTF16(rpk_path);

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
			mAttributes.push_back(pcu::ShapeAttributes());
			
			return true;
		}

		void AddInitialShape(std::vector<InitialShape>& shapes) {
			for (auto&shape : shapes) {
				mShapes.push_back(shape);
				mAttributes.push_back(pcu::ShapeAttributes());
			}
		}

		std::vector<GeneratedModel> GenerateGeometry() {
			return mModelGenerator->generateModel(mShapes, mAttributes, mPackagePath, L"com.esri.rhinoprt.RhinoEncoder", options, mAttrBuilder);
		}

		bool GenerateGeometry(double** vertices, int* vCount, int** indices, int* iCount, int** faceCount, int* faceCountCount) {

			auto generated_models = mModelGenerator->generateModel(mShapes, mAttributes, mPackagePath, L"com.esri.rhinoprt.RhinoEncoder", options, mAttrBuilder);

			if (generated_models.size() == 0) return false;

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
		void fillAttributeFromNode(std::wstring& ruleName, std::wstring& attrFullName, T value) {

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
			if (rule.mType == prt::AAT_FLOAT) {
				mAttrBuilder->setFloat(rule.mFullName.c_str(), value);
			}
		}

	private:

		std::vector<InitialShape> mShapes;
		std::string mPackagePath;
		std::vector<pcu::ShapeAttributes> mAttributes;

		RuleAttributes mRuleAttributes;
		pcu::AttributeMapBuilderPtr mAttrBuilder;

		pcu::EncoderOptions options;

		std::unique_ptr<ModelGenerator> mModelGenerator;
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

	inline RHINOPRT_API bool Generate(double** vertices, int* vCount, int** indices, int* iCount, int** faceCount, int* faceCountCount) {
		bool status = RhinoPRT::myPRTAPI->GenerateGeometry(vertices, vCount, indices, iCount, faceCount, faceCountCount);

		LOG_DBG << L"Generated geometry" << std::endl;

		/*for (int i = 0; i < vCount; i+=3) {
			LOG_DBG << " " << vertices[0][i] << " " << vertices[0][i+1] << " " << vertices[0][i+2] << std::endl;
		}*/

		return status;
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
		std::wstring ruleFile(rule);
		std::wstring attrFullName(fullName);


		RhinoPRT::myPRTAPI->fillAttributeFromNode<double>(ruleFile, attrFullName, value);
	}
}

#endif RHINOPRT