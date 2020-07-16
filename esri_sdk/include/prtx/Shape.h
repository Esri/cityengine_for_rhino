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

#ifndef PRTX_SHAPE_H_
#define PRTX_SHAPE_H_

#include "prtx/prtx.h"
#include "prtx/Attributable.h"
#include "prtx/Material.h"
#include "prtx/CGAError.h"
#include "prtx/TrimPlane.h"
#include "prtx/generated/BuiltinShapeAttributes.h"

#include <memory>


namespace prtx {

class Geometry;
using GeometryPtr = std::shared_ptr<Geometry>;

class Shape;

typedef std::shared_ptr<Shape> ShapePtr;			///< shared pointer to a Shape
typedef std::vector<ShapePtr>  ShapePtrVector;		///< vector of shared pointers to Shape


/**
 * The Shape class represents a node of the shape tree. A cga rule is applied on a shape, resulting in a number
 * of new shapes, the successors (which have the original shape set as their parent). Each shape has a unique ID,
 * and the tree structure is encoded in the parent ID of the shapes.
 *
 * Shape attributes can be read either through the Attributable interface (via their cga name, e.g. L"scope.sx" or
 * L"material.colormap") or through the dedicated getters in the BuiltinShapeAttributes interface (e.g. scope_sx() or material_colormap()).
 * If possible use the latter because they are faster.
 *
 * Generic CGA attributes can be read through the Attributable interface.
 *
 * The built-ins CGA attribute keys are:
*  * bool:
*    * trim.horizontal
*    * trim.vertical
*  * float:
*    * comp.index
*    * comp.total
*    * initialShape.origin.ox
*    * initialShape.origin.oy
*    * initialShape.origin.oz
*    * initialShape.origin.px
*    * initialShape.origin.py
*    * initialShape.origin.pz
*    * material.ambient.b
*    * material.ambient.g
*    * material.ambient.r
*    * material.bumpValue
*    * material.bumpmap.rw
*    * material.bumpmap.su
*    * material.bumpmap.sv
*    * material.bumpmap.tu
*    * material.bumpmap.tv
*    * material.color.a
*    * material.color.b
*    * material.color.g
*    * material.color.r
*    * material.colormap.rw
*    * material.colormap.su
*    * material.colormap.sv
*    * material.colormap.tu
*    * material.colormap.tv
*    * material.dirtmap.rw
*    * material.dirtmap.su
*    * material.dirtmap.sv
*    * material.dirtmap.tu
*    * material.dirtmap.tv
*    * material.emissive.b
*    * material.emissive.g
*    * material.emissive.r
*    * material.emissivemap.rw
*    * material.emissivemap.su
*    * material.emissivemap.sv
*    * material.emissivemap.tu
*    * material.emissivemap.tv
*    * material.metallic
*    * material.metallicmap.rw
*    * material.metallicmap.su
*    * material.metallicmap.sv
*    * material.metallicmap.tu
*    * material.metallicmap.tv
*    * material.normalmap.rw
*    * material.normalmap.su
*    * material.normalmap.sv
*    * material.normalmap.tu
*    * material.normalmap.tv
*    * material.occlusionmap.rw
*    * material.occlusionmap.su
*    * material.occlusionmap.sv
*    * material.occlusionmap.tu
*    * material.occlusionmap.tv
*    * material.opacity
*    * material.opacitymap.cutoff
*    * material.opacitymap.rw
*    * material.opacitymap.su
*    * material.opacitymap.sv
*    * material.opacitymap.tu
*    * material.opacitymap.tv
*    * material.reflectivity
*    * material.roughness
*    * material.roughnessmap.rw
*    * material.roughnessmap.su
*    * material.roughnessmap.sv
*    * material.roughnessmap.tu
*    * material.roughnessmap.tv
*    * material.shininess
*    * material.specular.b
*    * material.specular.g
*    * material.specular.r
*    * material.specularmap.rw
*    * material.specularmap.su
*    * material.specularmap.sv
*    * material.specularmap.tu
*    * material.specularmap.tv
*    * pivot.ox
*    * pivot.oy
*    * pivot.oz
*    * pivot.px
*    * pivot.py
*    * pivot.pz
*    * scope.rx
*    * scope.ry
*    * scope.rz
*    * scope.sx
*    * scope.sy
*    * scope.sz
*    * scope.tx
*    * scope.ty
*    * scope.tz
*    * seedian
*    * split.index
*    * split.total
*    * uid
*  * string:
*    * comp.sel
*    * initialShape.name
*    * initialShape.startRule
*    * material.bumpmap
*    * material.color.rgb
*    * material.colormap
*    * material.dirtmap
*    * material.emissivemap
*    * material.metallicmap
*    * material.name
*    * material.normalmap
*    * material.occlusionmap
*    * material.opacitymap
*    * material.opacitymap.mode
*    * material.roughnessmap
*    * material.shader
*    * material.specularmap
*
* There are also the intrinsic keys:
*  * int:
*    * .ID
*    * .parentID
*    * .shapeSymbol
*
 */
class PRTX_EXPORTS_API Shape : public Attributable, public BuiltinShapeAttributes {
public:
	// --- prtx::Attributable

