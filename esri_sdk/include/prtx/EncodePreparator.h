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

#ifndef PRTX_ENCODEPREPARATOR_H_
#define PRTX_ENCODEPREPARATOR_H_

#include "prt/Callbacks.h"

#include "prtx/NamePreparator.h"
#include "prtx/Shape.h"
#include "prtx/prtx.h"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4275)
#endif

#include <memory>

namespace prt {
	class Cache;
}

namespace prtx {

class InitialShape;

struct VertexNormalProcessor {
/**
 * Encoding modes for vertex normals.
 */
enum Action {
	PASS,							///< The vertex normals are not modified.
	DELETE_NORMALS,					///< The vertex normals are deleted.
	SET_MISSING_TO_FACE_NORMALS,	///< Any missing vertex normals will be set to the corresponding face normal.
	SET_ALL_TO_FACE_NORMALS			///< All vertex normals will be replaced with the corresponding face normal.
};
};

struct HoleProcessor {
/**
 * Encoding modes for holes.
 */
enum Action {
	PASS,							///< Faces with holes are not modified.
	TRIANGULATE_FACES_WITH_HOLES,	///< Faces with holes will be triangulated.
	DELETE_HOLES,					///< Any holes will be deleted.
	CONVERT_HOLES_TO_FACES			///< Holes will be converted to regular faces.
};
};


/**
 * Optional callback to preprocess material attributes based on initial shape attributes.
 *
 * For example, if a client code creates initial shapes with attributes to control material parameters,
 * this callback can be implemented to perform the actual set calls on the prt::Material objects before the
 * preparation process.
 */
struct MaterialTransformer {
	virtual void update(
			const prt::AttributeMap*	initialShapeAttributes,
			const prtx::MaterialPtr&	originalMaterial,
			prtx::MaterialBuilder&		materialUpdate
	) const = 0;
};


class Reports;
typedef std::shared_ptr<Reports> ReportsPtr;

/**
 * Helper class to transport CGA reports through the EncodePreparator.
 */
class PRTX_EXPORTS_API Reports {
public:
	Reports() { }
	Reports(
		const prtx::Shape::ReportBoolVect&		boolReps,
		const prtx::Shape::ReportFloatVect&		floatReps,
		const prtx::Shape::ReportStringVect&	stringReps
	);

	/**
	 * Append the reports from r to the current object.
	 */
	void append(const ReportsPtr& r);

	prtx::Shape::ReportBoolVect		mBools;
	prtx::Shape::ReportFloatVect	mFloats;
	prtx::Shape::ReportStringVect	mStrings;
};


class EncodePreparator;
typedef std::shared_ptr<EncodePreparator> EncodePreparatorPtr;


/**
 * The EncodePreparator is the most important tool for encoder authors. It implements common geometry encoding tasks.
 * Its main purpose is to gather prtx::Shape objects from the generated shape tree and create exportable groups of
 * prtx::Geometry objects with associated prtx::Mesh, prtx::Material and prtx::Texture objects. It identifies identical
 * geometry, material and texture instances and allows for their efficient sharing (e.g. to implement instancing).
 *
 * The preparation process is triggered by calling fetchFinalizedInstances and can be controlled by
 * setting the appropriate PreparationFlags.
 *
 * A simple usage example looks like this:
 * \code{.cpp}
	#include "prtx/EncodePreparator.h"

	prtx::DefaultNamePreparator namePrep;

	// create two namespaces for mesh and material names
	prtx::NamePreparator::NamespacePtr nsMesh = namePrep.newNamespace();
	prtx::NamePreparator::NamespacePtr nsMaterial = namePrep.newNamespace();

	// create the preparator
	prtx::EncodePreparatorPtr encPrep = prtx::EncodePreparator::create(true, namePrep, nsMesh, nsMaterial);

	// add any number of shapes
	encPrep->add(...);
	// ...

	// setup some simple preparation flags
	prtx::EncodePreparator::PreparationFlags flags;
	flags.instancing(false).mergeByMaterial(true).triangulate(true);

	// trigger the preparation of the shapes added so far
	prtx::EncodePreparator::InstanceVector instances;
	encPrep->fetchFinalizedInstances(instances, flags);

	// the instances are now ready for the actual encoding...
 * \endcode
 */

class PRTX_EXPORTS_API EncodePreparator {
public:
	static const prtx::DoubleVector IDENTITY;
	class FinalizedInstance;
	typedef std::vector<FinalizedInstance> InstanceVector;
	class PreparationFlags;

public:
	/**
	 * Factory member function to create an encode preparator instance. Typically, an encoder should keep one
	 * preparator instance during its life-time to perform multiple add and fetch operations.
	 *
	 * @param [in] enforceTextureConsistency Matches the materials to the corresponding meshes. If true, any textures
	 * without corresponding texture coordinates in the mesh(es) will be deleted from the corresponding materials.
	 * @param [in] namePreparator See NamePreparator
	 * @param [in] nsMeshes The namespace for mesh names.
	 * @param [in] nsMaterials The namespace for material names.
	 *
	 * @return Returns a shared pointer to the preparator instance.
	 *
	 * \sa NamePreparator
	 */
	static EncodePreparatorPtr create(
			bool 								enforceTextureConsistency,
			NamePreparator&						namePreparator,
			const NamePreparator::NamespacePtr&	nsMeshes,
			const NamePreparator::NamespacePtr&	nsMaterials,
			const MaterialTransformer*			materialTransformer = 0
	);

