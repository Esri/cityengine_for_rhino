#pragma once

#include "prt/Callbacks.h"

class IRhinoCallbacks : public prt::Callbacks {
public:
	virtual ~IRhinoCallbacks() override = default;

	virtual void addGeometry(const size_t initialShapeIndex, const double* vertexCoords, const size_t vextexCoordsCount,
		const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
		const size_t faceCountsCount) = 0;

	///<summary>
	///Adds a geometry and corresponding reports.
	///</summary>
	///<param name='faceRanges'>The range of the reports</param>
	///<param name='faceRangeSize'>The number of ranges</param>
	///<param name='reports'>An array of reports</param>
	virtual void add(const size_t initialShapeIndex, const double* vertexCoords, const size_t vextexCoordsCount,
		const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
		const size_t faceCountsCount, const uint32_t* faceRanges, size_t faceRangeSize, const prt::AttributeMap** reports) = 0;

	virtual void addReport(const size_t initialShapeIndex, const prt::AttributeMap* reports) = 0;
};