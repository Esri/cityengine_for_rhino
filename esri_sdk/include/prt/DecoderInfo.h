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

#ifndef PRT_DECODERINFO_H_
#define PRT_DECODERINFO_H_

#include "prt/prt.h"
#include "prt/Object.h"

namespace prt {

/**
 * DecoderInfo is used to return static decoder information.
 */

class PRT_EXPORTS_API DecoderInfo : public Object {
public:
	/**
	 * @param[out] status  Optional pointer to return the status.
	 * @returns the ID of the corresponding decoder.
	 */
	virtual const wchar_t* getID(Status* status = 0) const = 0;

	/**
	 * @param[out] status  Optional pointer to return the status.
	 * @returns the name of the corresponding decoder.
	 */
	virtual const wchar_t* getName(Status* status = 0) const = 0;

	/**
	 * @param[out] status  Optional pointer to return the status.
	 * @returns the description of the corresponding decoder.
	 */
	virtual const wchar_t* getDescription(Status* status = 0) const = 0;

	/**
	 * @param      iconsCount Number of returned icons
     * @param[out] status     Optional pointer to return the status.
	 * @returns the decoder icon as base64-encoded PNG.
	 */
	virtual const char* const* getIconsBase64(size_t* iconsCount, Status* status  = 0) const = 0;

	/**
	 * @returns a ';' separated list of file extensions (without a leading '.') supported by this decoder.
	 */
	virtual const wchar_t* getExtensions(Status* status = 0) const = 0;

protected:
	DecoderInfo() { }
	virtual ~DecoderInfo() { }
};


} /* namespace prt */


#endif /* PRT_DECODERINFO_H_ */