	/**
	 * Adds a shape to the preparator.
	 *
	 * @param cache						A valid cache. It is used to determine if the shape geometry is re-used (instanced).
	 * @param shape						The shape to add. Typically a shape created by a ShapeIterator subclass.
	 * @param initialShapeAttributes	The attributes of the corresponding initial shape.
	 * @param reports					Optional reports to process for this shape.
	 *
	 * \sa Shape
	 * \sa InitialShape
	 * \sa AttributeMap
	 * \sa MaterialTransformer
	 */
	virtual void add(
			prt::Cache*			        cache,
			const ShapePtr&				shape,
			const prt::AttributeMap*	initialShapeAttributes,
			const ReportsPtr&			reports = ReportsPtr()
	) = 0;

	/**
	 * Adds an initial shape to the preparator. This can be used to directly encode the geometry
	 * of an initial shape.
	 *
	 * @param cache					A valid cache. It is used to determine if the shape geometry is re-used (instanced).
	 * @param initialShape			The initial shape to add.
	 * @param initialShapeIndex		The current index of the initial shape in the encode sequence.
	 *
	 * \sa InitialShape
	 * \sa MaterialTransformer
	 */
	virtual void add(
			prt::Cache*			        cache,
			const prtx::InitialShape&	initialShape,
			size_t						initialShapeIndex
	) = 0;

	/**
	 * When adding an initial shape the encode preparator assigns a pseudo shape id that is then available in the finalized
	 * instance. This function returns this pseudo shape id.
	 * @param initialShapeIndex		The current index of the initial shape in question.
	 *
	 * \sa FinalizedInstance
	 */
	static uint32_t getInitialShapePseudoShapeId(size_t initialShapeIndex);

	/**
	 * Create geometry instances with corresponding transformations from the accumulated shapes and resets the preparator.
	 * Please note that a material and texture pool is kept during the whole lifetime of the preparator in order
	 * to detect identical materials and textures.
	 *
	 * Any unused vertex attribute data (e.g. unused vertex coordinates) are removed from the meshes regardless of the
	 * preparator flags.
	 *
	 * @param [out] instances Receives the instances.
	 * @param flags Preparation flags to control the preparation process.
	 * @param cbForLogStats optional pointer to a prt::SimpleOutputCallbacks instance. If provided some option-dependent stats might be written to logStats().
	 *
	 * \sa FinalizedInstance
	 * \sa prt::SimpleOutputCallbacks
	 */
	virtual void fetchFinalizedInstances(
			InstanceVector&			    instances,
			const PreparationFlags&	    flags,
			prt::SimpleOutputCallbacks* cbForLogStats = nullptr
	) = 0;

