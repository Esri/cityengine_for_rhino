/*
  COPYRIGHT (c) 2012-2020 Esri R&D Center Zurich
  TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
  Unpublished material - all rights reserved under the
  Copyright Laws of the United States and applicable international
  laws, treaties, and conventions.

  For additional information, contact:
  Environmental Systems Research Institute, Inc.
  Attn: Contracts and Legal Services Department
  380 New York Street
  Redlands, California, 92373
  USA

  email: contracts@esri.com
*/

#ifndef PRTX_MESH_H_
#define PRTX_MESH_H_

#include "prtx/prtx.h"
#include "prtx/Types.h"
#include "prtx/Material.h"

#include <vector>
#include <memory>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4231 4251 4275 4660)
#endif


namespace prtx {


typedef std::vector<uint32_t>  IndexVector;		///< vector of indices

class BoundingBox;

class Mesh;
typedef std::shared_ptr<Mesh> MeshPtr;		///< shared Mesh pointer
typedef std::vector<MeshPtr>  MeshPtrVector;     ///< vector of shared Mesh pointers


/**
 * Read-only description of a polygonal mesh.
 * A Mesh consists of vertex coordinates, normal coordinates, texture coordinates (uv coordinates),
 * materials and polygons (faces). Faces are described as a set of indices into the former.
 *
 * Faces (polygons) can be textured with an arbitrary number of uv sets and
 * can have normals at their vertices.
 * Each face can have a different Material assigned.
 * Faces with holes are supported. Holes are encoded as faces, and the relationship outer encircling face -
 * inner hole face is encoded on top of that.
 *
 * Mesh conditions:
 *  * has at least 1 face
 *  * vertex coordinates count is a multiple of 3
 *  * normal coordinates must is multiple of 3
 *  * uv coordinates is a multiple of 2 (per uv set)
 *  * vertex, normal and uv indices are in valid ranges , i.e. vertex indices are in [0, getVertexCoords().size()/3[
 *  * vertex normals must have length > 0.001
 *  * face:
 *    * has at least 1 vertex (1 vertex = 'point', 2 vertices = 'edge')
 *    * if vertex normals and/or uv coordinates are present: the number of indices is identical to the number of vertices
 *    * if the number of uvsets is in [0, getUVSetsCount()[
 *    * the material index is in in [0, getMaterials().size()[
 *
 * @sa MeshBuilder
 */
class PRTX_EXPORTS_API Mesh : public Content {
public:
	// @cond
	friend class MeshBuilder;
	struct Payload;
	// @endcond

public:
	virtual ~Mesh();

	/**
	 * @returns the name of the mesh. Might be empty.
	 */
	const std::wstring&			getName() const;
	/**
	 * @returns the axis-aligned BoundingBox of the mesh.
	 */
	const BoundingBox&			getBoundingBox() const;

	/**
	 * Gets all vertex coordinates as a series of (x,y,z) triplets. Size of the vector is 3 * number of vertices.
	 * @returns all vertex coordinates as a series of (x,y,z) triplets.
	 */
	const DoubleVector&			getVertexCoords() const;
	/**
	 * Gets all vertex normal coordinates as a series of (x,y,z) triplets. Size of the vector is 3 * number of normals.
	 * @returns all vertex normals as a series of (x,y,z) triplets.
	 */
	const DoubleVector&			getVertexNormalsCoords() const;
	/**
	 * Gets all uv coordinates (texture coordinates) of a specific uv set as a series of (u,v) pairs.
	 * Size of the vector is 2 * number of uvs.
	 * @param uvSet Index of uv set.
	 * @returns all uvs as a series of (u,v) pairs.
	 */
	const DoubleVector&			getUVCoords(uint32_t uvSet) const;
	/**
	 * Gets all texture regions of a specific uv set as a series of (u_min,v_min,u_max,v_max) quadruples. The region
	 * with index 0 is always (0,0,1,1). Actual sub-regions can therefore be identified by indices > 0.
	 * Size of the vector is 4 * number of regions.
	 * @param uvSet Index of uv set.
	 * @returns all regions as a series of (u_min,v_min,u_max,v_max) quadruples.
	 */
	const DoubleVector&			getTextureRegions(uint32_t uvSet) const;
	/**
	 * @returns Number of uv sets.
	 */
	uint32_t					getUVSetsCount() const;
	/**
	 * @returns vector with all materials.
	 */
	const MaterialPtrVector&	getMaterials() const;

