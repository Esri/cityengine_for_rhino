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

void RhinoCallbacks::add(const size_t initialShapeIndex,
						 const double * vertexCoords, const size_t vertexCoordsCount, 
						 const uint32_t * faceIndices, const size_t faceIndicesCount, 
						 const uint32_t * faceCounts, const size_t faceCountsCount, 
						 double const * const * uvs, size_t const * uvsSizes, 
						 uint32_t const * const * uvCounts, size_t const * uvCountsSizes, 
						 uint32_t const * const * uvIndices, size_t const * uvIndicesSizes, 
						 uint32_t uvSets,
						 const uint32_t* faceRanges, size_t faceRangesSize,
						 const prt::AttributeMap ** materials)
{
	addGeometry(initialShapeIndex, vertexCoords, vertexCoordsCount,
		faceIndices, faceIndicesCount,
		faceCounts, faceCountsCount);

	Model& currentModel = mModels[initialShapeIndex];
	
	// Add texture coordinates
	for (size_t uvSet = 0; uvSet < uvSets; ++uvSet)
	{
		size_t const psUVSSize = uvsSizes[uvSet];
		size_t const psUVCountsSize = uvCountsSizes[uvSet];
		size_t const psUVIndicesSize = uvIndicesSizes[uvSet];

#ifdef DEBUG
		LOG_DBG << " -- uvset " << uvSet << ": psUVCountsSize = " << psUVCountsSize << ", psUVIndicesSize = " << psUVIndicesSize;
#endif

		if (psUVSSize > 0 && psUVIndicesSize > 0 && psUVCountsSize > 0)
		{
			if (uvSet == 0)
			{
				double const* const psUVS = uvs[uvSet];
				uint32_t const* const psUVCounts = uvCounts[uvSet];
				uint32_t const* const psUVIndices = uvIndices[uvSet];

				for (size_t uvi = 0; uvi < psUVIndicesSize; ++uvi)
				{
					const uint32_t uvIdx = psUVIndices[uvi];
					const auto du = psUVS[uvIdx * 2 + 0];
					const auto dv = psUVS[uvIdx * 2 + 1];
					currentModel.mUVs.Append(ON_2fPoint(du, dv));
				}
			}
			else
			{
				// TODO add the other uv sets if any.
			}
		}
	}

	// -- convert materials into material attributes
#ifdef DEBUG
	LOG_DBG << "got " << faceRangesSize - 1 << " face ranges";
#endif
	if (faceRangesSize > 1) 
	{
		for (size_t fri = 0; fri < faceRangesSize - 1; ++fri) 
		{
			if (materials)
			{
				const prt::AttributeMap* attrMap = materials[fri];
				Materials::extractMaterials(initialShapeIndex, fri, attrMap, currentModel.mMaterials);
			}
		}
	}
}

void RhinoCallbacks::addReport(const size_t initialShapeIndex, const prtx::PRTUtils::AttributeMapPtr reports)
{
#ifdef DEBUG
	LOG_DBG << "In RhinoCallback::addReport";
#endif

	if (!reports) {
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
