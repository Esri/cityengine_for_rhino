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

#ifndef PRTX_DEBUGUTILS_H_
#define PRTX_DEBUGUTILS_H_

#include "prtx/prtx.h"
#include <ostream>
#include <memory>


namespace prtx {

class Attributable;
class Mesh;
class Geometry;
class Material;
class Shader;
class Texture;
class TrimPlane;
class BoundingBox;
class Reports;
class GenerateContext;

typedef std::shared_ptr<Geometry> GeometryPtr;
typedef std::shared_ptr<Reports> ReportsPtr;


/**
 * @namespace prtx::DebugUtils
 *
 * @brief Debug utilities for various prtx classes. Useful for prtx extension authors.
 */
namespace DebugUtils {


PRTX_EXPORTS_API void dump(std::wostream& stream, const Attributable& a);


/**
 * Dumps string representation of Geometry to stream.
 */
PRTX_EXPORTS_API void dump(std::wostream& stream, const Geometry& g);

/**
 * Dumps string representation of Mesh to stream.
 */
PRTX_EXPORTS_API void dump(std::wostream& stream, const Mesh& mesh);

/**
 * Dumps string representation of Material to stream.
 */
PRTX_EXPORTS_API void dump(std::wostream& stream, const Material& m);

/**
 * Dumps string representation of Shader to stream.
 */
PRTX_EXPORTS_API void dump(std::wostream& stream, const Shader& m);

/**
 * Dumps string representation of Texture to stream.
 */
PRTX_EXPORTS_API void dump(std::wostream& stream, const Texture& m);

/**
 * Dumps string representation of TrimPlane to stream.
 */
PRTX_EXPORTS_API void dump(std::wostream& stream, const TrimPlane& m);

PRTX_EXPORTS_API void dump(prtx::GenerateContext& context, size_t initialShapeIndex, const std::wstring& pathToDotFile);

/**
 * Dumps the current state of BoundingBox b to narrow stream.
 */
PRTX_EXPORTS_API std::ostream& operator<<(std::ostream& stream, const BoundingBox& b);

/**
 * Dumps the current state of BoundingBox b to wide stream.
 */
PRTX_EXPORTS_API std::wostream& operator<<(std::wostream& stream, const BoundingBox& b);

/**
 * Dumps a Reports object to a wide character stream
 */
PRTX_EXPORTS_API std::wostream& operator<<(std::wostream& stream, const prtx::ReportsPtr& r);

/**
 * Creates a box geometry from a bounding box instance.
 */
PRTX_EXPORTS_API prtx::GeometryPtr toGeometry(const prtx::BoundingBox& bbox, const std::wstring& geoName = std::wstring());


} // namespace DebugUtils


} // namespace prtx


#endif /* PRTX_DEBUGUTILS_H_ */