	/**
	 * @returns number of faces.
	 */
	uint32_t					getFaceCount() const;
	/**
	 * Gets the number of vertices in each face. Size of the vector is the number of faces.
	 * @returns a vector with the number of vertices for each face.
	 */
	const IndexVector&			getFaceVertexCounts() const;
	/**
	 * Gets the number of vertices of the selected face.
	 * @param faceId Index of face.
	 * @returns number of vertices of face faceId
	 */
	uint32_t					getFaceVertexCount(uint32_t faceId) const;
	/**
	 * Gets the vertex indices for the selected face. Vertex indices point into the vertex coordinates;
	 * i.e. vertex i has coordinates [i*3, i*3+1, i*3+2].
	 * @param faceId Index of face.
	 * @returns pointer to array with vertex indices; size of array is getFaceVertexCount(faceId).
	 */
	const uint32_t*				getFaceVertexIndices(uint32_t faceId) const;

	/**
	 * @returns number of holes.
	 */
	uint32_t					getHolesCount() const;
	/**
	 * @returns vector with the number of holes for each face. Size of vector is the number of faces.
	 */
	const IndexVector&			getFaceHolesCounts() const;
	/**
	 * @param faceId Index of face.
	 * @returns the number of holes in the selected face.
	 */
	uint32_t					getFaceHolesCount(uint32_t faceId) const;
	/**
	 * @param faceId Index of face.
	 * @returns pointer to array with all face indices of the holes for the selected face. Size of array is getFaceHolesCount(faceId).
	 */
	const uint32_t*				getFaceHolesIndices(uint32_t faceId) const;

	/**
	 * Gets the material indices for all faces.
	 * @returns vector with the index into the materials for each face. Size of vector is the number of faces.
	 */
	const IndexVector&			getFaceMaterialIndices() const;
	/**
	 * @param faceId Index of face.
	 * @returns the material index of the selected face.
	 */
	uint32_t					getFaceMaterialIndex(uint32_t faceId) const;

	/**
	 * A face can optionally have vertex normals. The number of vertex normals for a face is either 0 or equal to the
	 * number of vertices.
	 * Gets the number of vertex normals in each face. Size of the vector is the number of faces.
	 * @returns a vector with the number of vertex normalss for each face.
	 */
	const IndexVector&			getFaceVertexNormalCounts() const;
	/**
	 * A face can optionally have vertex normals. The number of vertex normals for a face is either 0 or equal to the
	 * number of vertices.
	 * @param faceId Index of face.
	 * @returns the number of vertex normals for the selected face.
	 */
	uint32_t					getFaceVertexNormalCount(uint32_t faceId) const;
	/**
	 * Gets the vertex normal indices for the selected face. Vertex normal indices point into the vertex normal coordinates;
	 * i.e. normal i has coordinates [i*3, i*3+1, i*3+2].
	 * @param faceId Index of face.
	 * @returns pointer to array with all vertex normal indices of the selected face. Size of array is getFaceVertexNormalCount(faceId).
	 */
	const uint32_t*				getFaceVertexNormalIndices(uint32_t faceId) const;

