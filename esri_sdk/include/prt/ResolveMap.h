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

#ifndef PRT_RESOLVEMAP_H_
#define PRT_RESOLVEMAP_H_

#include "Attributable.h"
#include "Object.h"


namespace prt {


/**
 * A ResolveMap maps an arbitrary string key to an URI. Resolve maps are used for
 * asset lookups during CGA generation (e.g. @cgaref{op_i.html,i()} and
 * @cgaref{op_texture.html,texture()} operations). Use the getString() member
 * function derived from the Attributable interface to lookup a key.
 * ResolveMaps can be stacked, i.e. a map can be put in place downstream.
 * @sa ResolveMapBuilder, prtx::ResolveMap
 */

class PRT_EXPORTS_API ResolveMap: public Attributable, public Object {
public:
	/**
	 * Searches the keys in the ResolveMap for the expression defined in query.
	 * Supported search queries include wildcards, regular expressions and file properties.
	 *
	 * This implements the CGA @cgaref{func_fileSearch.html,fileSearch()} function.
	 * This function is thread-safe.
	 * If the result does not fit into the allocated buffer, the result is truncated.
	 * The INOUT parameter resultSize must be compared to the original buffer size
	 * after the call to detect this situation.
	 *
	 *
	 * @param         project		The name of the current project ("projects" are "subfolders"
	 * 								in the resolve map starting with a '/').
	 * @param         query			The search query. See the CGA @cgaref{func_fileSearch.html,fileSearch()}
	 * 								function for syntax documentation.
	 *
	 * @param[in,out] result		Pointer to buffer to receive a string which contains the semicolon
	 *                              separated list of keys
	 * @param[in,out] resultSize	Pointer to size of reserved buffer; receives the size of
	 * 								the actual result (incl. terminating 0). Must be >= 1.
     * @param[out]    stat			Optional pointer to return the status.
	 * @returns result.
	 */
	virtual wchar_t* searchKey(const wchar_t* project, const wchar_t* query, wchar_t* result, size_t* resultSize, Status* stat = 0) const = 0;

protected:
	ResolveMap();
	virtual ~ResolveMap();
};


/**
 * Used for building a ResolveMap instance.
 */
class PRT_EXPORTS_API ResolveMapBuilder : public Object {
public:
	/**
	 * Creates a new instance which must be destroyed by the caller.
     * @param[out] status Optional pointer to return the status.
     * @returns a new ResolveMapBuilder instance or 0 on error.
	 */
	static ResolveMapBuilder* create(Status* status = 0);

	/**
	 * Creates a new builder instance which is initialized to the passed ResolveMap. The builder instance must be destroyed by the caller.
	 # @param      resolveMap ResolveMap to initialize this builder with.
     * @param[out] status     Optional pointer to return the status.
     * @returns a new ResolveMapBuilder instance or 0 on error.
	 */
	static ResolveMapBuilder* createFromResolveMap(ResolveMap const* resolveMap, Status* status = 0);

	/**
	 * Creates a new ResolveMap instance. The instnace must be destoyed by the caller.
     * @param[out] status Optional pointer to return the status.
     * @returns a new ResolveMap instance or 0 on error.
	 */
	virtual ResolveMap const* createResolveMap(Status* status = 0) const = 0;

	/**
	 * Creates a new ResolveMap instance and resets the builder. The instnace must be destoyed by the caller.
     * @param[out] status Optional pointer to return the status.
     * @returns a new ResolveMap instance or 0 on error.
	 */
	virtual ResolveMap const* createResolveMapAndReset(Status* status = 0) = 0;

	/**
	 * Adds a (key, URI) entry.
	 * @param key The (unique) key string.
	 * @param uri The uri.
	 * @returns status.
	 */
	virtual Status addEntry(const wchar_t* key, const wchar_t *uri) = 0;

	/**
	 * Puts a ResolveMap in place downstream. In key lookups, the lookup propagates to the downstream map
	 * if it is not found in the current map.
	 * @param map The downstream ResolveMap to set.
	 * @returns status.
	 */
	virtual Status setDownstreamMap(const ResolveMap* map) = 0;

protected:
	ResolveMapBuilder();
	virtual ~ResolveMapBuilder();
};


} // namespace prt


#endif /* PRT_RESOLVEMAP_H_ */
