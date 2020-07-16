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

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include "prt/Status.h"

#include <cstddef>
#include <cstdint>


namespace prt {

/** @namespace prt::StringUtils
 * @brief Utilities for string encoding.
 *
 */
namespace StringUtils {


/**
 * Converts a UTF16 wide string to a narrow string with default platform encoding.
 *
 * @param         utf16String The string to convert.
 * @param[in,out] result      Pointer to buffer to receive the null-terminated encoded representation.
 * @param[in,out] resultSize  Pointer to size of reserved buffer; returns the size of the actual result (incl. terminating 0). Is always greater than 0.
 * @param[out]    stat        Optional pointer to return the status.
 * @returns result.
 */
PRT_EXPORTS_API char*    toOSNarrowFromUTF16(const wchar_t* utf16String, char* result, size_t* resultSize, Status* stat = 0);

/**
 * Converts a narrow string with default platform encoding to a UTF16 wide string.
 *
 * @param         osString    The string to convert.
 * @param[in,out] result      Pointer to buffer to receive the null-terminated encoded representation.
 * @param[in,out] resultSize  Pointer to size of reserved buffer; returns the size of the actual result (incl. terminating 0). Is always greater than 0.
 * @param[out]    stat        Optional pointer to return the status.
 * @returns result.
 */
PRT_EXPORTS_API wchar_t* toUTF16FromOSNarrow(const char* osString, wchar_t* result, size_t* resultSize, Status* stat = 0);

/**
 * Converts a UTF8 narrow string to a UTF16 wide string.
 *
 * @param         utf8String  The string to convert.
 * @param[in,out] result      Pointer to buffer to receive the null-terminated encoded representation.
 * @param[in,out] resultSize  Pointer to size of reserved buffer; returns the size of the actual result (incl. terminating 0). Is always greater than 0.
 * @param[out]    stat        Optional pointer to return the status.
 * @returns result.
 */
PRT_EXPORTS_API wchar_t* toUTF16FromUTF8(const char* utf8String, wchar_t* result, size_t* resultSize, Status* stat = 0);

/**
 * Converts a UTF16 wide string to a UTF8 narrow string.
 *
 * @param         utf16String The string to convert.
 * @param[in,out] result      Pointer to buffer to receive the null-terminated encoded representation.
 * @param[in,out] resultSize  Pointer to size of reserved buffer; returns the size of the actual result (incl. terminating 0). Is always greater than 0.
 * @param[out]    stat        Optional pointer to return the status.
 * @returns result.
 *
 */
PRT_EXPORTS_API char*    toUTF8FromUTF16(const wchar_t* utf16String, char* result, size_t* resultSize, Status* stat = 0);

/**
 * Percent-encodes a UTF8 string.
 *
 * @param         utf8String The string to encode.
 * @param[in,out] result      Pointer to buffer to receive the null-terminated encoded representation.
 * @param[in,out] resultSize  Pointer to size of reserved buffer; returns the size of the actual result (incl. terminating 0). Is always greater than 0.
 * @param[out]    stat        Optional pointer to return the status.
 * @returns result.
 */
PRT_EXPORTS_API char*    percentEncode(const char* utf8String, char* result, size_t* resultSize, Status* stat = 0);

}


}

#endif /* STRINGUTILS_H_ */
