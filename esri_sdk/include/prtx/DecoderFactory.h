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

#ifndef PRTX_DECODERFACTORY_H_
#define PRTX_DECODERFACTORY_H_

#include "prtx/prtx.h"
#include "prtx/URI.h"
#include "prtx/ExtensionFactory.h"
#include "prtx/FileExtensions.h"

#include "prt/DecoderInfo.h"

#include <string>


#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4251 4231 4660)
#endif


class DecoderInfoImpl;


namespace prtx {

/**
 * This is the base class for all decoder factories. A subclass may want to override getMerit() to
 * control the resolve order of multiple decoders with the same type and id.
 */

class PRTX_EXPORTS_API DecoderFactory : public ExtensionFactory {
public:
	DecoderFactory() = delete;

	DecoderFactory(
			prt::ContentType		type,						///< [in] The content type of this extension.
			const std::wstring&		id,							///< [in] The ID of this extension.
			const std::wstring&		name,						///< [in] A human-readable name of this extension.
			const std::wstring&		desc,						///< [in] A human-readable description of this extension.
			const FileExtensions&	exts	= FileExtensions(),	///< [in] Supported file extensions of this extension.
			const char*				icon	= 0					///< [in] An icon as Base64-encoded PNG byte array.
	);
	virtual ~DecoderFactory();

	/**
	 * Returns a new instance of the corresponding decoder information.
	 * Must be destroyed by the client.
	 */
	prt::DecoderInfo* createDecoderInfo() const;

	/**
	 * Returns true, if the URI matches one of the registered file extensions.
	 */
	virtual bool canHandleURI(prtx::URIPtr uri) const override;

	/**
	 * Fixates the extension type to prtx::Extension::ET_DECODER;
	 *
	 * @return Always returns prtx::Extension::ET_DECODER.
	 */
	virtual prtx::Extension::ExtensionType getExtensionType() const final override {
		return Extension::ET_DECODER;
	}

protected:
	DecoderInfoImpl* mInfo {nullptr};
};


} /* namespace prtx */


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_DECODERFACTORY_H_ */
