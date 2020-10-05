#pragma once

struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "prt/Annotation.h"
#include "prt/RuleFileInfo.h"

#include "utils.h"
#include "Logger.h"

#include <limits>
#include <map>
#include <string>
#include <vector>

#include <objbase.h>
#include <combaseapi.h>
#include <strsafe.h>

#pragma comment(lib, "ole32.lib") // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

constexpr auto DBG = true;

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

enum AttributeAnnotation { A_COLOR = 0, A_RANGE, A_ENUM, A_FILE, A_DIR, A_NOANNOT };
enum EnumAnnotationType { ENUM_DOUBLE = 0, ENUM_BOOL, ENUM_STRING, ENUM_INVALID };


struct RangeAttributes {
	double mMin;
	double mMax;
	double mStepSize;
	bool mRestricted;
};

class AnnotationBase {
public:
	AnnotationBase(AttributeAnnotation annot, EnumAnnotationType enumType = ENUM_INVALID): mAnnotType(annot), mEnumType(enumType) {}

	virtual ~AnnotationBase() {}

	virtual const AttributeAnnotation getType() const { return mAnnotType; }

	virtual const EnumAnnotationType getEnumType() const { return mEnumType; }

protected:
	AttributeAnnotation mAnnotType;
	EnumAnnotationType mEnumType;
};

using AnnotationPtr = std::unique_ptr<AnnotationBase>;

class AnnotationRange : public AnnotationBase {
public:
	AnnotationRange(const prt::Annotation* an): AnnotationBase(A_RANGE), mStepSize(0) {

		for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
			const prt::AnnotationArgument* arg = an->getArgument(argIdx);
			const wchar_t* key = arg->getKey();
			if (std::wcscmp(key, MIN_KEY) == 0) {
				mMin = arg->getFloat();
			}
			else if (std::wcscmp(key, MAX_KEY) == 0) {
				mMax = arg->getFloat();
			}
			else if (std::wcscmp(key, STEP_KEY) == 0) {
				mStepSize = arg->getFloat();
			}
			else if (std::wcscmp(key, RESTRICTED_KEY) == 0) {
				mRestricted = arg->getBool();
			}
		}
	}

	RangeAttributes getAnnotArguments() const { return { mMin, mMax, mStepSize, mRestricted }; }

private:
	double mMin;
	double mMax;
	double mStepSize;
	bool mRestricted;
};

/// Generic enum annotation class
/// Enum annotations can be strings, numbers or bools
template<class T>
class AnnotationEnum : public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_NOANNOT, ENUM_INVALID) {
		LOG_WRN << L"Rule type incompatible with enum.";
	}

	const EnumAnnotationType getEnumType() const { return mEnumType; }

};

/// Specific implementations of enum annotation class

template<>
class AnnotationEnum<bool>: public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM, ENUM_BOOL)
	{
		for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
			mEnums.Append(an->getArgument(argIdx)->getBool());
		}
	}

private:
	ON_SimpleArray<bool> mEnums;
};

template<>
class AnnotationEnum<std::wstring> : public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM, ENUM_STRING)
	{
		for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
			mEnums.push_back(std::wstring(an->getArgument(argIdx)->getStr()));
		}
	}

	std::vector<std::wstring>& getAnnotArguments() { return mEnums; }

private:
	std::vector<std::wstring> mEnums;
};

template<>
class AnnotationEnum<double> : public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM, ENUM_DOUBLE) {
		for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
			mEnums.push_back(an->getArgument(argIdx)->getFloat());
		}
	}

	std::vector<double>& getAnnotArguments() { return mEnums; }

private:
	std::vector<double> mEnums;
};


class AnnotationFile : public AnnotationBase {
public:
	AnnotationFile(const prt::Annotation* an) : AnnotationBase(A_FILE) {}
};

class AnnotationDir : public AnnotationBase {
public:
	AnnotationDir(const prt::Annotation* an) : AnnotationBase(A_DIR) {}
};


AnnotationBase* getAnnotationObject(const wchar_t* annotName, const prt::Annotation* an, prt::AnnotationArgumentType attrType);

struct RuleAttribute {
	std::wstring mRuleFile;	/// The cga file in which this rule attribute is defined.
	std::wstring mFullName;	/// The attribute full name given to grasshopper.
	std::wstring mNickname;	/// The attribute short name displayed in the gh node.
	prt::AnnotationArgumentType mType;

	AttributeGroup groups;
	int order = ORDER_NONE;
	int groupOrder = ORDER_NONE;

	std::vector<AnnotationBase*> mAnnotations;
};

using RuleAttributes = std::vector<RuleAttribute>;

RuleAttributes getRuleAttributes(const std::wstring& ruleFile,
								 const prt::RuleFileInfo& ruleFileInfo);

std::wostream& operator<<(std::wostream& ostr, const RuleAttribute& ap);
std::ostream& operator<<(std::ostream& ostr, const RuleAttribute& ap);