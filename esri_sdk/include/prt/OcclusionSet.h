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

#ifndef PRT_OCCLUSIONSET_H_
#define PRT_OCCLUSIONSET_H_

#include "prt/Object.h"
#include "prt/Status.h"

#include <cstddef>
#include <cstdint>


namespace prt {


/**
 * The OcclusionSet is used to manage a set of occluders. These are generated
 * through generateOccluders().
 * The OcclusionSet is thread-safe.
 * It is the client's responsibility not to dispose handles which are currently
 * used in a generate() call.
 */

class PRT_EXPORTS_API OcclusionSet : public Object {
public:
	/**
	 * @param[out] stat Optional pointer to return the status.
	 * @returns a new Instance or 0 on error.
	 */
	static OcclusionSet* create(Status* stat = 0);

	typedef uint64_t Handle;

	/**
	 * @param handles Array of handles to dispose. 0-handles are ignored.
	 * @param count number of handles in array.
	 */
	virtual Status dispose(Handle* handles, size_t count) = 0;

protected:
	OcclusionSet();
	virtual ~OcclusionSet();
};


} // namespace prt


#endif /* PRT_OCCLUSIONSET_H_ */
