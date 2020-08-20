#include "ReportAttribute.h"

#include "RhinoCallbacks.h"

#include <algorithm>

namespace {

	void syncReports(const int shapeID, std::vector<Reporting::ReportAttribute>& reports)
	{
		int currID = reports.size();

		while (currID < shapeID) {
			reports.push_back(Reporting::getEmptyReport(currID));
			currID++;
		}
	}

	void addToMap(Reporting::GroupedReportMap& reports, const Reporting::ReportAttribute& report, const int shapeID)
	{
		auto it = reports.find(report.mReportName);
		if (it == reports.end()) {
			std::vector<Reporting::ReportAttribute> newVect;
			syncReports(shapeID, newVect);
			newVect.push_back(report);

			reports.insert(it, std::make_pair(report.mReportName, newVect));
		}
		else {
			syncReports(shapeID, reports.at(report.mReportName));
			reports.at(report.mReportName).push_back(report);
		}
	}

}

namespace Reporting {

	ReportAttribute getEmptyReport(int shapeID)
	{
		ReportAttribute report;
		report.mInitialShapeIndex = shapeID;
		report.mReportName = L"Empty Report";
		report.mType = prt::AttributeMap::PrimitiveType::PT_UNDEFINED;

		return report;
	}

	void extractReports(int initShapeId, Model& model, const prtx::PRTUtils::AttributeMapPtr reports)
	{
		size_t keyCount = 0;
		auto keys = reports->getKeys(&keyCount);

		for (size_t i = 0; i < keyCount; ++i) {
			auto key = keys[i];

			ReportAttribute ra;
			ra.mInitialShapeIndex = initShapeId;
			ra.mReportName.assign(key);
			ra.mType = reports->getType(key);

			switch (ra.mType) {
			case prt::AttributeMap::PrimitiveType::PT_BOOL:
				ra.mBoolReport = reports->getBool(key);
				break;
			case prt::AttributeMap::PrimitiveType::PT_STRING:
				ra.mStringReport.assign(reports->getString(key));
				break;
			case prt::AttributeMap::PrimitiveType::PT_FLOAT:
				ra.mDoubleReport = reports->getFloat(key);
				break;
			case prt::AttributeMap::PrimitiveType::PT_INT:
				ra.mIntReport = reports->getInt(key);
				break;
			default:
				LOG_ERR << L"Type of report not supported.";
				return;
			}

			model.mReports.emplace(ra.mReportName, std::move(ra));
		}
	}

	const ReportsVector ToReportsVector(const ReportMap& reports)
	{
		ReportsVector rep;
		rep.insert(rep.end(), reports.begin(), reports.end());
		return rep;
	}

	void GroupedReports::add(const ReportAttribute& report, const int shapeID)
	{
		if (report.mType == prt::AttributeMap::PrimitiveType::PT_BOOL) {
			addToMap(mGroupedBoolReports, report, shapeID);
		}
		else if (report.mType == prt::AttributeMap::PrimitiveType::PT_FLOAT) {
			addToMap(mGroupedDoubleReports, report, shapeID);
		}
		else if (report.mType == prt::AttributeMap::PrimitiveType::PT_STRING) {
			addToMap(mGroupedStringReports, report, shapeID);
		}
	}

	size_t GroupedReports::getReportCount() const
	{
		return mGroupedBoolReports.size() + mGroupedDoubleReports.size() + mGroupedStringReports.size();
	}

	bool GroupedReports::getReportKeys(ON_ClassArray<ON_wString>* pKeysArray, ON_SimpleArray<int>* pKeyTypeArray) const
	{
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

	const std::vector<ReportAttribute>& GroupedReports::getDoubleReports(std::wstring key) const
	{
		try {
			return mGroupedDoubleReports.at(key);
		}
		catch (const std::out_of_range) {
			return EMPTY_REPORTS;
		}

	}

	const std::vector<ReportAttribute>& GroupedReports::getBoolReports(std::wstring key) const
	{
		try {
			return mGroupedBoolReports.at(key);
		}
		catch (const std::out_of_range) {
			return EMPTY_REPORTS;
		}
	}

	const std::vector<ReportAttribute>& GroupedReports::getStringReports(std::wstring key) const
	{
		try {
			return mGroupedStringReports.at(key);
		}
		catch (const std::out_of_range) {
			return EMPTY_REPORTS;
		}
	}

	void GroupedReports::clear()
	{
		mGroupedBoolReports.clear();
		mGroupedStringReports.clear();
		mGroupedDoubleReports.clear();
	}

} // namepsace Reporting