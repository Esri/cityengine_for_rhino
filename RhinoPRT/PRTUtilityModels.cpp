#include "PRTUtilityModels.h"

#include <numeric>

InitialShape::InitialShape() {}

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
	const std::vector<uint32_t>& face, const std::map<std::string, std::string>& rep) :
	mInitialShapeIndex(initialShapeIdx), mVertices(vert), mIndices(indices), mFaces(face), mReport(rep) {}

