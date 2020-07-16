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

#ifndef PRTX_DATABACKEND_H_
#define PRTX_DATABACKEND_H_

#include "prtx/prtx.h"
#include "prtx/Types.h"
#include "prtx/URI.h"
#include "prtx/Content.h"
#include "prtx/Texture.h"
#include "prtx/Geometry.h"
#include "prtx/CGB.h"
#include "prtx/ResolveMap.h"
#include "prtx/Table.h"

#include "prt/Status.h"
#include "prt/Cache.h"

#include <string>
#include <memory>


namespace prtx {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;
using MeshPtrVector = std::vector<MeshPtr>;

class Material;
using MaterialPtr = std::shared_ptr<Material>;
using MaterialPtrVector = std::vector<MaterialPtr>;

using BinaryVectorPtr = std::shared_ptr<BinaryVector>;

class CTString;
using CTStringPtr = std::shared_ptr<CTString>;

/**
 * @namespace prtx::DataBackend
 *
 * @brief Provides a number of functions to resolve symbolic names to actual resources.
 * 
 * This includes the steps: (1) resolve the key to an URI (2) load content pointed at by the URI and (3) decode the content.
 *
 * All calls are thread-safe.
 *
 */
namespace DataBackend {


/**
 * Resolve a CGB.
 * @param cache        Cache to use. Set to nullptr for no cache.
 * @param key          Key (i.e. symbolic name) into resolveMap.
 * @param resolveMap   ResolveMap for key-to-URI lookup. If resolveMap is nullptr or key is not found,
 *                     it is tried to interpret the key as an URI.
 * @returns valid CGBPtr or throws an exception.
 */
PRTX_EXPORTS_API CGBPtr resolveCGB(prt::Cache* cache, const std::wstring& key, const prtx::ResolveMap* resolveMap = nullptr);

/**
 * Resolve a geometry.
 * @param      cache        Cache to use. Set to nullptr for no cache.
 * @param      key          Key (i.e. symbolic name) into resolveMap.
 * @param      resolveMap   ResolveMap for key-to-URI lookup. If resolveMap is nullptr or key is not found,
 *                          the function tries to interpret the key as an URI.
 * @param[out] warnings     Optional pointer to a wstring to add warnings.
 * @returns valid GeometryPtr or throws an exception.
 */
PRTX_EXPORTS_API GeometryPtr resolveGeometry(prt::Cache* cache, const std::wstring& key, const prtx::ResolveMap* resolveMap = nullptr, std::wstring* warnings = nullptr);

/**
 * Resolves materials
 * @param cache          Cache to use. Set to nullptr for no cache.
 * @param materials      [OUT] Vector to which 0, 1 or several MaterialPtrs are added.
 * @param key            Key (i.e. symbolic name) into resolveMap.
 * @param resolveMap     ResolveMap for key-to-URI lookup. If resolveMap is nullptr or key is not found,
 *                       it is tried to interpret the key as an URI.
 * @param[out] warnings  Optional pointer to a wstring to add warnings.
 */
PRTX_EXPORTS_API void resolveMaterial(prt::Cache* cache, MaterialPtrVector& materials, const std::wstring& key, const prtx::ResolveMap* resolveMap = nullptr, std::wstring* warnings = nullptr);

/**
 * Resolve a texture.
 * @param      cache        Cache to use. Set to nullptr for no cache.
 * @param      key          Key (i.e. symbolic name) into resolveMap.
 * @param      resolveMap   ResolveMap for key-to-URI lookup. If resolveMap is nullptr or key is not found,
 *                          it is tried to interpret the key as an URI.
 * @param[out] warnings     Optional pointer to a wstring to add warnings.
 * @returns valid TexturePtr or throws an exception.
 */
PRTX_EXPORTS_API TexturePtr resolveTexture(prt::Cache* cache, const std::wstring& key, const prtx::ResolveMap* resolveMap = nullptr, std::wstring* warnings = nullptr);


/**
 * Resolves (reads) generic binary data from an URI (or optionally via key and resolveMap).
 * @param      cache        Cache to use. Set to nullptr for no cache.
 * @param      key          Key (i.e. symbolic name) into resolveMap.
 * @param      resolveMap   ResolveMap for key-to-URI lookup. If resolveMap is nullptr or key is not found,
 *                          the key will be interpreted as an URI.
 * @param[out] warnings     Optional pointer to a std::wstring to add warnings.
 * @returns valid BinaryVectorPtr or throws an exception.
  */
PRTX_EXPORTS_API BinaryVectorPtr resolveBinaryData(prt::Cache* cache, const std::wstring& key, const prtx::ResolveMap* resolveMap = nullptr, std::wstring* warnings = nullptr);

/**
 * Resolves text.
 * @param      cache        Cache to use. Set to nullptr for no cache.
 * @param      key          Key (i.e. symbolic name) into resolveMap.
 * @param      resolveMap   ResolveMap for key-to-URI lookup. If resolveMap is nullptr or key is not found,
 *                          the key will be interpreted as an URI.
 * @param[out] warnings     Optional pointer to a std::wstring to add warnings.
 * @returns valid BinaryVectorPtr or throws an exception.
  */
PRTX_EXPORTS_API CTStringPtr resolveText(prt::Cache* cache, const std::wstring& key, const prtx::ResolveMap* resolveMap = nullptr, std::wstring* warnings = nullptr);

/**
 * Resolves table.
 * @param      cache        Cache to use. Set to nullptr for no cache.
 * @param      key          Key (i.e. symbolic name) into resolveMap.
 * @param      resolveMap   ResolveMap for key-to-URI lookup. If resolveMap is nullptr or key is not found,
 *                          the key will be interpreted as an URI.
 * @param[out] warnings     Optional pointer to a std::wstring to add warnings.
 * @returns valid BinaryVectorPtr or throws an exception.
  */
PRTX_EXPORTS_API TablePtr resolveTable(prt::Cache* cache, const std::wstring& key, const prtx::ResolveMap* resolveMap = nullptr, std::wstring* warnings = nullptr);


/**
 * Checks whether the passed uri is in the passed cache.
 * @param cache Cache to use.
 * @param ct    Content type of resource.
 * @param uri   URI to query.
 * @returns true if the resource described by uri and ct is in the cache, false otherwise.
 */
PRTX_EXPORTS_API bool isCached(prt::Cache* cache, prt::ContentType ct, const prtx::URIPtr& uri);

/**
 * [DEPRECATED, use resolveBinaryData()] Reads and returns the content pointed at by the URI.
 * Throws an exception on error.
 * @param      uri  URI to read from.
 * @param[out] data Vector to return a copy of the content.
 */
[[deprecated]] PRTX_EXPORTS_API void readURI(const prtx::URIPtr& uri, BinaryVector& data);


} // namespace DataBackend


} // namespace prtx


#endif /* PRTX_DATABACKEND_H_ */
