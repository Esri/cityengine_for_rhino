#include "utils.h"

#include "prt/StringUtils.h"

#ifdef _WIN32
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

namespace pcu {

#ifdef _WIN32
	const std::string FILE_SCHEMA = "file:/";
#else
	const std::string FILE_SCHEMA = "file:";
#endif

	/**
	 * Helper function to create a prt::AttributeMap from a ShapeAttributes structure.
	 */
	AttributeMapPtr createAttributeMapForShape(const ShapeAttributes& attrs, prt::AttributeMapBuilder& bld) {
		bld.setString(L"ruleFile", attrs.ruleFile.c_str());
		bld.setString(L"startRule", attrs.startRule.c_str());
		bld.setString(L"shapeName", attrs.shapeName.c_str());
		bld.setInt(L"seed", attrs.seed);
		bld.setFloat(L"height", attrs.height);

		return AttributeMapPtr{ bld.createAttributeMap() };
	}

	AttributeMapPtr createAttributeMapForEncoder(const EncoderOptions& encOpts, prt::AttributeMapBuilder& bld) {
		bld.setBool(L"emitReport", encOpts.emitReport);
		bld.setBool(L"emitGeometry", encOpts.emitGeometry);

		return AttributeMapPtr{ bld.createAttributeMap() };
	}

	AttributeMapPtr createValidatedOptions(const std::wstring & encID, const AttributeMapPtr & unvalidatedOptions) {
		const EncoderInfoPtr encInfo{ prt::createEncoderInfo(encID.c_str()) };

		if (!encInfo) {
			std::cout << "Failed to create encoder info: encoder not found for given ID." << std::endl;
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

	std::string toOSNarrowFromUTF16(const std::wstring& osWString) {
		return callAPI<wchar_t, char>(prt::StringUtils::toOSNarrowFromUTF16, osWString);
	}

	std::wstring toUTF16FromOSNarrow(const std::string& osString) {
		return callAPI<char, wchar_t>(prt::StringUtils::toUTF16FromOSNarrow, osString);
	}

	std::wstring toUTF16FromUTF8(const std::string& utf8String) {
		return callAPI<char, wchar_t>(prt::StringUtils::toUTF16FromUTF8, utf8String);
	}

	std::string toUTF8FromOSNarrow(const std::string& osString) {
		std::wstring utf16String = toUTF16FromOSNarrow(osString);
		return callAPI<wchar_t, char>(prt::StringUtils::toUTF8FromUTF16, utf16String);
	}

	std::string percentEncode(const std::string& utf8String) {
		return callAPI<char, char>(prt::StringUtils::percentEncode, utf8String);
	}

	URI toFileURI(const std::string & p)
	{
		const std::string utf8Path = toUTF8FromOSNarrow(p);
		const std::string u8PE = percentEncode(utf8Path);
		return FILE_SCHEMA + u8PE;
	}

}