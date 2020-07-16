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

#ifndef PRTX_RESOLVEMAP_PROVIDER_FACTORY_H_
#define PRTX_RESOLVEMAP_PROVIDER_FACTORY_H_

#include "prtx.h"
#include "prtx/Extension.h"
#include "prtx/ExtensionFactory.h"


namespace prtx {


/**
 * This is the base class for all resolve map provider factories.
 */

class PRTX_EXPORTS_API ResolveMapProviderFactory : public ExtensionFactory {
protected:
	ResolveMapProviderFactory() = default;

public:
	virtual ~ResolveMapProviderFactory() = default;

	/**
	 * Fixates the extension type to prtx::Extension::ET_RESOLVEMAP_PROVIDER.
	 *
	 * @return Always returns prtx::Extension::ET_RESOLVEMAP_PROVIDER.
	 */
	virtual prtx::Extension::ExtensionType getExtensionType() const final override {
		return Extension::ET_RESOLVEMAP_PROVIDER;
	}

	/**
	 * An resolve map provider is content type agnostic.
	 *
	 * @return Always returns prt::CT_UNDEFINED.
	 */
	virtual prt::ContentType getContentType() const final override {
		return prt::CT_UNDEFINED;
	}
};


} /* namespace prtx */


#endif /* PRTX_RESOLVEMAP_PROVIDER_FACTORY_H_ */