	/**
	 * A face can optionally have several layers of uv coordinates. The number of uv coordinates for a face and for a
	 * specific uv set is either 0 or equal to the number of vertices.
	 * @param uvSet Index of uv set.
	 * @returns the number of uvs for each face for the selected uv set.
	 */
	const IndexVector&			getFaceUVCounts(uint32_t uvSet) const;
	/**
	 * A face can optionally have several layers of uv coordinates. The number of uv coordinates for a face and for a
	 * specific uv set is either 0 or equal to the number of vertices.
	 * @param faceId Index of face.
	 * @param uvSet  Index of uv set.
	 * @returns the number of uvs for the selected face for the selected uv set.
	 */
	uint32_t					getFaceUVCount(uint32_t faceId, uint32_t uvSet) const;
	/**
	 * Gets the uv coordinate indices for the selected face and the selected uv set. Uv indices point into the uv coordinates
	 * of that set; i.e. uvs i has coordinates [i*2, i*2+1].
	 * @param faceId Index of face.
	 * @param uvSet  Index of uv set.
	 * @returns pointer to array with all uv indices of the selected face and uv set. Size of array is getFaceUVCount(faceId, uvSet).
	 */
	const uint32_t*				getFaceUVIndices(uint32_t faceId, uint32_t uvSet) const;
	/**
	 * Gets the texture region coordinate indices for the selected uv set. Region indices point into the region coordinates
	 * of that set; i.e. region i has coordinates [i*4, i*4+1, i*4+2, i*4+3]. The region with index 0 is always (0,0,1,1).
	 * Actual sub-regions can therefore be identified by indices > 0.
	 * @param uvSet Index of uv set.
	 * @returns vector with the index into the texture regions for each face. Size of vector is the number of faces.
	 */
	const IndexVector&			getFaceTextureRegionIndices(uint32_t uvSet) const;
	/**
	 * Gets the region index for the selected face and the selected uv set. Region indices point into the region coordinates
	 * of that set; i.e. region i has coordinates [i*4, i*4+1, i*4+2, i*4+3]. The region with index 0 is always (0,0,1,1).
	 * Actual sub-regions can therefore be identified by indices > 0.
	 * @param faceId Index of face.
	 * @param uvSet  Index of uv set.
	 * @returns the region index of the selected face and uv set.
	 */
	uint32_t					getFaceTextureRegionIndex(uint32_t faceId, uint32_t uvSet) const;

	/**
	 * Gets the index of the encircling face for a hole.
	 * @param faceId Index of face (hole).
	 * @returns index of encircling face or -1 if faceId is not a hole.
	 */
	int32_t						getFaceEncirclingFace(uint32_t faceId) const;

	/**
	 * Returns an estimation of the memory allocated by this prtx::Mesh instance (in bytes).
	 */
	size_t						getUsedMem() const;

protected:
	// @cond
	Mesh(Payload const* p);
	const Payload* mPayload;
	// @endcond
};


/**
 * A builder to construct meshes. Components (vertex coordinates, normal coordinates, uv coordinates, materials and faces) can be
 * added and get a unique index assigned.
 * The MeshBuilder allows for random access on the faces: faces can be added first and their content defined afterwards.
 *
 * @sa Mesh, MeshUtils
 *
 */
class PRTX_EXPORTS_API MeshBuilder : public SharedPtrBuilder<Mesh> {
public:
	/**
	 * Constructor. Creates an empty builder.
	 */
	MeshBuilder();
	MeshBuilder(const MeshBuilder& mb);
	/**
	 * Constructor which initializes the builder to a copy of the passed builder where existing vertices and normals are transformed.
	 * @param mb        The builder to copy and transform.
	 * @param trafoMatrix Matrix to transform the builder with. 4x4 harmonic transformation matrix, column major (OpenGL).
	 */
	MeshBuilder(const MeshBuilder& mb, const DoubleVector& trafoMatrix);
	/**
	 * Constructor which initializes the builder to a copy of the passed mesh.
	 * @param mesh The mesh to initialize the builder with.
	 */
	MeshBuilder(const Mesh& mesh);
	/**
	 * Constructor which initializes the builder to a transformed copy of the passed mesh.
	 * @param mesh        The mesh to transform and initialize the builder with.
	 * @param trafoMatrix Matrix to transform mesh with. 4x4 harmonic transformation matrix, column major (OpenGL).
	 */
	MeshBuilder(const Mesh& mesh, const DoubleVector& trafoMatrix);
	virtual ~MeshBuilder();

	MeshBuilder& operator=(const MeshBuilder& right);


	// SharedPtrBuilder interface implementation
	virtual MeshPtr createShared(std::wstring* warnings = 0) const;
	virtual MeshPtr createSharedAndReset(std::wstring* warnings = 0);


	/**
	 * Resets the builder.
	 */
	void reset();
	/**
	 * Removes all previously added materials.
	 */
	void resetMaterials();

