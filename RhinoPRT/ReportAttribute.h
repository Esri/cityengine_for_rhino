#pragma once

#include "utils.h"

#include "prt/AttributeMap.h"

#include <string>
#include <unordered_map>
#include <map>

typedef struct _Model Model;

namespace Reporting {

	struct ReportAttribute {
		int mInitialShapeIndex;
		std::wstring mReportName;
		prt::AttributeMap::PrimitiveType mType;

		std::wstring mStringReport;
		double mDoubleReport = 0;
		bool mBoolReport = false;
		int mIntReport = 0;
	};

	void extractReports(int initShapeId, Model& model, const prt::AttributeMap* reports);

	using ReportMap = std::unordered_map<std::wstring, ReportAttribute>;
	using GroupedReportMap = std::map<std::wstring, std::vector<ReportAttribute>>;

	const std::vector<ReportAttribute> EMPTY_REPORTS;
	const double EMPTY_REPORT_DOUBLE = std::numeric_limits<double>::lowest();
	const bool EMPTY_REPORT_BOOL(false);
	const std::wstring EMPTY_REPORT_STRING;

	class GroupedReports {
	public:

		void add(const ReportAttribute& report);
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

