#pragma once

#include "prt/AttributeMap.h"

#include <string>
#include <unordered_map>

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

}

using ReportMap = std::unordered_map<std::wstring, Reporting::ReportAttribute>;