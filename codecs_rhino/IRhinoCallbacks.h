#pragma once

#include "prt/Callbacks.h"

class IRhinoCallbacks : public prt::Callbacks {
public:
	virtual ~IRhinoCallbacks() override = default;

	virtual void addGeometry(const size_t initialShapeIndex, const double* vertexCoords, const size_t vextexCoordsCount,
		const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
		const size_t faceCountsCount) = 0;

	virtual void addReports(const size_t initialShapeIndex, const wchar_t** stringReportKeys,
		const wchar_t** stringReportValues, size_t stringReportCount,
		const wchar_t** floatReportKeys, const double* floatReportValues, size_t floatReportCount,
		const wchar_t** boolReportKeys, const bool* boolReportValues, size_t boolReportCount) = 0;
};