	/**
	 * Returns the world-space bounding box of the accumulated geometry.
	 * Will be reset after calling fetchFinalizedInstances.
	 *
	 * \sa prtx::BoundingBox
	 */
	virtual void getBoundingBox(prtx::BoundingBox& bbox) = 0;

	/**
	 * Returns an estimate of how much memory (in bytes) is consumed by the content added so far.
	 */
	virtual uint64_t getUsedMem(bool considerInstanceCounts = false) const = 0;

	/**
	 * Resets the preparator
	 */
	virtual void clear() = 0;

	virtual ~EncodePreparator();

protected:
	struct FinalizedInstanceBase {
		GeometryPtr             mGeometry;
		prtx::DoubleVector      mTransformation;
		prtx::MaterialPtrVector mMaterials;
		StringPtr               mInitialShapeName;
		StringPtr               mShapeName;
		uint32_t                mInitialShapeIndex;
		uint32_t                mShapeId;
		std::vector<uint32_t>   mMeshProperties;
		int32_t                 mPrototype;
		ReportsPtr              mReports;
	};

	EncodePreparator();
	EncodePreparator(const EncodePreparator&) = delete;
	EncodePreparator& operator=(const EncodePreparator&) = delete;

	static FinalizedInstanceBase& getBase(FinalizedInstance& instance);

public:
	/**
	 * Mesh property type.
	 * \sa FinalizedInstance::allMeshFacesConvex()
	 * \sa FinalizedInstance::allMeshFacesPlanar()
	 */
	enum MeshProperties {
		ALL_MESH_FACES_CONVEX = 1 << 0,	///< All faces of mesh are convex.
		ALL_MESH_FACES_PLANAR = 1 << 1	///< All faces of mesh are planar.
	};

	/**
	 * Container class for a finalized (= prepared) geometry instance.
	 */
	class PRTX_EXPORTS_API FinalizedInstance : private FinalizedInstanceBase {
	public:
		static const uint32_t NO_INITIAL_SHAPE_INDEX = uint32_t(-1);
		static const uint32_t NO_SHAPE_ID = uint32_t(-1);
		static const int32_t NO_PROTOTYPE_INDEX = -1;

	public:
		/**
		 * Returns the prepared geometry with associated meshes, materials and textures.
		 * Note that the geometry is potentially shared across multiple instances.
		 */
		const GeometryPtr& getGeometry() const;

		/**
		 * Returns the to world transformation of this instance in column-major format.
		 */
		const prtx::DoubleVector& getTransformation() const;

		/**
		 * Returns the finalized materials corresponding to the meshes in the geometry
		 * returned by getGeometry(). Note that in the case of enabled instancing,
		 * these are the final materials as specified in the rules, whereas the
		 * materials attached to the meshes are the original asset materials. There is
		 * one material for each mesh in the geometry.
		 *
		 * \sa PreparationFlags::instancing
		 */
		const prtx::MaterialPtrVector& getMaterials() const;

		/**
		 * Returns the finalized initial shape name.
		 * \sa prt::InitialShape
		 */
		const std::wstring& getInitialShapeName() const;

		/**
		 * Returns the finalized name of the shape this geometry comes from.
		 */
		const std::wstring& getShapeName() const;

		/**
		 * Returns the initial shape index.
		 */
		uint32_t getInitialShapeIndex() const;

		/**
		 * Returns the id of the shape this geometry comes from.
		 */
		uint32_t getShapeId() const;

		/**
		 * Returns the prototype index. All instances which share the same cached geometry
		 * (= backed by an URI) have the same index. Returns NO_PROTOTYPE_INDEX if the geometry is not cached.
		 *
		 * \sa prt::Cache
		 */
		int32_t getPrototypeIndex() const;

		/**
		 * Returns true if all faces of mesh at index meshIndex are convex.
		 * Note: will always return false unless PreparatorFlags::determineMeshProperties() has been set to true.
		 *
		 * \sa prtx::Geometry::getMeshes()
		 */
		bool allMeshFacesConvex(size_t meshIndex) const;

