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

#ifndef PRT_FILEOUTPUTCALLBACKS_H_
#define PRT_FILEOUTPUTCALLBACKS_H_

#include "prt/prt.h"
#include "prt/Object.h"
#include "prt/Callbacks.h"
#include "prt/ContentType.h"

namespace prt {

/**
 * File-based implementation of the SimpleOutputCallbacks interface.
 */
class PRT_EXPORTS_API FileOutputCallbacks: public SimpleOutputCallbacks, public Object {
public:
	/**
	 * Creates a FileOutputCallbacks instance. The returned instance must be destroyed by the caller.
	 *
	 * @param dir                   Absolute filesystem path to the base directory.
	 *                              The name parameter of the open calls will be a file relative to this directory.
	 * @param cgaOutputEnc          Optional encoding for cgaPrint, cgaReport and cgaError (string) calls.
	 * @param generateErrorFileName Optional: Specifies the file name used to write out the content of the
	 *                              generateError callback. May contain a relative path to dir.
	 *                              If set to nullptr, no file will be written.
	 * @param[out] stat             Optional pointer to return the status.
 	 * @returns a pointer to the instance or nullptr in case of error.
	 */
	static FileOutputCallbacks* create(const wchar_t* dir, StringEncoding cgaOutputEnc = SE_NATIVE,
	                                   const wchar_t* generateErrorFileName = nullptr,Status* status = nullptr);

protected:
	FileOutputCallbacks() = default;
	virtual ~FileOutputCallbacks() = default;
};


} // namespace prt


#endif /* PRT_FILEOUTPUTCALLBACKS_H_ */
