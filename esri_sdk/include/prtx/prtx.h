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

#ifndef PRTX_H_
#define PRTX_H_


#ifdef _WIN32
#	ifdef PRTX_EXPORTS
#		define PRTX_EXPORTS_API __declspec(dllexport)
#	else
#		define PRTX_EXPORTS_API __declspec(dllimport)
#	endif
#else
#	define PRTX_EXPORTS_API __attribute__ ((visibility ("default")))
#endif



/** @namespace prtx
 * @brief The Procedural Runtime eXtension namespace.
 * The prtx namespace contains the tools to extend the Procedural Runtime with stream adaptors, resolve map providers, decoders, and encoders.
 */
namespace prtx {

}

#endif /* PRTX_H_ */
