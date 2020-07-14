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
	

	//TODO: Convert the prt geometry into Rhino geometry.
}

void RhinoCallbacks::addReports(const size_t initialShapeIndex, const wchar_t ** stringReportKeys,
								const wchar_t ** stringReportValues, size_t stringReportCount, 
								const wchar_t ** floatReportKeys, const double * floatReportValues, size_t floatReportCount, 
								const wchar_t ** boolReportKeys, const bool * boolReportValues, size_t boolReportCount)
{
	// TODO??
}
