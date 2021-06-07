#pragma once

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 26451)
#	pragma warning (disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "prt/API.h"
#include "prt/FileOutputCallbacks.h"
#include "prt/LogHandler.h"

#include <filesystem>
#include <memory>
#include <string>

struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

/**
 * commonly used constant
 */
const std::wstring ENCODER_ID_RHINO = L"com.esri.rhinoprt.RhinoEncoder";

// forward declaration to break include loop
class GeneratedModel;
class RhinoCallbacks;

namespace pcu {

	std::wstring getDllLocation();
	std::filesystem::path getTempDir(const std::wstring& tmp_prefix);
	std::wstring getUUID();
	std::filesystem::path getUniqueTempDir(const std::filesystem::path& tempDir, const std::wstring& basename);

	template <typename C>
	C getDirSeparator();
	template <>
	char getDirSeparator();
	template <>
	wchar_t getDirSeparator();

	struct ShapeAttributes {
		std::wstring ruleFile;
		std::wstring startRule;
		std::wstring shapeName;
		int seed;

		ShapeAttributes(const std::wstring rulef = L"bin/rule.cgb", const std::wstring startRl = L"Default$Lot",
			const std::wstring shapeN = L"Lot", int sd = 555);
	};

	struct EncoderOptions {
		bool emitReport = true;
		bool emitGeometry = true;
		bool emitMaterial = true;
	};


	/**
	 * helpers for prt object management
	 */
	struct PRTDestroyer {
		void operator()(const prt::Object* p) const {
			if (p)
				p->destroy();
		}
	};

	using ObjectPtr = std::unique_ptr<const prt::Object, PRTDestroyer>;
	using CachePtr = std::unique_ptr<prt::CacheObject, PRTDestroyer>;
	using ResolveMapPtr = std::unique_ptr<const prt::ResolveMap, PRTDestroyer>;
	using ResolveMapSPtr = std::shared_ptr<const prt::ResolveMap>;
	using InitialShapePtr = std::unique_ptr<const prt::InitialShape, PRTDestroyer>;
	using InitialShapeBuilderPtr = std::unique_ptr<prt::InitialShapeBuilder, PRTDestroyer>;
	using AttributeMapPtr = std::unique_ptr<const prt::AttributeMap, PRTDestroyer>;
	using AttributeMapPtrVector = std::vector<AttributeMapPtr>;
	using AttributeMapBuilderPtr = std::unique_ptr<prt::AttributeMapBuilder, PRTDestroyer>;
	using AttributeMapBuilderVector = std::vector<AttributeMapBuilderPtr>;
	using FileOutputCallbacksPtr = std::unique_ptr<prt::FileOutputCallbacks, PRTDestroyer>;
	using ConsoleLogHandlerPtr = std::unique_ptr<prt::ConsoleLogHandler, PRTDestroyer>;
	using FileLogHandlerPtr = std::unique_ptr<prt::FileLogHandler, PRTDestroyer>;
	using RuleFileInfoPtr = std::unique_ptr<const prt::RuleFileInfo, PRTDestroyer>;
	using EncoderInfoPtr = std::unique_ptr<const prt::EncoderInfo, PRTDestroyer>;
	using DecoderInfoPtr = std::unique_ptr<const prt::DecoderInfo, PRTDestroyer>;
	using SimpleOutputCallbacksPtr = std::unique_ptr<prt::SimpleOutputCallbacks, PRTDestroyer>;
	using RhinoCallbacksPtr = std::unique_ptr<RhinoCallbacks>;

	AttributeMapPtr createAttributeMapForShape(const ShapeAttributes& attrs, prt::AttributeMapBuilder& bld);
	AttributeMapPtr createAttributeMapForEncoder(const EncoderOptions& attrs, prt::AttributeMapBuilder& bld);
	AttributeMapPtr createValidatedOptions(const std::wstring& encID, const AttributeMapPtr& unvalidatedOptions);

	/**
	 * String and URI helpers
	 */
	std::string toOSNarrowFromUTF16(const std::wstring& osWString);
	std::wstring toUTF16FromOSNarrow(const std::string& osString);
	std::wstring toUTF16FromUTF8(const std::string& utf8String);
	std::string toUTF8FromUTF16(const std::wstring& utf16String);
	std::string toUTF8FromOSNarrow(const std::string& osString);

	void appendToRhinoString(ON_wString& rhinoString, const std::wstring& appendee);

	std::string percentEncode(const std::string& utf8String);
	std::string toFileURI(const std::string& osNarrowPath);
	std::string toFileURI(const std::wstring& utf16Path);

	std::wstring filename(const std::wstring& path);

	constexpr const wchar_t STYLE_DELIMITER = L'$';
	constexpr const wchar_t IMPORT_DELIMITER = L'.';
	constexpr const wchar_t* DEFAULT_STYLE_PREFIX = L"Default$";

	bool isDefaultStyle(const std::wstring& attrName);
	std::wstring removePrefix(const std::wstring& attrName, wchar_t delim);
	std::wstring removeImport(const std::wstring& attrName);
	std::wstring removeStyle(const std::wstring& attrName);

	template<typename C>
	std::basic_string<C>& replace_not_in_range(std::basic_string<C>& str, const std::basic_string<C>& range, const C& replacement) {
		std::replace_if(str.begin(), str.end(), [&range](C c) { return (range.find(c) == std::basic_string<C>::npos); }, replacement);
		return str;
	}

	/**
	 * Resolve map helpers
	 */
	constexpr const wchar_t* ANNOT_START_RULE = L"@StartRule";
	std::wstring getRuleFileEntry(const ResolveMapSPtr& resolveMap);
	std::wstring detectStartRule(const RuleFileInfoPtr& ruleFileInfo);

	std::wstring toAssetKey(std::wstring key);

	struct PathRemover {
		void operator()(std::filesystem::path const* p);
	};

	using ScopedPath = std::unique_ptr<std::filesystem::path, PathRemover>;


} // namespace pcu