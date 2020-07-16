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

#ifndef PRTX_TRIMPLANE_H_
#define PRTX_TRIMPLANE_H_

#include "prtx/Geometry.h"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4251 4231 4275 4660)
#endif

#include <memory>


namespace prtx {


class TrimPlane;
typedef std::shared_ptr<TrimPlane> TrimPlanePtr;			///< shared TrimPlane pointer
typedef std::vector<TrimPlanePtr>  TrimPlanePtrVector;       ///< vector of shared TrimPlane pointers

/**
 * Provides read-only access to the trim planes generated during CGA generate().
 * Trim planes are in the Shape's pivot coordinate system.
 *
 * @sa ShapeUtils::getPivotToWorldTrafo()
 *
 */
class PRTX_EXPORTS_API TrimPlane {
public:
	TrimPlane(const TrimPlane&) = delete;
	TrimPlane& operator=(const TrimPlane&) = delete;
	virtual ~TrimPlane();

	/**
	 * See the trim shape attribute in the CGA ref.
	 * @returns true if the orientation of the TrimPlane is horizontal.
	 */
	bool isHorizontal() const;
	/**
	 * See the trim shape attribute in the CGA ref.
	 * @returns true if the orientation of the TrimPlane is vertical.
	 */
	bool isVertical()   const;
	/**
	 * Trim planes can be limited or unlimited.
	 * A trim plane is limited on the side where the end point of the original
	 * edge is a concave polygon vertex.
	 * @ returns true if the TrimPlane is limited on one or both sides.
	 */
	bool isLimited()    const;

	/**
	 * @returns the original edge in pivot coordinates: [startx, starty, startz, endx, endy, endz]
	 */
	const DoubleVector& getOrigEdgeCoords() const;
	/**
	 * Note that the size of the plane depends on the "/cgaenv/trimPlanesComputeSize" setting,
	 * see prt::InitialShapeBuilder::setAttributes().
	 * @returns the plane which was constructed during generate(), in pivot coordinates.
	 */
	MeshPtr getMesh() const;

protected:
	struct Payload;
	Payload* mPayload;

	TrimPlane(Payload* payload);
};


} /* namespace prtx */


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_TRIMPLANE_H_ */
