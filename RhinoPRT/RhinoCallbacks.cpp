#include "RhinoCallbacks.h"

void Model::addMaterial(const Materials::MaterialAttribute& ma)
{
	mMaterials.insert_or_assign(ma.mMatId, ma);
}

void Model::addReport(const Reporting::ReportAttribute& ra)
{
	mReports.emplace(ra.mReportName, ra);
}

const std::vector<ModelPart>& Model::getModelParts() const { return mModelParts; }
const Reporting::ReportMap& Model::getReports() const { return mReports; }
const Materials::MaterialsMap& Model::getMaterials() const { return mMaterials; }

bool RhinoCallbacks::addGeometry(const size_t initialShapeIndex,
								 const double * vertexCoords, const size_t vertexCoordsCount,
								 const double * normals, const size_t normalsCount,
								 const uint32_t * faceIndices, const size_t faceIndicesCount,
								 const uint32_t * faceCounts, const size_t faceCountsCount)
{
	if (vertexCoords == nullptr || normals == nullptr || faceIndices == nullptr || faceCounts == nullptr)
		return false;

	Model& currentModel = mModels[initialShapeIndex];
	ModelPart& modelPart = currentModel.addModelPart();

	modelPart.mVertices.assign(vertexCoords, vertexCoords + vertexCoordsCount);
	modelPart.mNormals.assign(normals, normals + normalsCount);
	modelPart.mIndices.assign(faceIndices, faceIndices + faceIndicesCount);
	modelPart.mFaces.assign(faceCounts, faceCounts + faceCountsCount);

	return true;
}

void RhinoCallbacks::addUVCoordinates(const size_t initialShapeIndex,
									  double const * const * uvs, size_t const * uvsSizes,
									  uint32_t const * const * uvCounts, size_t const * uvCountsSizes,
									  uint32_t const * const * uvIndices, size_t const * uvIndicesSizes,
									  uint32_t uvSets)
{
	Model& currentModel = mModels[initialShapeIndex];
	ModelPart& modelPart = currentModel.getCurrentModelPart();

	// Add texture coordinates
	for (size_t uvSet = 0; uvSet < uvSets; ++uvSet)
	{
		size_t const psUVSSize = uvsSizes[uvSet];
		size_t const psUVCountsSize = uvCountsSizes[uvSet];
		size_t const psUVIndicesSize = uvIndicesSizes[uvSet];

		if (psUVSSize > 0 && psUVIndicesSize > 0 && psUVCountsSize > 0)
		{
			if (uvSet == 0)
			{
#ifdef DEBUG
				LOG_DBG << " -- uvset " << uvSet << ": psUVCountsSize = " << psUVCountsSize << ", psUVIndicesSize = " << psUVIndicesSize;
#endif

				double const* const psUVS = uvs[uvSet];
				uint32_t const* const psUVCounts = uvCounts[uvSet];
				uint32_t const* const psUVIndices = uvIndices[uvSet];

				modelPart.mUVIndices.reserve(psUVIndicesSize);
				modelPart.mUVs.Reserve(psUVIndicesSize);
				modelPart.mUVCounts.reserve(psUVCountsSize);

				modelPart.mUVCounts.insert(modelPart.mUVCounts.end(), psUVCounts + 0, psUVCounts + psUVCountsSize);

				for (size_t uvi = 0; uvi < psUVIndicesSize; ++uvi)
				{
					const uint32_t uvIdx = psUVIndices[uvi];
					const auto du = psUVS[uvIdx * 2 + 0];
					const auto dv = psUVS[uvIdx * 2 + 1];
					modelPart.mUVs.Append(ON_2fPoint(static_cast<float>(du), static_cast<float>(dv)));
					modelPart.mUVIndices.push_back(static_cast<uint32_t>(uvi));
				}
			}
			else
			{
				LOG_INF << "IGNORED UV SET " << uvSet << ": Rhino does not support multiple uv sets.";
			}
		}
	}
}

void RhinoCallbacks::add(const size_t initialShapeIndex, const size_t instanceIndex,
						 const double * vertexCoords, const size_t vertexCoordsCount,
						 const double * normals, const size_t normalsCount,
						 const uint32_t * faceIndices, const size_t faceIndicesCount,
						 const uint32_t * faceCounts, const size_t faceCountsCount, 
						 double const * const * uvs, size_t const * uvsSizes, 
						 uint32_t const * const * uvCounts, size_t const * uvCountsSizes, 
						 uint32_t const * const * uvIndices, size_t const * uvIndicesSizes, 
						 uint32_t uvSets,
						 const uint32_t* /*faceRanges*/, size_t /*faceRangesSize*/,
						 const prt::AttributeMap ** materials, const size_t matCount)
{
	if (!addGeometry(initialShapeIndex, vertexCoords, vertexCoordsCount,
		normals, normalsCount,
		faceIndices, faceIndicesCount,
		faceCounts, faceCountsCount)) return;
	addUVCoordinates(initialShapeIndex, uvs, uvsSizes,
		uvCounts, uvCountsSizes,
		uvIndices, uvIndicesSizes, uvSets);

	Model& currentModel = mModels[initialShapeIndex];

	// -- convert materials into material attributes
#ifdef DEBUG
	LOG_DBG << "got " << matCount << " materials";
#endif
	if (matCount > 0 && materials) 
	{
		if(matCount > 1) {
			LOG_ERR << L"Multiple material for a single mesh part is not supported by Rhino. Taking only the first material" << std::endl;
		}

		const prt::AttributeMap* attrMap = materials[0];
		auto ma = Materials::extractMaterials(initialShapeIndex, instanceIndex, attrMap);
		currentModel.addMaterial(ma);
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
