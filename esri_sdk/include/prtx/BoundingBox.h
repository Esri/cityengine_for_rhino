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

#ifndef PRTX_BOUNDINGBOX_H_
#define PRTX_BOUNDINGBOX_H_

#include "prtx/prtx.h"
#include "prtx/Types.h"


#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#	pragma warning (disable : 4275) // non DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#endif


namespace prtx {


/**
 * Simple AABB.
 */
class PRTX_EXPORTS_API BoundingBox {
public:

	/**
	 * Constructs an "empty" bounding box with infinite bounds.
	 */
	BoundingBox();

	/**
	 * Constructs bounding box with preset bounds.
	 */
	BoundingBox(const DoubleVector& extMin, const DoubleVector& extMax);

	/**
	 * Constructs bounding box with preset bounds.
	 */
	BoundingBox(const double* extMin, const double* extMax);

	virtual ~BoundingBox();

	/**
	 * Reset to infinite bounds.
	 */
	void reset();

	/**
	 * Extend with another bounding box.
	 */
	void add(const BoundingBox& bbox);

	/**
	 * Extend bounding box to point (x,y,z).
	 */
	void add(const double& x, const double& y, const double& z);

	/**
	 * Translates bounding box by vector t, t must have 3 elements.
	 */
	void translate(const DoubleVector& t);

	/**
	 * @returns A reference to the min point.
	 */
	const DoubleVector& getMin() const;

	/**
	 * @returns A reference to the max point.
	 */
	const DoubleVector& getMax() const;

	/**
	 * @returns A concatenated copy of both getMin() and getMax().
	 */
	DoubleVector getMinMax() const;

	/**
	 * @returns The center point (3 elements).
	 */
	DoubleVector getCenter() const;

	/**
	 * @returns The "bottom" center point. Bottom is relative to y-direction.
	 */
	DoubleVector getCenterBottom() const;

	/**
	 * Resizes s to 3 elements and returns the size of the box.
	 */
	void getSize(prtx::DoubleVector& s) const;

	/**
	 * Bounding box diameter.
	 */
	double getDiameter() const;

	/**
	 * @returns True, if anything as been added to the bounding box. False, if bounding box is "empty", i.e. has infinite bounds.
	 */
	bool isModified() const;

protected:
	DoubleVector mExtMin, mExtMax;
};


} /* namespace prtx */


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_BOUNDINGBOX_H_ */
