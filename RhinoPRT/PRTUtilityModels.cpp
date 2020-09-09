#include "PRTUtilityModels.h"

#include "Logger.h"

#include <numeric>

/// NOT USED ANYMORE.
InitialShape::InitialShape(const std::vector<double> &vertices) : mVertices(vertices) {
	mIndices.resize(vertices.size() / 3);
	std::iota(std::begin(mIndices), std::end(mIndices), 0);

	// It is specific to the plane surface quad.
	mIndices[1] = 2;
	mIndices[2] = 3;
	mIndices[3] = 1;

	mFaceCounts.resize(1, (uint32_t)mIndices.size());
}

InitialShape::InitialShape(const double* vertices, int vCount, const int* indices, const int iCount, const int* faceCount, const int faceCountCount) {
	mVertices.reserve(vCount);
	mIndices.reserve(iCount);
	mFaceCounts.reserve(faceCountCount);

	for (int i = 0; i < vCount; ++i) {
		mVertices.push_back(vertices[i]);
	}

	for (int i = 0; i < iCount; ++i) {
		mIndices.push_back(indices[i]);
	}

	for (int i = 0; i < faceCountCount; ++i) {
		mFaceCounts.push_back(faceCount[i]);
	}
}

InitialShape::InitialShape(const ON_Mesh& mesh) {
	ON_wString shapeIdxStr;
	if (!mesh.GetUserString(INIT_SHAPE_ID_KEY.c_str(), shapeIdxStr))
	{
		LOG_WRN << L"InitialShapeID not found in given mesh";
		mID = -1;
	}
	else
	{
		std::wstring str(shapeIdxStr.Array());

		//cast to int
		try {
			mID = std::stoi(str);
		}
		catch (std::invalid_argument) {
			LOG_ERR << "Mesh id string was not a number, so it could not be parsed -> setting initial shape id to -1.";
			mID = -1;
		}
		catch (std::out_of_range) {
			LOG_ERR << "Mesh id could not be parsed, setting initial shape id to 0.";
			mID = -1;
		}
	}

	mVertices.reserve(mesh.VertexCount() * 3);
	mIndices.reserve(mesh.FaceCount() * 4);
	mFaceCounts.reserve(mesh.FaceCount());

	for (int i = 0; i < mesh.VertexCount(); ++i) {
		ON_3dPoint vertex = mesh.Vertex(i);
		mVertices.push_back(vertex.x);
		mVertices.push_back(vertex.y);
		mVertices.push_back(vertex.z);
	}

	for (int i = 0; i < mesh.FaceCount(); ++i) {
		mIndices.push_back(mesh.m_F.At(i)->vi[0]);
		mIndices.push_back(mesh.m_F.At(i)->vi[1]);
		mIndices.push_back(mesh.m_F.At(i)->vi[2]);
		if (mesh.m_F.At(i)->IsQuad()) {
			mIndices.push_back(mesh.m_F.At(i)->vi[3]);
			mFaceCounts.push_back(4);
		}
		else {
			mFaceCounts.push_back(3);
		}
	}
}

GeneratedModel::GeneratedModel(const size_t& initialShapeIdx, const std::vector<double>& vert, const std::vector<uint32_t>& indices,
	const std::vector<uint32_t>& face, const Reporting::ReportMap& rep):
	mInitialShapeIndex(initialShapeIdx), mVertices(vert), mIndices(indices), mFaces(face), mReports(rep) { }

GeneratedModel::GeneratedModel(const size_t & initialShapeIdx, const std::vector<double>& vert, const std::vector<uint32_t>& indices, const std::vector<uint32_t>& face,
	const ON_2fPointArray& uvs, const std::vector<uint32_t>& uvsIndices, const std::vector<uint32_t>& uvsCounts,
	const Reporting::ReportMap & rep, const Materials::MaterialsMap & mats):
	mInitialShapeIndex(initialShapeIdx), mVertices(vert), mIndices(indices), mFaces(face), mUVs(uvs), mUVIndices(uvsIndices), mUVCounts(uvsCounts), 
	mReports(rep), mMaterials(mats) { }

