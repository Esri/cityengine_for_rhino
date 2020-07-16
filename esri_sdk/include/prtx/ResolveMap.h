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

#ifndef PRTX_RESOLVEMAP_H_
#define PRTX_RESOLVEMAP_H_

#include "prtx/prtx.h"
#include "prtx/URI.h"
#include "prt/ResolveMap.h"

#include <string>

#ifdef __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif


namespace prtx {


/**
 * The prtx specialization of the prt::ResolveMap adds convenience functions to directly support
 * STL strings and URIs.
 */
class PRTX_EXPORTS_API ResolveMap : public prt::ResolveMap {
public:
	ResolveMap() {}

	/**
	 * returns empty URI if key could not be resolved
	 */
	virtual prtx::URIPtr resolveKey(const std::wstring& key) const = 0;


	/**
	 * Performs a reverse lookup and returns all keys which map to a given uri.
	 */
	virtual std::vector<std::wstring> resolveURI(const prtx::URIPtr& uri) const = 0;


	/**
	 * Searches the keys in the ResolveMap for the expression defined in query.
	 * Supported search queries include wildcards, regular expressions and file properties.
	 *
	 * This implements the CGA @cgaref{func_fileSearch.html,fileSearch()} function.
	 * This function is thread-safe.
	 *
	 * @param         project     The name of the current project ("projects" are "subfolders" in the resolve map starting with a '/').
	 * @param         query			The search query. See the CGA @cgaref{func_fileSearch.html,fileSearch()}
	 * 								function for syntax documentation.
	 *
	 * @returns result.
	 */
	virtual std::wstring searchKey(const std::wstring& project, const std::wstring& query) const = 0;

	/**
	 * Helper function to resolve a key with fall back to URI = key if key not found in ResolveMap.
	 * @param resolveMap The resolveMap to use.
	 * @param key        The key to resolve. If fallback to URI is used, key must be percent encoded.
	 * @returns the resolved URI if key can be resolved or a URI with the key else.
	 */
	static prtx::URIPtr resolveKeyWithURIFallback(ResolveMap const* resolveMap, const std::wstring& key);

	/**
	 * Helper function to replace the last segment in a key. Keys are often separated by '/' characters,
	 * for instance 'assets/elements/window.obj', and relative keys need to be constructed, e.g.
	 * 'assets/elements/glass.jpg'.
	 * @param key        The key whose last '/' separated segment needs to be replaced.
	 * @param newSegment The new segment.
	 * @returns key with the last segment replaced.
	 */
	static std::wstring replaceLastKeySegment(const std::wstring& key, const std::wstring& newSegment);

protected:
	virtual ~ResolveMap() {}

};


} // namespace prtx

#ifdef __clang__
#	pragma clang diagnostic pop
#endif


#endif /* PRTX_RESOLVEMAP_H_ */
