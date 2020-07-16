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

#ifndef PRTX_RESOLVEMAP_PROVIDER_H_
#define PRTX_RESOLVEMAP_PROVIDER_H_

#include "prt/ContentType.h"
#include "prt/Cache.h"

#include "prtx/prtx.h"
#include "prtx/Content.h"
#include "prtx/URI.h"
#include "prtx/Extension.h"
#include "prtx/ResolveMap.h"

#include <string>
#include <iosfwd>
#include <memory>


namespace prtx {


class ResolveMapProvider;
using ResolveMapProviderPtr = std::shared_ptr<ResolveMapProvider>;

/**
 * Base class for all PRTX extensions which can resolve a prtx::URI to a
 * prtx::Content based object.
 */
class PRTX_EXPORTS_API ResolveMapProvider : public Extension {
protected:
	ResolveMapProvider() = default;

public:
	virtual ~ResolveMapProvider() = default;

	/**
	 * Scans the resource tree specified by uri and populates a resolve map with
	 * the found keys and URIs.
	 *
	 * @returns Returns a new resolve map which must be destroyed by the caller.
	 *
	 * \sa prt::ResolveMap
	 */
	virtual prt::ResolveMap const* createResolveMap(
			prtx::URIPtr uri ///< The root of the resource tree.
	) const = 0;

	/**
	 * Fixates the extension type to ET_RESOLVEMAP_PROVIDER.
	 *
	 * @return Always returns prtx::Extension::ET_RESOLVEMAP_PROVIDER.
	 *
	 * \sa prtx::Extension
	 */
	virtual prtx::Extension::ExtensionType getExtensionType() const final override {
		return Extension::ET_RESOLVEMAP_PROVIDER;
	}

	/**
	 * The content type of a ResolveMap Provider is undefined, it can decode into multiple possible content objects.
	 *
	 * @return Always returns prt::CT_UNDEFINED.
	 *
	 * \sa prtx::Content
	 */
	virtual prt::ContentType getContentType() const final override {
		return prt::CT_UNDEFINED;
	}
};

} // namespace prtx


#endif /* PRTX_RESOLVEMAP_PROVIDER_H_ */