		/**
		 * Returns true if all faces of mesh at index meshIndex are planar.
		 * Note: will always return false unless PreparatorFlags::determineMeshProperties() has been set to true.
		 *
		 * \sa prtx::Geometry::getMeshes()
		 */
		bool allMeshFacesPlanar(size_t meshIndex) const;

		/**
		 * Returns the reports of the shape this instance has originated from.
		 */
		const ReportsPtr& getReports() const;

	private:
		FinalizedInstanceBase& getBase();
		friend class EncodePreparator;
	};

	/**
	 * Container class for the preparation flags. This class provides a number of chainable setters to control
	 * the preparation process.
	 *
	 * \sa prtx::EncodePreparator::fetchFinalizedInstances()
	 */
	class PRTX_EXPORTS_API PreparationFlags {
	public:
		/**
		 * Index sharing types for vertex attributes (coordinates, normals, texture corrdinates).
		 */
		enum IndexSharing {
			/**
			 * All vertex attributes have independent indexing (= the most compact representation).
			 */
			INDICES_SEPARATE_FOR_ALL_VERTEX_ATTRIBUTES				= 0,

			/**
			 * Vertex coordinates and vertex normals will use the same index. Data is replicated if needed.
			 */
			INDICES_SAME_FOR_VERTICES_AND_NORMALS					= (1 << 0),

			/**
			 * Vertex coordinates and the first texture coordinate set will use the same index. Data is replicated if needed.
			 */
			INDICES_SAME_FOR_VERTICES_AND_FIRST_UV_SET				= (1 << 1),

			/**
			 * Vertex coordinates and all texture coordinate sets will use the same index. Data is replicated if needed.
			 */
			INDICES_SAME_FOR_VERTICES_AND_ALL_UV_SETS				= (1 << 2) | INDICES_SAME_FOR_VERTICES_AND_FIRST_UV_SET,

			/**
			 * Vertex coordinates, vertex normals and the first texture coordinate set will use the same index. Data is replicated if needed.
			 */
			INDICES_SAME_FOR_VERTICES_AND_NORMALS_AND_FIRST_UV_SET	= INDICES_SAME_FOR_VERTICES_AND_NORMALS | INDICES_SAME_FOR_VERTICES_AND_FIRST_UV_SET,

			/**
			 * All vertex attributes will use the same index. Data is replicated if needed.
			 */
			INDICES_SAME_FOR_ALL_VERTEX_ATTRIBUTES					= INDICES_SAME_FOR_VERTICES_AND_NORMALS | INDICES_SAME_FOR_VERTICES_AND_ALL_UV_SETS
		};

	public:
		PreparationFlags();

		/**
		 * If true, the preparator shares/reuses cached/identical geometry objects over multiple instances.
		 */
		PreparationFlags& instancing(bool v);

		/**
		 * If true, meshes with the same material are combined per fetch call.
		 */
		PreparationFlags& mergeByMaterial(bool v);

		/**
		 * If true, textures are cut to the used area and the materials and geometries updated accordingly per fetch call.
		 * This cannot be turned off if texture atlases are activated.
		 */
		PreparationFlags& cutoutTextures(bool v);

		/**
		 * If true, texture atlases are created and the materials and geometries updated accordingly per fetch call.
		 */
		PreparationFlags& createTextureAtlases(bool v);

		/**
		 * If true, repeating textures are put into texture atlases as well.
		 */
		PreparationFlags& atlasRepeatingTextures(bool v);

		/**
		 * Set max dimension for texture atlases. v is taken to the power of 2, e.g. 11 => 2048x2048.
		 */
		PreparationFlags& maxAtlasDim(int v);

		/**
		 * If true, texture atlases are forced to have power of two dimensions.
		 */
		PreparationFlags& atlasAddWrapBorder(bool v);

		/**
		 * If true, texture atlases are forced even for single textures.
		 */
		PreparationFlags& forceAtlasing(bool v);

