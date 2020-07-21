#pragma once

struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "prt/Annotation.h"
#include "prt/RuleFileInfo.h"

#include "utils.h"
#include "Logger.h"

#include <string>
#include <vector>

#include <objbase.h>
#include <combaseapi.h>
#include <strsafe.h>

#pragma comment(lib, "ole32.lib") // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

constexpr auto DBG = true;

typedef struct _RuleAttribute RuleAttribute;

struct _RuleAttribute {
	std::wstring mRuleFile;	/// The cga file in which this rule attribute is defined.
	std::wstring mFullName;	/// The attribute full name given to grasshopper.
	std::wstring mNickname;	/// The attribute short name displayed in the gh node.
	prt::AnnotationArgumentType mType;
};

using RuleAttributes = std::vector<RuleAttribute>;

RuleAttributes getRuleAttributes(const std::wstring& ruleFile,
								 const prt::RuleFileInfo& ruleFileInfo);

std::wostream& operator<<(std::wostream& ostr, const RuleAttribute& ap);
std::ostream& operator<<(std::ostream& ostr, const RuleAttribute& ap);