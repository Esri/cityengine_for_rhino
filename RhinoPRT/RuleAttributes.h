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

//typedef struct _RuleAttribute RuleAttribute;

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

constexpr const wchar_t* MIN_KEY = L"min";
constexpr const wchar_t* MAX_KEY = L"max";

using AttributeGroup = std::vector<std::wstring>;
using AttributeGroupOrder = std::map<AttributeGroup, int>;
	
enum AttributeAnnotation { A_COLOR = 0, A_RANGE, A_ENUM, A_FILE, A_DIR, A_NOANNOT };


class AnnotationBase {
public:
	AnnotationBase(AttributeAnnotation annot): mAnnotType(annot) {}

	virtual ~AnnotationBase() {}

	virtual ON_SimpleArray<int> getAnnotArguments() { return ON_SimpleArray<int>(mAnnotType); };

protected:
	AttributeAnnotation mAnnotType;
};

using AnnotationPtr = std::unique_ptr<AnnotationBase>;

class AnnotationRange : public AnnotationBase {
public:
	AnnotationRange(const prt::Annotation* an): AnnotationBase(A_RANGE) {
		for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
			const prt::AnnotationArgument* arg = an->getArgument(argIdx);
			const wchar_t* key = arg->getKey();
			if (std::wcscmp(key, MIN_KEY) == 0) {
				mMin = arg->getFloat();
			}
			else if (std::wcscmp(key, MAX_KEY) == 0) {
				mMax = arg->getFloat();
			}
		}
	}

private:
	double mMin;
	double mMax;
};

/// Generic enum annotation class
/// Enum annotations can be strings, numbers or bools
template<class T>
class AnnotationEnum : public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_NOANNOT) {
		LOG_WRN << L"Rule type incompatible with enum.";
	}
};

template<>
class AnnotationEnum<bool>: public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM) {
		for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
			mEnums.Append(an->getArgument(argIdx)->getBool());
		}
	}

private:
	ON_SimpleArray<bool> mEnums;
};

template<>
class AnnotationEnum<const wchar_t*> : public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM) {
		for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
			mEnums.Append(an->getArgument(argIdx)->getStr());
		}
	}

private:
	ON_SimpleArray<const wchar_t*> mEnums;
};

template<>
class AnnotationEnum<double> : public AnnotationBase {
public:
	AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM) {
		for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
			mEnums.Append(an->getArgument(argIdx)->getFloat());
		}
	}

private:
	ON_SimpleArray<double> mEnums;
};


class AnnotationFile : public AnnotationBase {
public:
	AnnotationFile(const prt::Annotation* an) : AnnotationBase(A_FILE) {}
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

	AnnotationBase * mAnnotation;
};

using RuleAttributes = std::vector<RuleAttribute>;

RuleAttributes getRuleAttributes(const std::wstring& ruleFile,
								 const prt::RuleFileInfo& ruleFileInfo);

std::wostream& operator<<(std::wostream& ostr, const RuleAttribute& ap);
std::ostream& operator<<(std::ostream& ostr, const RuleAttribute& ap);