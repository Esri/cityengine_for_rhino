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

#ifndef PRT_ATTRIBUTEMAP_H_
#define PRT_ATTRIBUTEMAP_H_

#include "prt/prt.h"
#include "prt/Attributable.h"
#include "prt/Object.h"


namespace prt {


/**
 * A map implementing the Attributable interface. Use an AttributeMapBuilder to create an AttributeMap instance.
 */
class PRT_EXPORTS_API AttributeMap : public Attributable, public Object {
protected:
	AttributeMap();
	virtual ~AttributeMap();
};


/**
 * A builder to create AttributeMap instances.
 */
class PRT_EXPORTS_API AttributeMapBuilder : public AttributableSetter, public Object {
public:
	/**
	 * Creates a new AttributeMapBuilder instance.
	 *
	 * @param[out]	status	Optional pointer to return the status.
	 * @returns				Pointer to a new instance of an AttributeMapBuilder on success or 0 on failure.
	 */
	static	AttributeMapBuilder*	create(Status* status = 0);

	/**
	 * Creates a new AttributeMapBuilder instance initialized to the content of a AttributeMap.
	 *
	 * @param[in]	src		Source AttributeMap. The new instance wil be initialized to the values of src.
	 * @param[out]	status	Optional pointer to return the status.
	 * @returns				Pointer to a new instance of an AttributeMapBuilder on success or 0 on failure.
	 */
	static	AttributeMapBuilder*	createFromAttributeMap(const AttributeMap* src, Status* status = 0);

	/**
	 * Creates a new AttributeInstance which contains all currently set key-value pairs. The AttributeBuilder is reset to
	 * its initial state.
	 *
	 * @param[out]	status	Optional pointer to return the status.
	 * @returns				Pointer to a new instance of an AttributeMap on success or 0 on failure.
	 */
	virtual AttributeMap const*		createAttributeMapAndReset(Status* status = 0) = 0;

	/**
	 * Creates a new AttributeInstance which contains all currently set key-value pairs. The AttributeBuilder's state is preserved.
	 *
	 * @param[out]	status	Optional pointer to return the status.
	 * @returns				Pointer to a new instance of an AttributeMap on success or 0 on failure.
	 */
	virtual AttributeMap const*		createAttributeMap(Status* status = 0) const = 0;

	/**
	 * Resets the AttributeMapBuilder to the content of the passed AttributeMap.
	 *
	 * @param[in]	src		Source AttributeMap. The new instance wil be initialized to the values of src.
	 * @param[out]	status	Optional pointer to return the status.
	 */
	virtual void resetToAttributeMap(const AttributeMap* src, Status* status = 0) = 0;

	/**
	 * Adds the content of src to the current builder state. Conflict resolution is as follows:
	 * - If key and type of the incoming item match with an existing item the content will be overwritten.
	 * - If the key matches but not the type, the content will only be overwritten if force equals true.
	 *
	 * @param[in]	src		Source AttributeMap. Its content will be added to the builder.
	 * @param[in]	force	Conflict resolution control flag used in case of type mismatch.
	 * @param[out]	status	Optional pointer to return the status.
	 */
	virtual void updateWithAttributeMap(const AttributeMap* src, bool force = false, Status* status = 0) = 0;

protected:
	AttributeMapBuilder();
	virtual ~AttributeMapBuilder();
};


} // namespace prt


#endif /* PRT_ATTRIBUTEMAP_H_ */
