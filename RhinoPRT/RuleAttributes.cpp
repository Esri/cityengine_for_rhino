#include "RuleAttributes.h"

#include "Logger.h"

#include <algorithm>

namespace {

	std::wstring getNiceName(const std::wstring& attrName) {
		return pcu::removeImport(pcu::removeStyle(attrName));
	}

}

AnnotationRange::AnnotationRange(const prt::Annotation* an) : AnnotationBase(A_RANGE), mStepSize(0) {

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

RangeAttributes AnnotationRange::getAnnotArguments() const { return { mMin, mMax, mStepSize, mRestricted }; }

/// Specific implementations of enum annotation constructors

template<>
AnnotationEnum<bool>::AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM, ENUM_BOOL)
{
	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		mEnums.push_back(an->getArgument(argIdx)->getBool());
	}
}

template<>
AnnotationEnum<std::wstring>::AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM, ENUM_STRING)
{
	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		mEnums.push_back(std::wstring(an->getArgument(argIdx)->getStr()));
	}
}

template<>
AnnotationEnum<double>::AnnotationEnum(const prt::Annotation* an) : AnnotationBase(A_ENUM, ENUM_DOUBLE) {
	for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
		mEnums.push_back(an->getArgument(argIdx)->getFloat());
	}
}

/// =========================================================== ///

bool annotCompatibleWithType(AttributeAnnotation annot, prt::AnnotationArgumentType type) {
	switch (type) {
	case prt::AnnotationArgumentType::AAT_BOOL:
		return annot == AttributeAnnotation::A_ENUM;
	case prt::AnnotationArgumentType::AAT_INT:
	case prt::AnnotationArgumentType::AAT_FLOAT:
		return annot == A_ENUM || annot == A_RANGE;
	case prt::AnnotationArgumentType::AAT_STR:
		return annot != A_RANGE;
	default:
		return false;
	}
}

AnnotationBase* getAnnotationObject(const wchar_t* annotName, const prt::Annotation* an, prt::AnnotationArgumentType attrType)
{
	if (!std::wcscmp(annotName, ANNOT_COLOR) && annotCompatibleWithType(A_COLOR, attrType)) {
		return new AnnotationBase(A_COLOR);
	}
	else if (!std::wcscmp(annotName, ANNOT_ENUM)) {
		if(attrType == prt::AAT_BOOL) return new AnnotationEnum<bool>(an);
		if (attrType == prt::AAT_FLOAT) return new AnnotationEnum<double>(an);
		if (attrType == prt::AAT_STR) return new AnnotationEnum<std::wstring>(an);
		if (attrType == prt::AAT_INT) return new AnnotationEnum<int>(an);
		return new AnnotationBase(A_NOANNOT);
	}
	else if (!std::wcscmp(annotName, ANNOT_RANGE) && annotCompatibleWithType(A_RANGE, attrType)) {
		return new AnnotationRange(an);
	}
	else if (!std::wcscmp(annotName, ANNOT_DIR) && annotCompatibleWithType(A_DIR, attrType)) {
		return new AnnotationBase(A_DIR);
	}
	else if(!std::wcscmp(annotName, ANNOT_FILE) && annotCompatibleWithType(A_FILE, attrType)) {
		return new AnnotationFile(an);
	}
	else {
		return new AnnotationBase(A_NOANNOT);
	}
}

RuleAttributes getRuleAttributes(const std::wstring& ruleFile, const prt::RuleFileInfo& ruleFileInfo)
{
	RuleAttributes ra;

	std::wstring mainCgaRuleName = pcu::filename(ruleFile);
	size_t idxExtension = mainCgaRuleName.find(L".cgb");
	if (idxExtension != std::wstring::npos)
		mainCgaRuleName = mainCgaRuleName.substr(0, idxExtension);
	
	for (size_t i = 0; i < ruleFileInfo.getNumAttributes(); ++i) {
		const prt::RuleFileInfo::Entry* attr = ruleFileInfo.getAttribute(i);

		if (attr->getNumParameters() != 0) continue;

		RuleAttribute ruleAttr;
		ruleAttr.mFullName = attr->getName();
		ruleAttr.mRuleFile = mainCgaRuleName;
		ruleAttr.mNickname = getNiceName(ruleAttr.mFullName);
		ruleAttr.mType = attr->getReturnType();

		// process prt::Annotation
		bool hidden = false;

		for (size_t a = 0; a < attr->getNumAnnotations(); ++a) {
			const prt::Annotation* an = attr->getAnnotation(a);
			const wchar_t* anName = an->getName();

			if (!(std::wcscmp(anName, ANNOT_HIDDEN))) {
				hidden = true;
			}
			else if (!std::wcscmp(anName, ANNOT_ORDER)) {
				if (an->getNumArguments() >= 1 && an->getArgument(0)->getType() == prt::AAT_FLOAT) {
					ruleAttr.order = static_cast<int>(an->getArgument(0)->getFloat());
				}
			}
			else if (!std::wcscmp(anName, ANNOT_GROUP)) {
				for (int argIdx = 0; argIdx < an->getNumArguments(); ++argIdx) {
					if (an->getArgument(argIdx)->getType() == prt::AAT_STR) {
						ruleAttr.groups.push_back(an->getArgument(argIdx)->getStr());
					}
					else if (argIdx == an->getNumArguments() - 1 &&
						an->getArgument(argIdx)->getType() == prt::AAT_FLOAT) {
						ruleAttr.groupOrder = static_cast<int>(an->getArgument(argIdx)->getFloat());
					}
				}
			}
			else {
				ruleAttr.mAnnotations.push_back(getAnnotationObject(anName, an, attr->getReturnType()));
			}
		}

		if (hidden) continue;

		ra.push_back(ruleAttr);
		if (DBG) LOG_DBG << ruleAttr;
	}

	// Group and order attributes.
	// Set a global order:
	// - First sort by group / group order
	// - Then by order in group
	std::sort(ra.begin(), ra.end(), [](const RuleAttribute& left, const RuleAttribute& right){
		if (left.groups.size() == 0) {
			if (right.groups.size() == 0)
			{
				// No groups for both attributes: sort by order if they are set, else sort by string compare.
				if(left.order != right.order) return left.order < right.order;
				return left.mNickname.compare(right.mNickname) < 0;
			}
			else {
				return true; // Attributes without group are placed first.
			}
		}
		else if (right.groups.size() == 0) {
			return false; // Attributes without group are placed first.
		}

		// support only first level groups for now.
		int group_cmpr = left.groups.front().compare(right.groups.front());
		if (group_cmpr == 0) 
		{
			// same group, sort by order if set.
			if (left.order != right.order) return left.order < right.order;
			return left.mNickname.compare(right.mNickname) < 0;
		}
		else {
			// different group, sort by groupOrder if they are set, else use string compare.
			if (left.groupOrder != right.groupOrder) return left.groupOrder < right.groupOrder;
			return left.groups.front().compare(right.groups.front()) < 0;
		}
	});

	return ra;
}

std::wostream& operator<<(std::wostream& ostr, const RuleAttribute& ap) {
	ostr << L"RuleAttribute '" << ap.mFullName << L"': ruleFile = '" << ap.mRuleFile << L"'" << std::endl;
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const RuleAttribute& ap) {
	std::wostringstream wostr;
	wostr << ap;
	ostr << pcu::toOSNarrowFromUTF16(wostr.str());
	return ostr;
}
