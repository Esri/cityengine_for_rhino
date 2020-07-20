#include "RuleAttributes.h"

#include "Logger.h"


std::wstring getNiceName(const std::wstring& attrName) {
		return pcu::removeImport(pcu::removeStyle(attrName));
}

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
		ruleAttr.mRuleFile = mainCgaRuleName; // TODO: Only support 1 cgb file for now
		ruleAttr.mNickname = getNiceName(ruleAttr.mFullName);
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