GeneratedModel::GeneratedModel(const size_t& initialShapeIdx, const Model& model, const Reporting::ReportMap& rep, const Materials::MaterialsMap& mats):
	mInitialShapeIndex(initialShapeIdx), mModel(model), mReports(rep), mMaterials(mats) { }

const ON_Mesh GeneratedModel::getMeshFromGenModel() const 
{

	size_t nbVertices = mVertices.size() / 3;

	ON_Mesh mesh(mFaces.size(), nbVertices, true, true);

	// Set the initial shape id.
	mesh.SetUserString(INIT_SHAPE_ID_KEY.c_str(), std::to_wstring(mInitialShapeIndex).c_str());

	for (size_t v_id = 0; v_id < nbVertices; ++v_id) {
		mesh.SetVertex(v_id, ON_3dPoint(mVertices[v_id * 3], mVertices[v_id * 3 + 1], mVertices[v_id * 3 + 2]));
	}

	int faceid(0);
	int currindex(0);
	for (int face : mFaces) {
		if (face == 3) {
			mesh.SetTriangle(faceid, mIndices[currindex], mIndices[currindex + 1], mIndices[currindex + 2]);
			currindex += face;
			faceid++;
		}
		else if (face == 4) {
			mesh.SetQuad(faceid, mIndices[currindex], mIndices[currindex + 1], mIndices[currindex + 2], mIndices[currindex + 3]);
			currindex += face;
			faceid++;
		}
		else {
			//ignore face because it is invalid
			currindex += face;
			LOG_WRN << "Ignored face with invalid number of vertices :" << face;
		}
	}

	// Printing a rhino error log if the created mesh is invalid
	ON_wString log_str;
	ON_TextLog log(log_str);
	if (!mesh.IsValid(&log))
	{
		mesh.Dump(log);
		LOG_ERR << log_str;
	}

	mesh.ComputeVertexNormals();
	if (!mesh.Compact()) {
		LOG_ERR << "Mesh has been compacted.";
	}

	// Create the uv mapping
	/*auto uv_mesh = getTextureMappingMesh();
	uv_mesh.ComputeVertexNormals();
	uv_mesh.Compact();*/

	return mesh;
}

const ON_Mesh GeneratedModel::toON_Mesh(const ModelPart& modelPart) const 
{
	ON_Mesh mesh(modelPart.mUVCounts.size(), modelPart.mIndices.size(), true, true);

	// Set the initial shape id.
	mesh.SetUserString(INIT_SHAPE_ID_KEY.c_str(), std::to_wstring(mInitialShapeIndex).c_str());
	
	// Duplicate vertices
	for (size_t v_id = 0; v_id < modelPart.mIndices.size(); ++v_id) {
		auto index = modelPart.mIndices[v_id];
		mesh.SetVertex(v_id, ON_3dPoint(modelPart.mVertices[index * 3], modelPart.mVertices[index * 3 + 1], modelPart.mVertices[index * 3 + 2]));
	}
	
	int faceid(0);
	int currindex(0);
	for (int face : modelPart.mFaces) {
		if (face == 3) {
			mesh.SetTriangle(faceid, currindex, currindex + 1, currindex + 2);
			currindex += face;
			faceid++;
		}
		else if (face == 4) {
			mesh.SetQuad(faceid, currindex, currindex + 1, currindex + 2, currindex + 3);
			currindex += face;
			faceid++;
		}
		else {
			//ignore face because it is invalid
			currindex += face;
			LOG_WRN << "Ignored face with invalid number of vertices :" << face;
		}
	}
	
	for (size_t i = 0; i < modelPart.mUVs.Count(); ++i) {
		mesh.SetTextureCoord(i, modelPart.mUVs[i].x, modelPart.mUVs[i].y);
	}

	mesh.ComputeVertexNormals();
	mesh.Compact();

	// Printing a rhino error log if the created mesh is invalid
	ON_wString log_str;
	ON_TextLog log(log_str);
	if (!mesh.IsValid(&log))
	{
		mesh.Dump(log);
		LOG_ERR << log_str;
	}

	return mesh;
}

const MeshBundle GeneratedModel::getMeshesFromGenModel() const 
{
	MeshBundle mesh;

	for (const auto& modelPart : mModel.mModelParts)
	{
		mesh.push_back(toON_Mesh(modelPart));
	}

	return mesh;
}

