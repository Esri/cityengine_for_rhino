#include "RhinoPRT.h"

namespace RhinoPRT {

	template<typename T>
	void RhinoPRTAPI::fillAttributeFromNode(const std::wstring& ruleName, const std::wstring& attrFullName, T value) {

		// Find the RuleAttribute object corresponding to the given attribute name.
		auto reverseLookupAttribute = [this](const std::wstring gh_attrFullName) {
			auto it = std::find_if(mRuleAttributes.begin(), mRuleAttributes.end(), [&gh_attrFullName](const auto& ra) {return ra.mFullName == gh_attrFullName; });
			if (it != mRuleAttributes.end()) return *it;
			return RULE_NOT_FOUND;
		};

		const RuleAttribute rule = reverseLookupAttribute(attrFullName);
		assert(!rule.mFullName.empty()); // Check if the rule was found

		// If the attribute is found, register the value in the attribute map builder
		//TODO: check for difference with default value, only add the attribute if it is the case.
		setRuleAttributeValue(rule, value);
	}

	template<typename T>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, T value) {
		LOG_WRN << L"Ignored unsupported attribute '" << rule.mFullName << "' type: " << T << " ---- " << rule.mType << " of rule file: '" << rule.mRuleFile << "'" << std::endl;
	}

	// Specific instantiations of setRuleAttributeValue.

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, double value) {
		if (rule.mType == prt::AAT_FLOAT) {
			mAttrBuilder->setFloat(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set a double value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, int value) {
		if (rule.mType == prt::AAT_INT) {
			mAttrBuilder->setInt(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set an int value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, bool value) {
		if (rule.mType == prt::AAT_BOOL) {
			mAttrBuilder->setBool(rule.mFullName.c_str(), value);
		}
		else {
			LOG_ERR << L"Trying to set a boolean value to an attribute of type " << rule.mType << std::endl;
		}
	}

	template<>
	void RhinoPRTAPI::setRuleAttributeValue(const RuleAttribute& rule, std::wstring value) {
		if (rule.mType == prt::AAT_STR) {
			mAttrBuilder->setString(rule.mFullName.c_str(), value.c_str());
		}
		else {
			LOG_ERR << L"Trying to set a wstring to an attribute of type " << rule.mType << std::endl;
		}
	}
}