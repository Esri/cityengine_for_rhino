#include "utils.h"

#include "Logger.h"
#include "PRTUtilityModels.h"

#include "prt/StringUtils.h"

#include <rpc.h>

#ifdef _WIN32
	#include <Windows.h>
	#include <conio.h>
#else
#	include <dlfcn.h>
#endif

#include <filesystem>

#include <cwchar>
#include <sstream>
#include <string>

namespace pcu {

	ShapeAttributes::ShapeAttributes(const std::wstring rulef, const std::wstring startRl,
		const std::wstring shapeN) : ruleFile(rulef), startRule(startRl), shapeName(shapeN) { }


	// location of RhinoPRT shared library
	std::wstring getDllLocation() {
#ifdef _WIN32
		char dllPath[_MAX_PATH];
		char drive[8];
		char dir[_MAX_PATH];
		HMODULE hModule = 0;

		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
						  (LPCWSTR)getDllLocation, &hModule);
		const DWORD res = ::GetModuleFileNameA(hModule, dllPath, _MAX_PATH);
		if (res == 0) {
			// TODO DWORD e = ::GetLastError();
			throw std::runtime_error("failed to get plugin location");
		}

		_splitpath_s(dllPath, drive, 8, dir, _MAX_PATH, 0, 0, 0, 0);
		std::wstring rootPath = pcu::toUTF16FromOSNarrow(drive);
		rootPath.append(pcu::toUTF16FromOSNarrow(dir));
#else
		Dl_info dl_info;
		dladdr((const void*)getPluginRoot, &dl_info);
		const std::string tmp(dl_info.dli_fname);
		std::wstring rootPath = pcu::toUTF16FromOSNarrow(tmp.substr(0, tmp.find_last_of(pcu::getDirSeparator<char>())));
#endif

		// ensure path separator at end
		if (*rootPath.rbegin() != pcu::getDirSeparator<wchar_t>())
			rootPath.append(1, pcu::getDirSeparator<wchar_t>());

		return rootPath;
	}

	template <>
	char getDirSeparator() {
#ifdef _WIN32
		static const char SEPARATOR = '\\';
#else
		static const char SEPARATOR = '/';
#endif
		return SEPARATOR;
	}

	template <>
	wchar_t getDirSeparator() {
#ifdef _WIN32
		static const wchar_t SEPARATOR = L'\\';
#else
		static const wchar_t SEPARATOR = L'/';
#endif
		return SEPARATOR;
	}

	template <>
	std::string getDirSeparator() {
		return std::string(1, getDirSeparator<char>());
	}

	template <>
	std::wstring getDirSeparator() {
		return std::wstring(1, getDirSeparator<wchar_t>());
	}

	std::filesystem::path getTempDir(const std::wstring& tmp_prefix)
	{
		const auto tempPath = std::filesystem::temp_directory_path();

		auto prtTempPath = tempPath / tmp_prefix;
		if (!std::filesystem::exists(prtTempPath)) {
			if (!std::filesystem::create_directory(prtTempPath)) 
			{
				LOG_ERR << L"Could not create PRT temporary directory at " << prtTempPath.wstring() << L". Returning the default temp dir at " << tempPath.wstring();
				return tempPath;
			}
		}

		return prtTempPath;
	}

	std::filesystem::path getUniqueTempDir(const std::filesystem::path& tempDir, const std::wstring& basename)
	{
		const std::wstring uuid = getUUID();
		const auto uniqueTempDir = tempDir / (basename + uuid);

		if (!std::filesystem::create_directory(uniqueTempDir))
		{
			LOG_ERR << L"Could not create unique temporary directory at " << uniqueTempDir.wstring() << L", returning default temp dir.";
			return tempDir;
		}

		return uniqueTempDir;
	}

	std::wstring getUUID()
	{
#ifdef _WIN32
		RPC_STATUS status;
		
		UUID uid;
		status = UuidCreate(&uid);
		if (status != RPC_S_OK)
			throw std::runtime_error("Failed to create UUID");

		wchar_t* str = nullptr;
		status = UuidToStringW(&uid, (RPC_WSTR*)&str);
		if (status == RPC_S_OK && str != nullptr && std::wcslen(str) > 0) {
			return std::wstring(str);
		}

		LOG_ERR << "Failed to create UUID";
		throw std::runtime_error("Failed to create UUID");		
#else
#error Windows platform required
#endif
	}

#ifdef _WIN32
	const std::string FILE_SCHEMA = "file:/";
#else
	const std::string FILE_SCHEMA = "file:";