	/**
	 * Set the name of the Mesh.
	 * @param name name to set.
	 */
	void     setName(const std::wstring& name);

	/**
	 * Adds a material to the material array.
	 * @param material Material to add.
	 * @returns index of the Material.
	 */
	uint32_t addMaterial(const MaterialPtr& material);
	/**
	 * Adds a vector of materials to the material array.
	 * @param materials Materials to add.
	 */
	void     addMaterials(const MaterialPtrVector& materials);

	/**
	 * Adds a vertex (i.e. a coordinate triplet) to the vertex coordinates array.
	 * @param coords (x, y, z) coordinate triplet to add.
	 * @returns index of the added vertex (i.e. vertex i has coordinates [i*3, i*3+1, i*3+2].
	 */
	uint32_t addVertexCoords(const double coords[3]);
	/**
	 * Adds a number of vertices (i.e. a number of coordinate triplets) to the vertex coordinates array.
	 * @param coords Pointer to an array of (x, y, z) coordinate triplets to add.
	 * @param count  Number of coordinates (i.e. vertexCount * 3) to add.
	 */
	void     addVertexCoords(const double* coords, size_t count);
	/**
	 * Adds a number of vertices (i.e. a number of coordinate triplets) to the vertex coordinates array.
	 * @param coords Pointer to a vector of (x, y, z) coordinate triplets to add.
	 */
	void     addVertexCoords(const DoubleVector& coords);

	/**
	 * Adds a normal (i.e. a coordinate triplet) to the normal coordinates array.
	 * @param coords (x, y, z) coordinate triplet to add.
	 * @returns index of the added normal (i.e. normal i has coordinates [i*3, i*3+1, i*3+2].
	 */
	uint32_t addNormalCoords(const double coords[3]);
	/**
	 * Adds a number of normals (i.e. a number of coordinate triplets) to the normal coordinates array.
	 * @param coords Pointer to an array of (x, y, z) coordinate triplets to add.
	 * @param count  Number of coordinates (i.e. normalCount * 3) to add.
	 */
	void     addNormalCoords(const double* coords, size_t count);
	/**
	 * Adds a number of normals (i.e. a number of coordinate triplets) to the normal coordinates array.
	 * @param coords Pointer to a vector of (x, y, z) coordinate triplets to add.
	 */
	void     addNormalCoords(const DoubleVector& coords);

	/**
	 * Adds a uv tuple (i.e. a coordinate tuple) to the selected uv coordinates array.
	 * @param uvSet  Index of uv set.
	 * @param coords (u, v) coordinate tuple to add.
	 * @returns index of the added uv tuple (i.e. uv tuple i has coordinates [i*2, i*2+1]).
	 */
	uint32_t addUVCoords(uint32_t uvSet, const double coords[2]);
	/**
	 * Adds a number of uv tuples (i.e. a number of coordinate tuples) to the selected uv coordinates array.
	 * @param uvSet  Index of uv set.
	 * @param coords Pointer to an array of (u, v) coordinate tuples to add.
	 * @param count  Number of coordinates (i.e. uvTupleCount * 2) to add.
	 */
	void     addUVCoords(uint32_t uvSet, const double* coords, size_t count);
	/**
	 * Adds a number of uv tuples (i.e. a number of coordinate tuples) to the selected uv coordinates array.
	 * @param uvSet  Index of uv set.
	 * @param coords Pointer to a vector of (u, v) coordinate tuples to add.
	 */
	void     addUVCoords(uint32_t uvSet, const DoubleVector& coords);

