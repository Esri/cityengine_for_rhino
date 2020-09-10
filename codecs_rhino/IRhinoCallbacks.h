#pragma once

#include "prt/Callbacks.h"
#include "prtx/PRTUtils.h"

class IRhinoCallbacks : public prt::Callbacks {
public:
	virtual ~IRhinoCallbacks() override = default;

	virtual void addGeometry(const size_t initialShapeIndex, const double* vertexCoords, const size_t vextexCoordsCount,
		const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
		const size_t faceCountsCount, double const* const* uvs, size_t const* uvsSizes,
		uint32_t const* const* uvCounts, size_t const* uvCountsSizes,
		uint32_t const* const* uvIndices, size_t const* uvIndicesSizes,
		uint32_t uvSets) = 0;

	///<summary>
	///Adds a geometry and corresponding materials.
	///</summary>
	///<param name='faceRanges'>The range of the reports</param>
	///<param name='faceRangeSize'>The number of ranges</param>
	///<param name='reports'>An array of reports</param>
	virtual void add(const size_t initialShapeIndex, const size_t instanceIndex,
		const double* vertexCoords, const size_t vextexCoordsCount,
		const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
		const size_t faceCountsCount,
		double const* const* uvs, size_t const* uvsSizes,
		uint32_t const* const* uvCounts, size_t const* uvCountsSizes,
		uint32_t const* const* uvIndices, size_t const* uvIndicesSizes,
		uint32_t uvSets,
		const uint32_t* faceRanges, size_t faceRangesSize,
		const prt::AttributeMap** materials, size_t matCount) = 0;

	virtual void addReport(const size_t initialShapeIndex, const prtx::PRTUtils::AttributeMapPtr reports) = 0;
};