		/**
		 * Specifies the size to scale larger input images down to. Ignored if zero.
		 */
		PreparationFlags& maxTexSize(uint32_t v);

		/**
		 * If true, all meshes are triangulated.
		 */
		PreparationFlags& triangulate(bool v);

		/**
		 * All meshes are offset by (x,y,z).
		 */
		PreparationFlags& offset(double x, double y, double z);

		/**
		 * All meshes are offset by xyz. xyz is expected contain three double values.
		 */
		PreparationFlags& offset(const double* xyz);

		/**
		 * The specified operation is applied to all vertex normals.
		 *
		 * \sa VertexNormalProcessor
		 */
		PreparationFlags& processVertexNormals(VertexNormalProcessor::Action v);

		/**
		 * The specified action is applied to faces with holes.
		 *
		 * \sa HoleProcessor
		 */
		PreparationFlags& processHoles(HoleProcessor::Action v);

		/**
		 * If true, vertices will be merged together if they are closer than the distance
		 * specified with mergeToleranceVertices(float v).
		 */
		PreparationFlags& mergeVertices(bool v);

		/**
		 * If true, vertex normals will be merged if they are within the tolerance set by mergeToleranceNormals().
		 */
		PreparationFlags& cleanupVertexNormals(bool v);

		/**
		 * If true, texture coordinates will be merged if they are within the tolerance set by mergeToleranceUVs().
		 */
		PreparationFlags& cleanupUVs(bool v);

		/**
		 * Merge tolerance for vertex coordinates. See mergeVertices().
		 */
		PreparationFlags& mergeToleranceVertices(float v);

		/**
		 * Merge tolerance for vertex normals. See cleanupVertexNormals().
		 */
		PreparationFlags& mergeToleranceNormals(float v);

		/**
		 * Merge tolerance for texture coordinates. See cleanupUVs().
		 */
		PreparationFlags& mergeToleranceUVs(float v);

		/**
		 * Specify the index setup of the finalized meshes.
		 *
		 * \sa IndexSharing
		 */
		PreparationFlags& indexSharing(IndexSharing v);

		/**
		 * If true, performs a mesh analysis. The results can be accessed in FinalizedInstance::allMeshFacesConvex()
		 * and FinalizedInstance::allMeshFacesPlanar().
		 */
		PreparationFlags& determineMeshProperties(bool v);

		bool instancing() const;
		bool mergeByMaterial() const;
		bool cutoutTextures() const;
		bool createTextureAtlases() const;
		bool atlasRepeatingTextures() const;
		int maxAtlasDim() const;
		bool atlasAddWrapBorder() const;
		bool forceAtlasing() const;
		uint32_t maxTexSize() const;
		bool triangulate() const;
		const double* offset() const;
		VertexNormalProcessor::Action processVertexNormals() const;
		HoleProcessor::Action processHoles() const;
		bool mergeVertices() const;
		bool cleanupVertexNormals() const;
		bool cleanupUVs() const;
		float mergeToleranceVertices() const;
		float mergeToleranceNormals() const;
		float mergeToleranceUVs() const;
		IndexSharing indexSharing() const;
		bool determineMeshProperties() const;

	private:
		bool mInstancing;
		bool mMergeByMaterial;
		bool mCutoutTextures;
		bool mCreateTextureAtlases;
		bool mAtlasRepeatingTextures;
		int  mMaxAtlasDim;
		bool mAtlasAddWrapBorder;
		bool mForceAtlasing;
		uint32_t mMaxTexSize;
		bool mTriangulate;
		bool mDetermineMeshProperties;
		bool mMergeVertices;
		bool mCleanupVertexNormals;
		bool mCleanupUVs;
		VertexNormalProcessor::Action mVertexNormalAction;
		HoleProcessor::Action mHoleAction;
		IndexSharing mIndexSharing;
		float mMergeToleranceVertices;
		float mMergeToleranceNormals;
		float mMergeToleranceUVs;
		double mOffset[3];
	};
};


} // namespace prtx


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_ENCODEPREPARATOR_H_ */
