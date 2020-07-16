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

#ifndef PRT_LOGLEVEL_H_
#define PRT_LOGLEVEL_H_

#include "prt/prt.h"


namespace prt {

/**
 * Log levels for the the logger.
 */
enum PRT_EXPORTS_API LogLevel {
	LOG_TRACE   = 0,	///< Internal call tracing. This log level is unused in release builds of the runtime but may be used by client applications.
	LOG_DEBUG   = 1,	///< Internal debug information. This log level is unused in release builds of the runtime but may be used by client applications.
	LOG_INFO    = 2,	///< Informational messages such as configuration information during initialization.
	LOG_WARNING = 3,	///< Warning messages issued at a condition that did not abort an ongoing execution.
	LOG_ERROR   = 4,	///< Error messages issued at a condition that altered/aborted the ongoing execution.
	LOG_FATAL   = 5,	///< Fatal errors that leave the procedural runtime in an inconsistent /non-operational state.
	LOG_NO      = 1000,	///< No log level set.
};


}

#endif /* PRT_LOGLEVEL_H_ */
