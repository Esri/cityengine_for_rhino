#pragma once

#include "utils.h"

#include "prt/AttributeMap.h"
#include "prtx/PRTUtils.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <map>

class Model;

namespace Reporting {

	struct ReportAttribute {
		int mInitialShapeIndex;
		std::wstring mReportName;
		prt::AttributeMap::PrimitiveType mType;

		std::wstring mStringReport;
		double mDoubleReport = 0;
		bool mBoolReport = false;
		int mIntReport = 0;

		ReportAttribute() = default;
	};

	using ReportsVector = std::vector<ReportAttribute>;
	using ReportMap = std::map<std::wstring, ReportAttribute>;
	using GroupedReportMap = std::map<std::wstring, std::vector<ReportAttribute>>;

	const std::vector<ReportAttribute> EMPTY_REPORTS;
	const std::wstring EMPTY_REPORT_STRING(L"\0");

	ReportAttribute getEmptyReport(int initialShapeIndex);

	void extractReports(int initShapeId, Model& model, const prtx::PRTUtils::AttributeMapPtr reports);
	ReportsVector ToReportsVector(const ReportMap& reports);

	class GroupedReports {
	public:

		void add(const ReportAttribute& report, const int initialShapeIndex);
		size_t getReportCount() const;
		bool getReportKeys(ON_ClassArray<ON_wString>* pKeysArray, ON_SimpleArray<int>* pKeyTypeArray) const;
		const std::vector<ReportAttribute>& getDoubleReports(std::wstring key) const;
		const std::vector<ReportAttribute>& getBoolReports(std::wstring key) const;
		const std::vector<ReportAttribute>& getStringReports(std::wstring key) const;
		void clear();

	private:
		GroupedReportMap mGroupedStringReports;
		GroupedReportMap mGroupedBoolReports;
		GroupedReportMap mGroupedDoubleReports;
	};

} //namespace reporting

