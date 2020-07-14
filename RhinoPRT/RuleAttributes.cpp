#include "RuleAttributes.h"

#include "Logger.h"

RuleAttributes getRuleAttributes(const std::wstring& ruleFile, const prt::RuleFileInfo& ruleFileInfo) {
	RuleAttributes ra;

	std::wstring mainCgaRuleName = pcu::filename(ruleFile);
	size_t idxExtension = mainCgaRuleName.find(L".cgb");
	if (idxExtension != std::wstring::npos)
		mainCgaRuleName = mainCgaRuleName.substr(0, idxExtension);
	
	for (size_t i = 0; i < ruleFileInfo.getNumAttributes(); ++i) {
		const prt::RuleFileInfo::Entry* attr = ruleFileInfo.getAttribute(i);

		RuleAttribute ruleAttr;
		ruleAttr.mFullName = attr->getName();
		ruleAttr.mRuleFile = mainCgaRuleName; // Only support 1 cgb file for now
		ruleAttr.mNickname = attr->getName(); // TODO: implement a function that creates the nickname from the full name
		ruleAttr.mType = attr->getReturnType();

		//TODO: implement prt::Annotation support.

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