#ifndef RHINOPRT
#define RHINOPRT

#include "version.h"
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

	class RhinoPRTAPI {
	public:
		const RuleAttribute RULE_NOT_FOUND{};

		bool InitializeRhinoPRT();
		void ShutdownRhinoPRT();
		bool IsPRTInitialized();

		void SetRPKPath(const std::wstring &rpk_path);

		int GetRuleAttributeCount();
		RuleAttributes GetRuleAttributes();

		void AddInitialShape(const std::vector<InitialShape>& shapes);
		void ClearInitialShapes();

		std::vector<GeneratedModel> GenerateGeometry();

		template<typename T>
		void fillAttributeFromNode(const std::wstring& ruleName, const std::wstring& attrFullName, T value);

		template<typename T>
		void setRuleAttributeValue(const RuleAttribute& rule, T value);

		int groupReportsByKeys();

		const Reporting::GroupedReports& getReports() const { return mGroupedReports; }
		Reporting::ReportsVector getReportsOfModel(int initialShapeID);

	private:

		std::vector<InitialShape> mShapes;
		std::wstring mPackagePath;
		std::vector<pcu::ShapeAttributes> mAttributes;

		RuleAttributes mRuleAttributes;
		pcu::AttributeMapBuilderPtr mAttrBuilder;

		pcu::EncoderOptions options;

		std::unique_ptr<ModelGenerator> mModelGenerator;
		std::vector<GeneratedModel> mGeneratedModels;

		Reporting::GroupedReports mGroupedReports;
	};

	// Global PRT handle
	RhinoPRTAPI& get();

} // namespace RhinoPRT

// Define exposed functions here
extern "C" {

	RHINOPRT_API void GetProductVersion(ON_wString* version_Str);

	RHINOPRT_API bool InitializeRhinoPRT();

	RHINOPRT_API void ShutdownRhinoPRT();

	RHINOPRT_API void SetPackage(const wchar_t* rpk_path);

	RHINOPRT_API bool AddMeshTest(ON_SimpleArray<const ON_Mesh*>* pMesh);

	RHINOPRT_API void ClearInitialShapes();

	RHINOPRT_API bool GenerateTest(ON_SimpleArray<ON_Mesh*>* pMeshArray);

	RHINOPRT_API int GetRuleAttributesCount();

	RHINOPRT_API bool GetRuleAttribute(int attrIdx, wchar_t* rule, int rule_size, wchar_t* name, int name_size, wchar_t* nickname, int nickname_size, prt::AnnotationArgumentType* type);

	RHINOPRT_API void SetRuleAttributeDouble(const wchar_t* rule, const wchar_t* fullName, double value);

	RHINOPRT_API void SetRuleAttributeBoolean(const wchar_t* rule, const wchar_t* fullName, bool value);

	RHINOPRT_API void SetRuleAttributeInteger(const wchar_t* rule, const wchar_t* fullName, int value);

	RHINOPRT_API void SetRuleAttributeString(const wchar_t* rule, const wchar_t* fullName, const wchar_t* value);

	RHINOPRT_API void GetReports(int initialShapeId, ON_ClassArray<ON_wString>* pKeysArray,
		ON_SimpleArray<double>* pDoubleReports,
		ON_SimpleArray<bool>* pBoolReports,
		ON_ClassArray<ON_wString>* pStringReports);

	inline RHINOPRT_API bool GetMaterial(int meshID, int* uvSet, wchar_t* pColorMapTex, int pColorMapTexSize)
	{
		const auto& genModels = RhinoPRT::myPRTAPI->getGenModels();

		if (meshID >= genModels.size()) {
			LOG_ERR << L"ShapeID out of range";
			return false;
		}

		auto& currModel = genModels[meshID];
		auto& material = currModel.getMaterials();
		auto& mat = material.at(0); // First facerange

		if (mat.mColormapTexID != -1) {
			//a colormap is present -> get the colormap texture path

			*uvSet = 0;
			auto path = mat.mRhinoMat.m_textures[mat.mColormapTexID].m_image_file_reference.RelativePath();
			auto path_str = std::wstring(path.Array());
			wcscpy_s(pColorMapTex, pColorMapTexSize, path_str.c_str());
			return true;
		}

		return false;
	}

}

#endif RHINOPRT