	/**
	 * Adds a texture region quadruple to the selected texture region array.
	 * @param uvSet  Index of uv set.
	 * @param coords (u, v) coordinate tuple to add.
	 * @returns index of the added texture region quadruple.
	 *
	 * Note: texture regions are only supported by the EncodePreparator, not yet by decoders and CGA itself.
	 */
	uint32_t addTextureRegionCoords(uint32_t uvSet, const double coords[4]);
	/**
	 * Adds a number of texture region quadruples to the selected texture region array.
	 * @param uvSet  Index of uv set.
	 * @param coords Pointer to an array of (u_min, v_min, u_max, v_max) coordinate tuples to add.
	 * @param count  Number of coordinates (i.e. textureRegionCount * 4) to add.
	 *
	 * Note: texture regions are only supported by the EncodePreparator, not yet by decoders and CGA itself.
	 */
	void     addTextureRegionCoords(uint32_t uvSet, const double* coords, size_t count);
	/**
	 * Adds a number of texture region quadruples to the texture region array.
	 * @param uvSet  Index of uv set.
	 * @param coords Pointer to a vector of (u_min, v_min, u_max, v_max) coordinate quadruples to add.
	 *
	 * Note: texture regions are only supported by the EncodePreparator, not yet by decoders and CGA itself.
	 */
	void     addTextureRegionCoords(uint32_t uvSet, const DoubleVector& coords);

	/**
	 * Adds a face with the passed indices.
	 * @param vertexIndices    Vector with vertex indices of face.
	 * @param holeFacesIndices Vector with face indices of holes inside this face.
	 * @param materialIndex    Material index for this face.
	 * @param normalIndices    Vector with vertex normal indices of face. Might be empty.
	 * @param uvIndices        Vector with a vector of uv indices for face. Vectors might be empty.
	 * @returns index of new face.
	 */
	uint32_t addFace(const IndexVector& vertexIndices, const IndexVector& holeFacesIndices,
			uint32_t materialIndex, const IndexVector& normalIndices, const std::vector<IndexVector>& uvIndices);
	/**
	 * Adds an empty face. Use trhe returned index to setup face details.
	 * @returns index of new face.
	 */
	uint32_t addFace();
	/**
	 * Adds a vertex index to the selected face.
	 * @param faceId Index of face.
	 * @param index  Vertex index to add.
	 */
	void addFaceVertexIndex(uint32_t faceId, uint32_t index);
	/**
	 * Adds a hole face index to the selected face.
	 * @param faceId Index of face.
	 * @param index  Face index of hole to add.
	 */
	void addFaceHoleIndex(uint32_t faceId, uint32_t index);
	/**
	 * Adds a normal index to the selected face.
	 * @param faceId Index of face.
	 * @param index  Normal index to add.
	 */
	void addFaceNormalIndex(uint32_t faceId, uint32_t index);
	/**
	 * Adds a uv index to the selected face and the selected uv set.
	 * @param faceId Index of face.
	 * @param uvSet  Index of uv set.
	 * @param index  UV index to add.
	 */
	void addFaceUVIndex(uint32_t faceId, uint32_t uvSet, uint32_t i);

	/**
	 * Sets the vertex indices of the selected face.
	 * @param faceId  Index of face.
	 * @param indices Vertex indices to set.
	 */
	void setFaceVertexIndices(uint32_t faceId, const IndexVector& indices);
	/**
	 * Sets the normal indices of the selected face.
	 * @param faceId  Index of face.
	 * @param indices Normal indices to set.
	 */
	void setFaceNormalIndices(uint32_t faceId, const IndexVector& indices);
	/**
	 * Sets the uv indices of the selected face and the selected uv set.
	 * @param faceId  Index of face.
	 * @param uvSet   Index of uv set.
	 * @param indices UV indices to set.
	 */
	void setFaceUVIndices(uint32_t faceId, uint32_t uvSet, const IndexVector& indices);

	/**
	 * Sets the material index of the selected face.
	 * @param faceId Index of face.
	 * @param index  Material index to set.
	 */
	void setFaceMaterialIndex(uint32_t faceId, uint32_t index);
	/**
	 * Sets the texture region index in the selected uv set of the selected face.
	 * @param faceId Index of face.
	 * @param uvSet  Index of uv set.
	 * @param index  texture region index to set.
	 */
	void setFaceTextureRegionIndex(uint32_t faceId, uint32_t uvSet, uint32_t index);

	/**
	 * @returns true if the builder is empty, i.e. nothing got set or added.
	 */
	bool isEmpty() const;

	struct Face;

private:
	void checkAndClean(std::wstring* warnings) const;

	Mesh::Payload *mPayload;
	std::vector<Face*> mFaces;
};

} // namespace prtx

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* PRTX_MESH_H_ */
