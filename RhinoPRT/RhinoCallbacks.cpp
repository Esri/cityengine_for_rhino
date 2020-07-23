#include "RhinoCallbacks.h"

void RhinoCallbacks::addGeometry(const size_t initialShapeIndex, const double * vertexCoords, 
								 const size_t vextexCoordsCount, const uint32_t * faceIndices,
								 const size_t faceIndicesCount, const uint32_t * faceCounts, const size_t faceCountsCount)
{
	Model& currentModel = mModels[initialShapeIndex];

	if (vertexCoords != nullptr)
		currentModel.mVertices.insert(currentModel.mVertices.end(), vertexCoords, vertexCoords + vextexCoordsCount);

	if (faceIndices != nullptr)
		currentModel.mIndices.insert(currentModel.mIndices.end(), faceIndices, faceIndices + faceIndicesCount);

	if (faceCounts != nullptr)
		currentModel.mFaces.insert(currentModel.mFaces.end(), faceCounts, faceCounts + faceCountsCount);
}

void RhinoCallbacks::add(const size_t initialShapeIndex, const double * vertexCoords, const size_t vextexCoordsCount,
						 const uint32_t * faceIndices, const size_t faceIndicesCount, 
						 const uint32_t * faceCounts, const size_t faceCountsCount,
						 const uint32_t * faceRanges, size_t faceRangeSize, const prt::AttributeMap ** reports)
{
	//TODO
}

void RhinoCallbacks::addReport(const size_t initialShapeIndex, const prt::AttributeMap * reports)
{
#ifdef DEBUG
	LOG_DBG << "In RhinoCallback::addReport";
#endif

	if (reports == nullptr) {
		LOG_WRN << "Trying to add null report, ignoring.";
		return;
	}

	if (mModels.size() <= initialShapeIndex) {
		LOG_ERR << "Shape index is bigger than the number of generated models.";
		return;
	}

	Model& model = mModels[initialShapeIndex];

	Reporting::extractReports(initialShapeIndex, model, reports);

#ifdef DEBUG
	LOG_DBG << "End of RhinoCallback::addReport";
#endif
}
