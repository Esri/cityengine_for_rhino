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

#ifndef PRT_OBJECT_H
#define PRT_OBJECT_H

#include "prt/prt.h"
#include "prt/Status.h"

#include <cstddef>


namespace prt {


/**
 * The base class of all client-instantiable classes. prt objects can either be instantiated
 * via static crate() call or a matching builder and must be destroyed via destroy().
 *
 */
class PRT_EXPORTS_API Object {
public:
	/**
	 * Destroys this instance.
	 */
	void destroy() const;

	/**
	 * Returns the XML representation of this object. Does not include any XML headers,
	 * for a full XML document see toXMLDocument(). Implemented by all subclasses.
	 *
	 * If the result does not fit into the allocated buffer, the result is truncated.
	 * The INOUT parameter resultSize must be compared to the original buffer size after
	 * the call to detect this situation.
	 *
	 * @param[in,out] result     Pointer to buffer to receive the null-terminated XML encoded representation.
	 * @param[in,out] resultSize Pointer to size of reserved buffer; returns the size of the actual result (incl. terminating 0). Is always greater than 0.
     * @param[out]    stat       Optional pointer to return the status.
	 * @returns result.
	 */
	virtual char* toXML(char* result, size_t* resultSize, Status* stat = 0) const = 0;

	/**
	 * Dumps the object to a standalone XML document. Basically combines the output of toXML() with XML headers.
	 *
	 * If the result does not fit into the allocated buffer, the result is truncated. the INOUT parameter resultSize must be compared to
	 * the original buffer size after the call to detect this situation.
	 *
	 * @param[in,out] result     Pointer to buffer to receive the null-terminated XML encoded representation.
	 * @param[in,out] resultSize Pointer to size of reserved buffer; returns the size of the actual result (incl. terminating 0). Is always greater than 0.
     * @param[out]    stat       Optional pointer to return the status.
	 * @returns result.
	 */
	char* toXMLDocument(char* result, size_t* resultSize, Status* stat = 0) const;

protected:
	Object() { }
	Object(const Object&) { }
	virtual ~Object() { }
};


} /* namespace prt */


#endif /* PRT_OBJECT_H */
