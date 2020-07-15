#include "utils.h"

#include "wrap.h"

#include "prt/StringUtils.h"

#ifdef _WIN32
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <cwchar>
#include <sstream>
#include <string>

namespace pcu {

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
		bld.setInt(L"seed", attrs.seed);

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

	const ON_Mesh getMeshFromGenModel(const GeneratedModel& model) {
		auto faces = model.getFaces();
		auto vertices = model.getVertices();
		auto indices = model.getIndices();

		size_t nbVertices = vertices.size() / 3;

		ON_Mesh mesh(faces.size(), nbVertices, false, false);

		for (size_t v_id = 0; v_id < nbVertices; ++v_id) {
			mesh.SetVertex(v_id, ON_3dPoint(vertices[v_id * 3], vertices[v_id * 3 + 1], vertices[v_id * 3 + 2]));
		}

		int faceid(0);
		int currindex(0);
		for (int face : faces) {
			if (face == 3) {
				mesh.SetTriangle(faceid, indices[currindex], indices[currindex + 1], indices[currindex + 2]);
				currindex += face;
			}
			else if (face == 4) {
				mesh.SetQuad(faceid, indices[currindex], indices[currindex + 1], indices[currindex + 2], indices[currindex + 3]);
				currindex += face;
			}
			else {
				//ignore face because it is invalid
				currindex += face;
			}
			faceid++;
		}

		// Printing an error log if the created mesh is invalid
		FILE* fp = ON::OpenFile(L"C:\\Windows\\Temp\\rhino_log_2.txt", L"w");
		if (fp) {
			ON_TextLog log(fp);
			if (!mesh.IsValid(&log))
				mesh.Dump(log);
			ON::CloseFile(fp);
		}

		mesh.ComputeVertexNormals();
		mesh.Compact();

		return mesh;
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

	std::string toUTF8FromOSNarrow(const std::string& osString) {
		std::wstring utf16String = toUTF16FromOSNarrow(osString);
		return callAPI<wchar_t, char>(prt::StringUtils::toUTF8FromUTF16, utf16String);
	}

	std::string percentEncode(const std::string& utf8String) {
		return callAPI<char, char>(prt::StringUtils::percentEncode, utf8String);
	}

	URI toFileURI(const std::string& p)
	{
		const std::string utf8Path = toUTF8FromOSNarrow(p);
		const std::string u8PE = percentEncode(utf8Path);
		return FILE_SCHEMA + u8PE;
	}

	URI toFileURI(const std::wstring& p) {
		return toFileURI(toOSNarrowFromUTF16(p));
	}

	std::wstring getRuleFileEntry(const ResolveMapPtr& resolveMap) {
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

}