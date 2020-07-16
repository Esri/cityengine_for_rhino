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

#ifndef PRTX_INITIALSHAPE_H_
#define PRTX_INITIALSHAPE_H_

#include "prt/InitialShape.h"
#include "prtx/ResolveMap.h"

namespace prtx {

class Geometry;
using GeometryPtr = std::shared_ptr<Geometry>;

class PRT_EXPORTS_API InitialShape : public prt::InitialShape {
public:
	virtual GeometryPtr             getGeometry() const = 0;
	virtual const std::wstring&     getWarnings() const = 0;
	virtual const prtx::ResolveMap* getPRTXResolveMap() const = 0;

protected:
	InitialShape() {}
	virtual ~InitialShape() {}
};

}

#endif /* PRTX_INITIALSHAPE_H_ */
