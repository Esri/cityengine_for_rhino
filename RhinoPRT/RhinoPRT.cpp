#include "RhinoPRT.h"

namespace {

	void addToMap(RhinoPRT::GroupedReportMap& reports, const std::pair<const std::wstring, Reporting::ReportAttribute> report) 
	{
		auto it = reports.find(report.first);
		if (it == reports.end()) {
			std::vector<Reporting::ReportAttribute> newVect{ report.second };
			reports.insert(it, std::make_pair(report.first, newVect));
		}
		else {
			reports.at(report.first).push_back(report.second);
		}
	}

}

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

	///<summary>
	///The reports of all generated models will be grouped together by keys. 
	///Meaning that all reports with the same key will be added to a vector.
	///</summary>
	///<return>The total number of reports.</return>
	int RhinoPRTAPI::groupReportsByKeys() {
		if (mGeneratedModels.empty()) return 0;

		mGroupedStringReports.clear();
		mGroupedBoolReports.clear();
		mGroupedDoubleReports.clear();

		for (const auto& model : mGeneratedModels) {
			for (const auto& report : model.getReport()) {
				if (report.second.mType == prt::AttributeMap::PrimitiveType::PT_BOOL) {
					addToMap(mGroupedBoolReports, report);
				}
				else if (report.second.mType == prt::AttributeMap::PrimitiveType::PT_FLOAT) {
					addToMap(mGroupedDoubleReports, report);
				}
				else if (report.second.mType == prt::AttributeMap::PrimitiveType::PT_STRING) {
					addToMap(mGroupedStringReports, report);
				}
			}
		}

		return mGroupedStringReports.size() + mGroupedBoolReports.size() + mGroupedDoubleReports.size();
	}

	bool RhinoPRTAPI::getReportKeys(ON_ClassArray<ON_wString>* pKeysArray, ON_SimpleArray<int>* pKeyTypeArray) {
		auto getReportInfosFunc = [&pKeysArray, &pKeyTypeArray](auto& it) {
			pKeysArray->Append(ON_wString(it.first.c_str()));
			pKeyTypeArray->Append(it.second.front().mType);
		};

		if (!mGroupedDoubleReports.empty()) {
			std::for_each(mGroupedDoubleReports.begin(), mGroupedDoubleReports.end(), getReportInfosFunc);
		}
		if (!mGroupedStringReports.empty()) {
			std::for_each(mGroupedStringReports.begin(), mGroupedStringReports.end(), getReportInfosFunc);
		}
		if (!mGroupedBoolReports.empty()) {
			std::for_each(mGroupedBoolReports.begin(), mGroupedBoolReports.end(), getReportInfosFunc);
		}

		return true;
	}

	std::vector<Reporting::ReportAttribute> RhinoPRTAPI::getDoubleReports(std::wstring key) {
		try {
			return mGroupedDoubleReports.at(key);
		}
		catch (const std::out_of_range& oor) {
			return std::vector<Reporting::ReportAttribute>();
		}
		
	}

	std::vector<Reporting::ReportAttribute> RhinoPRTAPI::getBoolReports(std::wstring key) {
		try {
			return mGroupedBoolReports.at(key);
		}
		catch (const std::out_of_range& oor) {
			return std::vector<Reporting::ReportAttribute>();
		}
	}

	std::vector<Reporting::ReportAttribute> RhinoPRTAPI::getStringReports(std::wstring key) {
		try {
			return mGroupedStringReports.at(key);
		}
		catch (const std::out_of_range& oor) {
			return std::vector<Reporting::ReportAttribute>();
		}
	}
}