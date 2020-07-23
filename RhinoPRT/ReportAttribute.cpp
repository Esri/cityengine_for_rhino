#include "ReportAttribute.h"

#include "RhinoCallbacks.h"

namespace Reporting {
	void extractReports(int initShapeId, Model& model, const prt::AttributeMap* reports) {

		size_t keyCount = 0;
		auto keys = reports->getKeys(&keyCount);

		for (size_t i = 0; i < keyCount; ++i) {
			auto key = keys[i];

			Reporting::ReportAttribute ra;
			ra.mInitialShapeIndex = initShapeId;
			ra.mReportName.assign(key);
			ra.mStringReport.assign(key);
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
}