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

#include "ReportAttribute.h"

#include "RhinoCallbacks.h"

#include <algorithm>

namespace {

void syncReports(const int shapeID, std::vector<Reporting::ReportAttribute>& reports) {
	size_t currID = reports.size();

	while (currID < shapeID) {
		reports.push_back(Reporting::getEmptyReport(currID));
		currID++;
	}
}

void addToMap(Reporting::GroupedReportMap& reports, const Reporting::ReportAttribute& report,
              const int initialShapeIndex) {
	auto it = reports.find(report.mReportName);
	if (it == reports.end()) {
		std::vector<Reporting::ReportAttribute> newVect;
		syncReports(initialShapeIndex, newVect);
		newVect.push_back(report);

		reports.insert(it, std::make_pair(report.mReportName, newVect));
	}
	else {
		syncReports(initialShapeIndex, reports.at(report.mReportName));
		reports.at(report.mReportName).push_back(report);
	}
}

} // namespace

namespace Reporting {

ReportAttribute getEmptyReport(size_t initialShapeIndex) {
	ReportAttribute report;
	report.mInitialShapeIndex = initialShapeIndex;
	report.mReportName = L"Empty Report";
	report.mType = prt::AttributeMap::PrimitiveType::PT_UNDEFINED;

	return report;
}

void extractReports(size_t initShapeId, Model& model, const prtx::PRTUtils::AttributeMapPtr reports) {
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

		// model.mReports.emplace(ra.mReportName, std::move(ra));
		model.addReport(ra);
	}
}

ReportsVector ToReportsVector(const ReportMap& reports) {
	ReportsVector rep;
	rep.reserve(reports.size());

	for (const auto& report : reports) {
		rep.push_back(report.second);
	}

	// auto fct = [](const std::pair<std::wstring, ReportAttribute> p) { return ReportAttribute(p.second); };

	// auto lastit = std::transform(reports.begin(), reports.end(), rep.end(), fct);

	return rep;
}

void GroupedReports::add(const ReportAttribute& report, const int initialShapeIndex) {
	if (report.mType == prt::AttributeMap::PrimitiveType::PT_BOOL) {
		addToMap(mGroupedBoolReports, report, initialShapeIndex);
	}
	else if (report.mType == prt::AttributeMap::PrimitiveType::PT_FLOAT) {
		addToMap(mGroupedDoubleReports, report, initialShapeIndex);
	}
	else if (report.mType == prt::AttributeMap::PrimitiveType::PT_STRING) {
		addToMap(mGroupedStringReports, report, initialShapeIndex);
	}
}

size_t GroupedReports::getReportCount() const {
	return mGroupedBoolReports.size() + mGroupedDoubleReports.size() + mGroupedStringReports.size();
}

bool GroupedReports::getReportKeys(ON_ClassArray<ON_wString>* pKeysArray, ON_SimpleArray<int>* pKeyTypeArray) const {
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

const std::vector<ReportAttribute>& GroupedReports::getDoubleReports(std::wstring key) const {
	try {
		return mGroupedDoubleReports.at(key);
	}
	catch (const std::out_of_range) {
		return EMPTY_REPORTS;
	}
}

const std::vector<ReportAttribute>& GroupedReports::getBoolReports(std::wstring key) const {
	try {
		return mGroupedBoolReports.at(key);
	}
	catch (const std::out_of_range) {
		return EMPTY_REPORTS;
	}
}

const std::vector<ReportAttribute>& GroupedReports::getStringReports(std::wstring key) const {
	try {
		return mGroupedStringReports.at(key);
	}
	catch (const std::out_of_range) {
		return EMPTY_REPORTS;
	}
}

void GroupedReports::clear() {
	mGroupedBoolReports.clear();
	mGroupedStringReports.clear();
	mGroupedDoubleReports.clear();
}

} // namespace Reporting