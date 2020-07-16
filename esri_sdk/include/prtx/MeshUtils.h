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

#ifndef PRTX_MESHUTILS_H_
#define PRTX_MESHUTILS_H_

#include "prtx/Mesh.h"


namespace prtx {


/** @namespace prtx::MeshUtils
 * @brief Utility functions for prtx::Mesh.
 */
namespace MeshUtils {


/**
 * Transforms a number of vertices (i.e. (x, y, z) coordinate triples) with a transformation matrix.
 * @param[in, out] coords Vertices to transform. Size must be multiple of 3 (3 * number of vertices).
 * @param          matrix Matrix to transform vertices with. 4x4 harmonic transformation matrix, column major (OpenGL).
 */
PRTX_EXPORTS_API void transformCoords(DoubleVector& coords, const DoubleVector& matrix);

/**
 * Creates a triangulated version of the passed Mesh.
 * @param mesh             The mesh to trriangulate.
 * @param triangle2FaceMap Optional pointer to index vector to return the mapping of the new triangles (faces) to the old faces.
 */
PRTX_EXPORTS_API MeshPtr createTriangulatedMesh(const MeshPtr& mesh, IndexVector* triangle2FaceMap = 0);


} // namespace MeshUtils


} // namespace prtx


#endif /* PRTX_MESHUTILS_H_ */