#endif

	/**
	 * Helper function to add the ShapeAttributes infos to an existing prt::AttributeMap.
	 */
	AttributeMapPtr createAttributeMapForShape(const ShapeAttributes& attrs, prt::AttributeMapBuilder& bld) {
		bld.setString(L"ruleFile", attrs.ruleFile.c_str());
		bld.setString(L"startRule", attrs.startRule.c_str());
		bld.setString(L"shapeName", attrs.shapeName.c_str());

		return AttributeMapPtr{ bld.createAttributeMap() };
	}

	AttributeMapPtr createAttributeMapForEncoder(const EncoderOptions& encOpts, prt::AttributeMapBuilder& bld) {
		bld.setBool(L"emitReport", encOpts.emitReport);
		bld.setBool(L"emitGeometry", encOpts.emitGeometry);
		bld.setBool(L"emitMaterials", encOpts.emitMaterial);

		return AttributeMapPtr{ bld.createAttributeMap() };
	}

	AttributeMapPtr createValidatedOptions(const std::wstring & encID, const AttributeMapPtr & unvalidatedOptions) {
		const EncoderInfoPtr encInfo{ prt::createEncoderInfo(encID.c_str()) };

		if (!encInfo) {
			LOG_ERR << "Failed to create encoder info: encoder not found for given ID." << std::endl;
			return nullptr;
		}

		const prt::AttributeMap* validatedOptions = nullptr;
		encInfo->createValidatedOptionsAndStates(unvalidatedOptions.get(), &validatedOptions);
		return AttributeMapPtr(validatedOptions);
	}

	template <typename inC, typename outC, typename FUNC>
	std::basic_string<outC> callAPI(FUNC f, const std::basic_string<inC>& s) {
		std::vector<outC> buffer(s.size());
		size_t size = buffer.size();
		f(s.c_str(), buffer.data(), &size, nullptr);
		if (size > buffer.size()) {
			buffer.resize(size);
			f(s.c_str(), buffer.data(), &size, nullptr);
		}
		return std::basic_string<outC>{buffer.data()};
	}

	std::wstring filename(const std::wstring& path) {
		size_t pos = path.find_last_of(L'/');
		if (pos != std::string::npos) {
			return path.substr(pos + 1);
		}
		else
			return path;
	}

	std::string toOSNarrowFromUTF16(const std::wstring& osWString) {
		return callAPI<wchar_t, char>(prt::StringUtils::toOSNarrowFromUTF16, osWString);
	}

	std::wstring toUTF16FromOSNarrow(const std::string& osString) {
		return callAPI<char, wchar_t>(prt::StringUtils::toUTF16FromOSNarrow, osString);
	}

	std::wstring toUTF16FromUTF8(const std::string& utf8String) {
		return callAPI<char, wchar_t>(prt::StringUtils::toUTF16FromUTF8, utf8String);
	}

	std::string toUTF8FromUTF16(const std::wstring& utf16String) {
		return callAPI<wchar_t, char>(prt::StringUtils::toUTF8FromUTF16, utf16String);
	}

	std::string toUTF8FromOSNarrow(const std::string& osString) {
		std::wstring utf16String = toUTF16FromOSNarrow(osString);
		return toUTF8FromUTF16(utf16String);
	}

	void appendToRhinoString(ON_wString& rhinoString, const std::wstring& appendee) {
		rhinoString += appendee.c_str();
	}

	std::string percentEncode(const std::string& utf8String) {
		return callAPI<char, char>(prt::StringUtils::percentEncode, utf8String);
	}

	std::string toFileURI(const std::string& osNarrowPath) {
		const std::string utf8Path = toUTF8FromOSNarrow(osNarrowPath);
		const std::string u8PE = percentEncode(utf8Path);
		return FILE_SCHEMA + u8PE;
	}

	std::string toFileURI(const std::wstring& utf16Path) {
		const std::string utf8Path = toUTF8FromUTF16(utf16Path);
		const std::string u8PE = percentEncode(utf8Path);
		return FILE_SCHEMA + u8PE;
	}

	bool isDefaultStyle(const std::wstring& attrName)
	{
		return attrName.find(DEFAULT_STYLE_PREFIX) != std::wstring::npos;
	}

	std::wstring removePrefix(const std::wstring& attrName, wchar_t delim) {
		const auto sepPos = attrName.find(delim);

		if (sepPos == std::wstring::npos) return attrName;
		if (sepPos == attrName.length() - 1) return {};
		if (attrName.length() <= 1) return {};
		return attrName.substr(sepPos + 1);
	}

	std::wstring removeImport(const std::wstring& attrName) {
		return removePrefix(attrName, IMPORT_DELIMITER);
	}

	std::wstring removeStyle(const std::wstring& attrName) {
		return removePrefix(attrName, STYLE_DELIMITER);
	}

	std::wstring getRuleFileEntry(const ResolveMapSPtr& resolveMap) {
		const std::wstring sCGB(L".cgb");

		size_t nKeys;
		wchar_t const* const* keys = resolveMap->getKeys(&nKeys);
		for (size_t k = 0; k < nKeys; ++k) {
			const std::wstring key(keys[k]);
			if (std::equal(sCGB.rbegin(), sCGB.rend(), key.rbegin()))
				return key;
		}

		return {};
	}

	std::wstring detectStartRule(const RuleFileInfoPtr& ruleFileInfo) {
		for (size_t r = 0; r < ruleFileInfo->getNumRules(); r++) {
			const auto* rule = ruleFileInfo->getRule(r);

			// start rules must not have any parameters
			if (rule->getNumParameters() > 0)
				continue;

			for (size_t a = 0; a < rule->getNumAnnotations(); a++) {
				if (std::wcscmp(rule->getAnnotation(a)->getName(), ANNOT_START_RULE) == 0) {
					return rule->getName();
				}
			}
		}
		return {};
	}

	std::wstring toAssetKey(std::wstring key)
	{
		// add the "asset" prefix to the key
		std::wstring assetKey(L"assets/");
		assetKey = assetKey.append(key);
		return assetKey;
	}

	void PathRemover::operator()(std::filesystem::path const* p) {
		if ((p != nullptr) && std::filesystem::exists(*p)) {
			std::error_code errorCode;
			std::filesystem::remove_all(*p, errorCode);
			if (errorCode)
				LOG_ERR << "Unable to remove path at " << *p << ", " << errorCode.message();
			else
				LOG_DBG << "Removed path at " << *p;
			delete p;
		}
	}
}