#pragma once

#include "prt/Callbacks.h"
#include "IRhinoCallbacks.h"

#include "prtx/PRTUtils.h"
#include "prtx/Types.h"

#include "utils.h"
#include "Logger.h"
#include "ReportAttribute.h"
#include "MaterialAttribute.h"

#include <vector>
#include <unordered_map>
#include <iostream>

#define DEBUG

struct ModelPart {
	std::vector<double> mVertices;
	prtx::DoubleVector mNormals;
	std::vector<uint32_t> mIndices;
	std::vector<uint32_t> mFaces;
	ON_2fPointArray mUVs;
	std::vector<uint32_t> mUVIndices;
	std::vector<uint32_t> mUVCounts;
};

class Model {
public:
	Model() = default;

	ModelPart& addModelPart()
	{ 
		mModelParts.push_back(ModelPart()); 
		return mModelParts.back();
	}

	ModelPart& getCurrentModelPart()
	{
		return mModelParts.back();
	}

	void addMaterial(const Materials::MaterialAttribute& ma);
	void addReport(const Reporting::ReportAttribute& ra);

	const std::vector<ModelPart>& getModelParts() const;
	const Reporting::ReportMap& getReports() const;
	const Materials::MaterialsMap& getMaterials() const;

private:

	std::vector<ModelPart> mModelParts;
	Reporting::ReportMap mReports;
	Materials::MaterialsMap mMaterials;

};

class RhinoCallbacks : public IRhinoCallbacks {
private:

	std::vector<Model> mModels;

public:

	RhinoCallbacks(const size_t initialShapeCount) {
		mModels.resize(initialShapeCount);
	}

	virtual ~RhinoCallbacks() = default;

	// Inherited via IRhinoCallbacks
	bool addGeometry(const size_t initialShapeIndex, const double * vertexCoords, const size_t vertexCoordsCount,
		const double * normals, const size_t normalsCount,
		const uint32_t * faceIndices, const size_t faceIndicesCount, 
		const uint32_t * faceCounts, const size_t faceCountsCount);

	void addUVCoordinates(const size_t initialShapeIndex,
		double const * const * uvs, size_t const * uvsSizes,
		uint32_t const * const * uvCounts, size_t const * uvCountsSizes,
		uint32_t const * const * uvIndices, size_t const * uvIndicesSizes,
		uint32_t uvSets);

	void add(const size_t initialShapeIndex, const size_t instanceIndex,
		const double* vertexCoords, const size_t vertexCoordsCount,
		const double* normals, const size_t normalsCount,
		const uint32_t* faceIndices, const size_t faceIndicesCount, const uint32_t* faceCounts,
		const size_t faceCountsCount,
		double const* const* uvs, size_t const* uvsSizes,
		uint32_t const* const* uvCounts, size_t const* uvCountsSizes,
		uint32_t const* const* uvIndices, size_t const* uvIndicesSizes,
		uint32_t uvSets,
		const uint32_t* faceRanges, size_t faceRangesSize,
		const prt::AttributeMap** materials, size_t matCount) override;
	void addReport(const size_t initialShapeIndex, const prtx::PRTUtils::AttributeMapPtr reports) override;

	size_t getInitialShapeCount() const {
		return mModels.size();
	}

	const Model& getModel(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");
		
		return mModels[initialShapeIdx];
	}

	const Reporting::ReportMap& getReport(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].getReports();
	}

	const Materials::MaterialsMap getMaterial(const size_t initialShapeIdx) const {
		if (initialShapeIdx >= mModels.size())
			throw std::out_of_range("initial shape index is out of range.");

		return mModels[initialShapeIdx].getMaterials();
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