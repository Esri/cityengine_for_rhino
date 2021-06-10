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

struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when
                 // using /permissive-

#include "prt/Annotation.h"
#include "prt/RuleFileInfo.h"

#include "Logger.h"
#include "utils.h"

#include <limits>
#include <map>
#include <string>
#include <vector>

#include <combaseapi.h>
#include <objbase.h>
#include <strsafe.h>

#pragma comment(lib, "ole32.lib") // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was
                                  // unexpected here" when using /permissive-

constexpr const wchar_t* ANNOT_RANGE = L"@Range";
constexpr const wchar_t* ANNOT_ENUM = L"@Enum";
constexpr const wchar_t* ANNOT_HIDDEN = L"@Hidden";
constexpr const wchar_t* ANNOT_COLOR = L"@Color";
constexpr const wchar_t* ANNOT_DIR = L"@Directory";
constexpr const wchar_t* ANNOT_FILE = L"@File";
constexpr const wchar_t* ANNOT_ORDER = L"@Order";
constexpr const wchar_t* ANNOT_GROUP = L"@Group";

constexpr int ORDER_FIRST = std::numeric_limits<int>::lowest();
constexpr int ORDER_NONE = std::numeric_limits<int>::infinity();

/// Range Annotation Keys
constexpr const wchar_t* MIN_KEY = L"min";
constexpr const wchar_t* MAX_KEY = L"max";
constexpr const wchar_t* STEP_KEY = L"stepsize";
constexpr const wchar_t* RESTRICTED_KEY = L"restricted";

using AttributeGroup = std::vector<std::wstring>;
using AttributeGroupOrder = std::map<AttributeGroup, int>;

enum class AttributeAnnotation { COLOR = 0, RANGE, ENUM, FILE, DIR, NOANNOT };
enum class EnumAnnotationType { DOUBLE = 0, BOOL, STRING, INVALID };

struct RangeAttributes {
	double mMin;
	double mMax;
	double mStepSize;
	bool mRestricted;
};

class AnnotationBase {
public:
	AnnotationBase(AttributeAnnotation annot, EnumAnnotationType enumType = EnumAnnotationType::INVALID)
	    : mAnnotType(annot), mEnumType(enumType) {}
	AnnotationBase(const AnnotationBase&) = delete;

	virtual ~AnnotationBase() {}

	const AttributeAnnotation getType() const {
		return mAnnotType;
	}

	const EnumAnnotationType getEnumType() const {
		return mEnumType;
	}

protected:
	AttributeAnnotation mAnnotType;
	EnumAnnotationType mEnumType;
};

class AnnotationRange : public AnnotationBase {
public:
	AnnotationRange(const prt::Annotation* an);

	RangeAttributes getAnnotArguments() const;

private:
	double mMin;
	double mMax;
	double mStepSize;
	bool mRestricted;
};

/// Generic enum annotation class
/// Enum annotations can be strings, numbers or bools
template <class T>
class AnnotationEnum : public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* /*an*/) : AnnotationBase(AttributeAnnotation::NOANNOT) {
		LOG_WRN << L"Rule type incompatible with enum.";
	}

	std::vector<T> getAnnotArguments() const {
		return mEnums;
	}
	bool isRestricted() const {
		return mRestricted;
	}

private:
	bool mRestricted;
	std::vector<T> mEnums;
};

class AnnotationFile : public AnnotationBase {
public:
	AnnotationFile(const prt::Annotation* /*an*/) : AnnotationBase(AttributeAnnotation::FILE) {}
};

class AnnotationDir : public AnnotationBase {
public:
	AnnotationDir(const prt::Annotation* /*an*/) : AnnotationBase(AttributeAnnotation::DIR) {}
};

using AnnotationUPtr = std::unique_ptr<AnnotationBase>;

void addAnnotationObject(const wchar_t* annotName, const prt::Annotation* an, prt::AnnotationArgumentType attrType,
                         std::vector<AnnotationUPtr>& annotVector);

struct RuleAttribute {
	std::wstring mRuleFile; /// The cga file in which this rule attribute is defined.
	std::wstring mFullName; /// The attribute full name given to grasshopper.
	std::wstring mNickname; /// The attribute short name displayed in the gh node.
	prt::AnnotationArgumentType mType;

	AttributeGroup groups;
	int order = ORDER_NONE;
	int groupOrder = ORDER_NONE;

	std::vector<AnnotationUPtr> mAnnotations;
};

using RuleAttributeUPtr = std::unique_ptr<RuleAttribute>;
using RuleAttributes = std::vector<RuleAttributeUPtr>;

void createRuleAttributes(const std::wstring& ruleFile, const prt::RuleFileInfo& ruleFileInfo, RuleAttributes& ra);

std::wostream& operator<<(std::wostream& ostr, const RuleAttribute& ap);
std::ostream& operator<<(std::ostream& ostr, const RuleAttribute& ap);