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

#ifndef PRTX_TYPES_H_
#define PRTX_TYPES_H_

#include "prtx/prtx.h"

#include <memory>
#include <string>
#include <vector>


namespace prtx {

typedef uint8_t							Bool;				///< uint8_t based Bool type
typedef std::vector<prtx::Bool>			BoolVector;			///< vector of uint8_t based Bool type
typedef std::vector<double>				DoubleVector;		///< vector of double
typedef std::vector<int32_t>			Int32Vector;		///< vector of int32_t
typedef std::vector<std::string>		StringVector;		///< vector of std::string
typedef std::vector<std::wstring>		WStringVector;		///< vector of std::wstring
typedef std::shared_ptr<std::wstring>	StringPtr;			///< shared std::string
typedef std::vector<uint8_t>			BinaryVector;		///< vector to hold binary data

const Bool PRTX_FALSE	= static_cast<Bool>(0u);
const Bool PRTX_TRUE	= static_cast<Bool>(1u);
const Bool PRTX_NO		= static_cast<Bool>(0u);
const Bool PRTX_YES		= static_cast<Bool>(1u);

inline bool toPrimitive(Bool b) { return (b == PRTX_TRUE); }
PRTX_EXPORTS_API Bool boolFromString(const std::wstring& w);
PRTX_EXPORTS_API Bool boolFromString(const std::string& s);

} // namespace prtx


#endif /* PRTX_TYPES_H_ */
