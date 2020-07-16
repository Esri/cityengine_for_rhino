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

#ifndef PRTX_SHAPEUTILS_H_
#define PRTX_SHAPEUTILS_H_

#include "prt/Callbacks.h"

#include "prtx/prtx.h"
#include "prtx/Shape.h"
#include "prtx/InitialShape.h"

#include <cassert>


namespace prtx {


/** @namespace prtx::ShapeUtils
 * @brief Utility functions for working with prtx::Shape instances.
 */
namespace ShapeUtils {


/**
 * Extracts the transformation matrix which transforms a shape's geometry to world coordinates.
 * This matrix depends on the actual geometry, the shape's attributes (scope, pivot) and the intialShape's
 * origin.
 *
 * @param      shape
 * @param[out] trafoMatrix 4x4 harmonic transformation matrix, column major (OpenGL)
 */
PRTX_EXPORTS_API void getGeometryToWorldTrafo(const Shape& shape, DoubleVector& trafoMatrix);



/**
 * Extracts the transformation matrix which transforms from a shape's pivot coordinates to world coordinates.
 * This matrix depends on the shape's attributes (pivot) and the intialShape's
 * origin.
 *
 * @param      shape
 * @param[out] trafoMatrix 4x4 harmonic transformation matrix, column major (OpenGL)
 */
PRTX_EXPORTS_API void getPivotToWorldTrafo(const Shape& shape, DoubleVector& trafoMatrix);

/**
 * Extracts the transformation matrix which transforms from object coordinates to world coordinates.
 * This matrix depends on the intialShape's origin.
 *
 * @param      shape
 * @param[out] trafoMatrix 4x4 harmonic transformation matrix, column major (OpenGL)
 */
PRTX_EXPORTS_API void getObjectToWorldTrafo(const Shape& shape, DoubleVector& trafoMatrix);


/**
 * Transforms vIn (3 components) with the transformation matrix trafo (column first).
 *
 * @param      trafo    Transformation matrix.
 * @param      vIn      vertex to transform (x,y,z)
 * @param[out] vOut     location to write transformed vertex (x,y,z)
 */
PRTX_EXPORTS_API inline void transformPoint(const prtx::DoubleVector& trafo, const double vIn[3], double vOut[3]) {
	assert(trafo.size() == 16);
	vOut[0] = trafo[0]*vIn[0] + trafo[4]*vIn[1] + trafo[ 8]*vIn[2] + trafo[12];
	vOut[1] = trafo[1]*vIn[0] + trafo[5]*vIn[1] + trafo[ 9]*vIn[2] + trafo[13];
	vOut[2] = trafo[2]*vIn[0] + trafo[6]*vIn[1] + trafo[10]*vIn[2] + trafo[14];
}


/**
 * Get the axis-aligned bounding box of a shape's geometry in world coordinates.
 *
 * @param shape          The shape.
 * @param bbMinMaxExtent Vector of doubles to receive min and max extent [minX, minY, minZ, maxX, maxY, maxZ]
 */
PRTX_EXPORTS_API void getBoundingBoxWorld(const prtx::Shape& shape, prtx::DoubleVector& bbMinMaxExtent);


/**
 * Combine a shape's material with the material of the shape's geometry.
 * The shape's material is written during cga generation. It defaults to "CityEngineMaterial".
 * As long as it isn't changed, it is overridden by it's geometry's material (e.g. if you insert [i()]
 * a textured asset, material.colormap is set to that texture).
 * As soon as it is changed (via set(material.colormap, "myPic.jpg")), the shape material "wins".
 *
 * In other words, you will get the material you see in the CityEngine renderer.
 *
 * It is not trivial, use this function.
 *
 * @param shapeMat The shape material.
 * @param meshMat  The mesh material.
 * @returns The combined material.
 */
PRTX_EXPORTS_API MaterialPtr combineMaterials(const Material& shapeMat, const Material& meshMat);


/**
 * Gets the full signature of a shape's rule, inclusive style, in the form:
 *  style$import.rule({t})		with t = type = {b|f|s}
 *
 * @param shape The shape.
 * @returns signature string.
 */
PRTX_EXPORTS_API std::wstring getStyledRuleSignature(const Shape& shape);

//
/**
 * Gets the name of a shape's rule, inclusive style, in the form:
 *  style$import.rule
 *
 * @param shape The shape.
 * @returns rule name string.
 */
PRTX_EXPORTS_API std::wstring getStyledRuleName(const Shape& shape);

/**
 * Gets the name of a shape's rule, without style, in the form:
 *  import.rule
 *
 * @param shape The shape.
 * @returns rule name string.
 */
PRTX_EXPORTS_API std::wstring getRuleName(const Shape& shape);

/**
* Gets the parameters of a shape's rule in the form:
*  ({t})   with t = type = {b|f|s}
*
* @param shape The shape.
* @returns rule parameters string.
*/
PRTX_EXPORTS_API std::wstring getRuleParameters(const Shape& shape);

/**
* Gets the actual arguments i.e. evaluated parameters of a shape's rule in the form:
*  ({evaluated parameters})
*
* @param shape The shape.
* @returns rule parameters string.
*/
PRTX_EXPORTS_API std::wstring getRuleArguments(const Shape& shape);



} // namespace ShapeUtils


} // namespace prtx


#endif /* PRTX_SHAPEUTILS_H_ */
