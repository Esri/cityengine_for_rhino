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

#pragma once

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
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
#include <stdexcept>

struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when
                 // using /permissive-

// forward declaration to break include loop
class GeneratedModel;
class RhinoCallbacks;

namespace pcu {

std::filesystem::path getDllLocation();
std::filesystem::path getTempDir(const std::wstring& tmp_prefix);
std::wstring getUUID();
std::filesystem::path getUniqueTempDir(const std::filesystem::path& tempDir, const std::wstring& basename);

template <typename C>
C getDirSeparator();
template <>
char getDirSeparator();
template <>
wchar_t getDirSeparator();

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
using RuleFileInfoSPtr = std::unique_ptr<const prt::RuleFileInfo, PRTDestroyer>;
using EncoderInfoPtr = std::unique_ptr<const prt::EncoderInfo, PRTDestroyer>;
using DecoderInfoPtr = std::unique_ptr<const prt::DecoderInfo, PRTDestroyer>;
using SimpleOutputCallbacksPtr = std::unique_ptr<prt::SimpleOutputCallbacks, PRTDestroyer>;
using RhinoCallbacksPtr = std::unique_ptr<RhinoCallbacks>;

struct ShapeAttributes {
	std::wstring ruleFile;
	std::wstring startRule;
	std::wstring shapeName;
	RuleFileInfoPtr ruleFileInfo;
	int seed;

	ShapeAttributes(RuleFileInfoPtr& ruleFileInfo, const std::wstring rulef = L"bin/rule.cgb",
	                const std::wstring startRl = L"Default$Lot", const std::wstring shapeN = L"Lot",
	                const int seed = 0);
};

AttributeMapPtr createAttributeMapForShape(const ShapeAttributes& attrs, prt::AttributeMapBuilder& bld);
AttributeMapPtr createValidatedOptions(const wchar_t* encID, const prt::AttributeMap* unvalidatedOptions = nullptr);

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

constexpr const wchar_t IMPORT_DELIMITER = L'.';
constexpr const wchar_t STYLE_DELIMITER = L'$';
constexpr const wchar_t* DEFAULT_STYLE_PREFIX = L"Default$";
constexpr const wchar_t* CE_ARRAY_DELIMITER = L":";

bool isDefaultStyle(const std::wstring& attrName);
std::wstring removePrefix(const std::wstring& attrName, wchar_t delim);
std::wstring removeStyle(const std::wstring& attrName);
std::wstring removeImport(const std::wstring& attrName);
std::wstring getImportPrefix(const std::wstring& attrName);

template <typename C>
std::basic_string<C>& replace_not_in_range(std::basic_string<C>& str, const std::basic_string<C>& range,
                                           const C& replacement) {
	std::replace_if(
	        str.begin(), str.end(), [&range](C c) { return (range.find(c) == std::basic_string<C>::npos); },
	        replacement);
	return str;
}

std::vector<const wchar_t*> split(const std::wstring& i_str, const std::wstring& i_delim) {
	std::vector<const wchar_t*> result;

	size_t found = i_str.find(i_delim);
	size_t startIndex = 0;

	while (found != std::wstring::npos) {
		result.push_back(std::wstring(i_str.begin() + startIndex, i_str.begin() + found).c_str());
		startIndex = found + i_delim.size();
		found = i_str.find(i_delim, startIndex);
	}
	if (startIndex != i_str.size())
		result.push_back(std::wstring(i_str.begin() + startIndex, i_str.end()).c_str());
	return result;
}

std::vector<const wchar_t*> fromCeArray(const std::wstring& stringArray) {
	return pcu::split(stringArray, CE_ARRAY_DELIMITER);
}

/**
 * Interop helpers
 */

template<typename T>
void fillMapBuilder(const std::wstring& key, T value, AttributeMapBuilderPtr& aBuilder) {
	throw std::invalid_argument("Received type is not supported");
}

template<>
void fillMapBuilder<bool>(const std::wstring& key, bool value, AttributeMapBuilderPtr& aBuilder) {
	aBuilder->setBool(key.c_str(), value);
}

template<>
void fillMapBuilder<double>(const std::wstring& key, double value, AttributeMapBuilderPtr& aBuilder) {
	aBuilder->setFloat(key.c_str(), value);
}

template<typename T>
void fillArrayMapBuilder(const std::wstring& key, const std::vector<std::wstring>& values, AttributeMapBuilderPtr& aBuilder) {
	throw std::invalid_argument("Received type is not supported");
}

template<>
void fillArrayMapBuilder<bool>(const std::wstring& key, const std::vector<std::wstring>& values, AttributeMapBuilderPtr& aBuilder) {
	bool* bArray = new bool[values.size()];
	for (int i = 0; i < values.size(); ++i) {
		bArray[i] = values[i] == L"true";
	}
	aBuilder->setBoolArray(key.c_str(), bArray, values.size());
}

template<>
void fillArrayMapBuilder<double>(const std::wstring& key, const std::vector<std::wstring>& values, AttributeMapBuilderPtr& aBuilder) {
	double* dArray = new double[values.size()];
	for (int i = 0; i < values.size(); ++i) {
		dArray[i] = std::stod(values[i]);
	}
	aBuilder->setFloatArray(key.c_str(), dArray, values.size());
}

template<typename T>
void unpackAttributes(int start, int count, ON_ClassArray<ON_wString>* keys, ON_SimpleArray<T>* values, AttributeMapBuilderPtr& aBuilder) {
	for (int i = start; i < start + count; ++i) {
		const std::wstring key(keys->At(i)->Array());

		const T value(*values->At(i));
		pcu::fillMapBuilder(key, value, aBuilder);
	}
}

template<typename T>
void unpackArrayAttributes(int start, int count, ON_ClassArray<ON_wString>* keys, ON_ClassArray<ON_wString>* values,
	AttributeMapBuilderPtr& aBuilder) {
	for (int i = start; i < start + count; ++i) {
		const std::wstring key(keys->At(i)->Array());
		auto vArray = fromCeArray(values->At(i)->Array());
		pcu::fillArrayMapBuilder<T>(key, vArray, aBuilder);
	}
}

void unpackStringAttributes(int start, int count, ON_ClassArray<ON_wString>* keys, ON_ClassArray<ON_wString>* values, AttributeMapBuilderPtr& aBuilder, bool isArray) {
	for (int i = start; i < start + count; ++i) {
		const std::wstring key(keys->At(i)->Array());

		if (isArray) {
			auto strings = fromCeArray(values->At(i)->Array());
			aBuilder->setStringArray(key.c_str(), strings.data(), strings.size());
		} else 
			aBuilder->setString(key.c_str(), values->At(i)->Array());
	}
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