	// --- BuiltinShapeAttributes : all accessible through Attributable interface
	// with cga-name

	// --- non-cga shape attributes => simple types accessible through SPIIAttributable interface,
	// with "." prefix

	/**
	 * @returns the unique, nonzero ID of the shape.
	 */
	virtual uint32_t					   getID() const = 0;
	/**
	 * @returns the ID of the parent shape, 0 for the root node.
	 */
	virtual uint32_t					   getParentID() const = 0;
	/**
	 * Returns the shape's geometry. The coordinate system is arbitrary; in order to transform the geometry
	 * into the world coordinate system, the bounding box of the geometry, the shape's pivot and scope and the
	 * initial shape's origin need to be taken in account.
	 * @sa ShapeUtils::getGeometryToWorldTrafo()
	 * @returns the shape's geometry.
	 */
	virtual GeometryPtr					   getGeometry() const = 0;
	/**
	 * Returns the shape's material. This is the material which can be manipulated within CGA rules.
	 * Note that the shape's geometry also has a Material. In order to find the "final Material", the two materials
	 * need to be combined as follows: (1) take the geometry's material (2) every attribute which got set during generate():
	 * override the value with the ine from the shape's material.
	 * @sa ShapeUtils::combineMaterials()
	 * @returns  the shape's material.
	 */
	virtual MaterialPtr					   getMaterial() const = 0;

	/**
	 * @returns a vector with all CGAErrors which occurred during generate().
	 */
	virtual const CGAErrorPtrVector&       getCGAErrors() const = 0;
	/**
	 * @returns a string which is a concatenation of all print() operations and functions which occurred during generate().
	 */
	virtual const std::wstring&			   getCGAPrint()  const = 0;

	// reporting
	typedef std::pair<StringPtr, bool>      ReportBool;
	typedef std::pair<StringPtr, double>    ReportFloat;
	typedef std::pair<StringPtr, StringPtr> ReportString;
	typedef std::vector<ReportBool>   		ReportBoolVect;
	typedef std::vector<ReportFloat>  		ReportFloatVect;
	typedef std::vector<ReportString> 		ReportStringVect;

	/**
	 * @returns a vector with all boolean reports from the report() operation which occured during generate().
	 */
	virtual const ReportBoolVect&   getCGAReportsBool()   const = 0;
	/**
	 * @returns a vector with all float reports from the report() operation which occured during generate().
	 */
	virtual const ReportFloatVect&  getCGAReportsFloat()  const = 0;
	/**
	 * @returns a vector with all string reports from the report() operation which occurred during generate().
	 */
	virtual const ReportStringVect& getCGAReportsString() const = 0;


	/**
	 * Every shape has a shape symbol which correspondents to a cga rule in the initial shape's rule file.
	 * Use the functions in ShapeUtils to turn it into human readable strings.
	 * @sa ShapeUtils::getRuleArguments(), ShapeUtils::getRuleName(), ShapeUtils::getRuleParameters(),
	 * ShapeUtils::getStyledRuleName(), ShapeUtils::getStyledRuleSignature()
	 * @returns the shape's shape symbol.
	 */
	virtual int32_t			          getShapeSymbol() const = 0;
	/**
	 * Note: trim planes are in pivot coordinates. In order to transform the geometry
	 * into the world coordinate system, the bounding box of the geometry, the shape's pivot and the initial
	 * shape's origin need to be taken in account.
	 * @sa ShapeUtils::getPivotToWorldTrafo
	 * @returns a vector with all trim planes.
	 */
	virtual const TrimPlanePtrVector& getTrimPlanes()  const = 0;


protected:
	Shape() {}
	virtual ~Shape() {}
};


} // namespace prtx


#endif /* SPI_SHAPE_H_ */
