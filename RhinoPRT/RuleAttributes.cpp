#include "RuleAttributes.h"

#include "Logger.h"

namespace {

	std::wstring getNiceName(const std::wstring& attrName) {
		return pcu::removeImport(pcu::removeStyle(attrName));
	}

}

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

AnnotationBase* getAnnotationObject(const wchar_t* annotName, const prt::Annotation* an, prt::AnnotationArgumentType attrType) {
	if (!std::wcscmp(annotName, ANNOT_COLOR) && annotCompatibleWithType(A_COLOR, attrType)) {
		return new AnnotationBase(A_COLOR);
	}
	else if (!std::wcscmp(annotName, ANNOT_ENUM)) {
		if(attrType == prt::AAT_BOOL) return new AnnotationEnum<bool>(an);
		if (attrType == prt::AAT_FLOAT) return new AnnotationEnum<double>(an);
		if (attrType == prt::AAT_STR) return new AnnotationEnum<const wchar_t*>(an);
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

RuleAttributes getRuleAttributes(const std::wstring& ruleFile, const prt::RuleFileInfo& ruleFileInfo) {
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

		// prt::Annotation
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
				ruleAttr.mAnnotation = getAnnotationObject(anName, an, attr->getReturnType());
			}
			
		}

		if (hidden) continue;

		ra.push_back(ruleAttr);
		if (DBG) LOG_DBG << ruleAttr;
	}

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
