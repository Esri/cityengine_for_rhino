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

#ifndef PRTX_GEOMETRY_H_
#define PRTX_GEOMETRY_H_

#include "prtx/prtx.h"
#include "prtx/Builder.h"
#include "prtx/URI.h"
#include "prtx/Content.h"
#include "prtx/Attributable.h"

#include <vector>
#include <memory>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4231 4251 4275 4660)
#endif


class AttributablePayload;


namespace prtx {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;
using MeshPtrVector = std::vector<MeshPtr>;

class BoundingBox;
class Geometry;
typedef std::shared_ptr<Geometry> GeometryPtr;		///< shared Geometry pointer
typedef std::vector<GeometryPtr>  GeometryPtrVector;		///< vector of shared Geometry pointers

/**
 * Read-only container for polygon meshes. A Geometry has an URI which is either:
 *  * empty / invalid: the Geometry is a transient result of the generate() process
 *  * valid:           the URI uniquely identifies this geometry which originates from an external asset (file).
 *
 *  The coordinate system of the Geometry is arbitrary.
 *
 *  @sa DataBackend, GeometryBuilder, Mesh
 */
class PRTX_EXPORTS_API Geometry : public Content, public Attributable {

public:
	virtual ~Geometry() {}

	/**
	 * @returns the URI which is either empty or uniquely identifies the external asset (file).
	 */
	URIPtr getURI() const {
		return mURI;
	}

	/**
	 * @returns a vector with all meshes.
	 */
	const MeshPtrVector& getMeshes() const {
		return mMeshes;
	}

	/**
	 * @returns axis-aligned BoundingBox.
	 */
	virtual const BoundingBox& getBoundingBox() const = 0;

protected:
	// @cond
	Geometry(const prtx::URIPtr& uri) : mURI(uri) {}
	Geometry(const prtx::URIPtr& uri, const MeshPtrVector& meshes);

	URIPtr			mURI;
	MeshPtrVector	mMeshes;
	// @endcond
};


/**
 * Builder to create (immutable) Geometry instances.
 */
class PRTX_EXPORTS_API GeometryBuilder : public SharedPtrBuilder<Geometry>, public AttributableSetter {
public:
	/**
	 * Constructs an empty GeometryBuilder.
	 */
	GeometryBuilder();

	/**
	 * Constructs a GeometryBuilder initialized to the passed geometry.
	 * @param geometry Geometry to initialize builder with.
	 */
	GeometryBuilder(const Geometry& geometry);

	virtual ~GeometryBuilder();

	// SharedPtrBuilder interface implementation
	/**
	 * Creates a geometry instance. Throws an exception if requirements are not met.
	 * @param[out] warnings Optional pointer to return warnings.
	 * @return shared pointer to new Geometry instance.
	 */
	virtual GeometryPtr createShared(std::wstring* warnings = 0) const;
	/**
	 * Creates a geometry instance and resets the builder's state.
	 * Throws an exception if requirements are not met.
	 * @param[out] warnings Optional pointer to return warnings.
	 * @return shared pointer to new Geometry instance.
	 */
	virtual GeometryPtr createSharedAndReset(std::wstring* warnings = 0);


	/**
	 * Sets the URI for the Geometry.
	 * @param uri URI for the geometry.
	 */
	void setURI(const URIPtr& uri);

	/**
	 * Adds a mesh to the builder.
	 * @param mesh Mesh to add.
	 * @returns index of newly inserted mesh in internal vector.
	 */
	uint32_t addMesh(const MeshPtr& mesh);

	/**
	 * Adds a vector of meshes to the builder.
	 * @param meshes Meshes to add.
	 */
	void addMeshes(const MeshPtrVector& meshes);

	/**@{
	 * prtx::AttributableSetter Implementation
	 */
	virtual void setBool		(const std::wstring& key, Bool					val);
	virtual void setInt			(const std::wstring& key, int32_t				val);
	virtual void setFloat		(const std::wstring& key, double				val);
	virtual void setString		(const std::wstring& key, const std::wstring&	val);
	virtual void setBoolArray	(const std::wstring& key, const BoolVector&		val);
	virtual void setIntArray	(const std::wstring& key, const Int32Vector&	val);
	virtual void setFloatArray	(const std::wstring& key, const DoubleVector&	val);
	virtual void setStringArray	(const std::wstring& key, const WStringVector&	val);
	virtual void setBlindData	(const std::wstring& key, void*					val);
	/**@}*/

private:
	URIPtr								mURI;
	MeshPtrVector						mMeshes;

	AttributablePayload*				mAttributes;
};


} /* namespace prtx */



#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#endif /* PRTX_GEOMETRY_H_ */
