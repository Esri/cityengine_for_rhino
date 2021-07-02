/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "utils.h"

#include "prt/AttributeMap.h"
#include "prtx/PRTUtils.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class Model;

namespace Reporting {

struct ReportAttribute {
	size_t mInitialShapeIndex = 0;
	std::wstring mReportName;
	prt::AttributeMap::PrimitiveType mType = prt::AttributeMap::PT_UNDEFINED;

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

ReportAttribute getEmptyReport(size_t initialShapeIndex);

void extractReports(size_t initShapeId, GeneratedModel& model, const prtx::PRTUtils::AttributeMapPtr reports);
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

} // namespace Reporting
