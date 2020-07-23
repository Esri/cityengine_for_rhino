#pragma once

#include "prt/Callbacks.h"
#include "IRhinoCallbacks.h"

#include "utils.h"
#include "Logger.h"
#include "ReportAttribute.h"

#include <vector>
#include <unordered_map>
#include <iostream>

#define DEBUG

typedef struct _Model {
	std::vector<double> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaces;
	ReportMap mReports;
} Model;

class RhinoCallbacks : public IRhinoCallbacks {
private:

	std::vector<Model> mModels;

public:

	RhinoCallbacks(const size_t initialShapeCount) {
		mModels.resize(initialShapeCount);
	}

	virtual ~RhinoCallbacks() = default;

	// Inherited via IRhinoCallbacks
	void addGeometry(const size_t initialShapeIndex, const double * vertexCoords, const size_t vextexCoordsCount, const uint32_t * faceIndices, const size_t faceIndicesCount, const uint32_t * faceCounts, const size_t faceCountsCount) override;
	void add(const size_t initialShapeIndex, const double * vertexCoords, const size_t vextexCoordsCount, const uint32_t * faceIndices, const size_t faceIndicesCount, const uint32_t * faceCounts, const size_t faceCountsCount, const uint32_t* faceRanges, size_t faceRangeSize, const prt::AttributeMap** reports) override;
	void addReport(const size_t initialShapeIndex, const prt::AttributeMap* reports) override;

	size_t getInitialShapeCount() const {
		return mModels.size();
	}

	const std::vector<double>& getVertices(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mVertices;
	}

	const std::vector<uint32_t>& getIndices(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mIndices;
	}

	const std::vector<uint32_t>& getFaces(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mFaces;
	}

	const ReportMap& getReport(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].mReports;
	}

	prt::Status generateError(size_t isIndex, prt::Status status, const wchar_t* message) {
		LOG_ERR << L"GENERATE ERROR:" << isIndex << " " << status << " " << message;
		return prt::STATUS_OK;
	}

	prt::Status assetError(size_t isIndex, prt::CGAErrorLevel level, const wchar_t* key, const wchar_t* uri,
		const wchar_t* message) {
		LOG_ERR << L"ASSET ERROR:" << isIndex << " " << level << " " << key << " " << uri << " " << message << std::endl;
		return prt::STATUS_OK;
	}

	prt::Status cgaError(size_t isIndex, int32_t shapeID, prt::CGAErrorLevel level, int32_t methodId, int32_t pc,
		const wchar_t* message) {
		LOG_ERR << L"CGA ERROR:" << isIndex << " " << shapeID << " " << level << " " << methodId << " " << pc << " " << message << std::endl;
		return prt::STATUS_OK;
	}

	prt::Status cgaPrint(size_t isIndex, int32_t shapeID, const wchar_t* txt) {
		LOG_INF << L"CGA PRINT:" << isIndex << " " << shapeID << " " << txt << std::endl;
		return prt::STATUS_OK;
	}

	prt::Status cgaReportBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, bool /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, double /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status cgaReportString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
		const wchar_t* /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, bool /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, double /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const wchar_t* /*value*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrBoolArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const bool* /*ptr*/,
		size_t /*size*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrFloatArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const double* /*ptr*/,
		size_t /*size*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrStringArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
		const wchar_t* const* /*ptr*/, size_t /*size*/) {
		return prt::STATUS_OK;
	}

	prt::Status attrBoolArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const bool* /*ptr*/,
		size_t /*size*/, size_t) {
		return prt::STATUS_OK;
	}

	prt::Status attrFloatArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/, const double* /*ptr*/,
		size_t /*size*/, size_t) {
		return prt::STATUS_OK;
	}

	prt::Status attrStringArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
		const wchar_t* const* /*ptr*/, size_t /*size*/, size_t) {
		return prt::STATUS_OK;
	}
};