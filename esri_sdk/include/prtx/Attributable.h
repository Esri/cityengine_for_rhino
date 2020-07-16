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

#ifndef PRTX_ATTRIBUTABLE_H_
#define PRTX_ATTRIBUTABLE_H_

#include "prt/Attributable.h"

#include "prtx/prtx.h"
#include "prtx/Types.h"

#include <string>


namespace prtx {

/**
 * Interface to provide read-only access to attributes of the basic types.
 * Exceptions are thrown on errors.
 *
 * Each key is unique over all types.
 */
class PRTX_EXPORTS_API Attributable {
public:
	/**
	 * Primitive Types (inherited from prt::Attributable)
	 */
	using PrimitiveType = uint8_t;

	static const PrimitiveType PT_UNDEFINED			= prt::Attributable::PT_UNDEFINED;
	static const PrimitiveType PT_STRING			= prt::Attributable::PT_STRING;
	static const PrimitiveType PT_FLOAT				= prt::Attributable::PT_FLOAT;
	static const PrimitiveType PT_BOOL				= prt::Attributable::PT_BOOL;
	static const PrimitiveType PT_INT				= prt::Attributable::PT_INT;
	static const PrimitiveType PT_STRING_ARRAY		= prt::Attributable::PT_STRING_ARRAY;
	static const PrimitiveType PT_FLOAT_ARRAY		= prt::Attributable::PT_FLOAT_ARRAY;
	static const PrimitiveType PT_BOOL_ARRAY		= prt::Attributable::PT_BOOL_ARRAY;
	static const PrimitiveType PT_INT_ARRAY			= prt::Attributable::PT_INT_ARRAY;
	static const PrimitiveType PT_BLIND_DATA		= prt::Attributable::PT_BLIND_DATA;
	static const PrimitiveType PT_BLIND_DATA_ARRAY	= prt::Attributable::PT_BLIND_DATA_ARRAY;
	static const PrimitiveType PT_COUNT				= prt::Attributable::PT_COUNT;

	/** Get all used keys. The returned reference stays valid until destruction of the Attributable.
	 * @returns reference to vector with all all keys.
	 */
	virtual const WStringVector&	getKeys()									const = 0;
	/**
	 * Checks whether a key is set.
	 * @param key Key to look for.
	 * @returns true if the key is set, false otherwise.
	 */
	virtual bool					hasKey			(const std::wstring& key)	const = 0;
	/**
	 * @param key Key to look for.
	 * @returns the type of the key's value or throws an exception if key is not found.
	 */
	virtual PrimitiveType			getType			(const std::wstring& key)	const = 0;

	/**
	 * Gets a boolean value.
	 * @param key Key to look for.
	 * @returns the value associated with a key or throws an exception if key not found.
	 */
	virtual Bool					getBool			(const std::wstring& key)	const = 0;
	/**
	 * Gets an integer value.
	 * @param key Key to look for.
	 * @returns the value associated with a key or throws an exception if key not found.
	 */
	virtual int32_t					getInt			(const std::wstring& key)	const = 0;
	/**
	 * Gets a float value.
	 * @param key Key to look for.
	 * @returns the value associated with a key or throws an exception if key not found.
	 */
	virtual double					getFloat		(const std::wstring& key)	const = 0;
	/**
	 * Gets a string. The returned reference stays valid until destruction of the Attributable.
	 * @param key Key to look for.
	 * @returns reference to the string associated with a key or throws an exception if key not found.
	 */
	virtual const std::wstring&		getString		(const std::wstring& key)	const = 0;

	/**
	 * Gets an array of booleans. The returned reference stays valid until destruction of the Attributable.
	 * @param key Key to look for.
	 * @returns reference to a vector associated with a key or throws an exception if key not found.
	 */
	virtual const BoolVector&		getBoolArray	(const std::wstring& key)	const = 0;
	/**
	 * Gets an array of integers. The returned reference stays valid until destruction of the Attributable.
	 * @param key Key to look for.
	 * @returns reference to a vector associated with a key or throws an exception if key not found.
	 */
	virtual const Int32Vector&		getIntArray		(const std::wstring& key)	const = 0;
	/**
	 * Gets an array of floats. The returned reference stays valid until destruction of the Attributable.
	 * @param key Key to look for.
	 * @returns reference to a vector associated with a key or throws an exception if key not found.
	 */
	virtual const DoubleVector&		getFloatArray	(const std::wstring& key)	const = 0;
	/**
	 * Gets an array of strings. The returned reference stays valid until destruction of the Attributable.
	 * @param key Key to look for.
	 * @returns reference to a vector associated with a key or throws an exception if key not found.
	 */
	virtual const WStringVector&	getStringArray	(const std::wstring& key)	const = 0;

	/**
	 * Gets a blind data value.
	 * @param key Key to look for.
	 * @returns blind data value associated with a key or throws an exception if key not found.
	 */
	virtual void*					getBlindData	(const std::wstring& key)	const = 0;

protected:
	Attributable() {}
	virtual ~Attributable() {}
};


/**
 * Interface for setting attributable values.
 */
class PRTX_EXPORTS_API AttributableSetter {
public:
	/**
	 * Sets a boolean value.
	 * @param key The key of the value.
	 * @param val The value to set.
	 */
	virtual void setBool			(const std::wstring& key, Bool								val) = 0;
	/**
	 * Sets an integer value.
	 * @param key The key of the value.
	 * @param val The value to set.
	 */
	virtual void setInt				(const std::wstring& key, int32_t							val) = 0;
	/**
	 * Sets a float value.
	 * @param key The key of the value.
	 * @param val The value to set.
	 */
	virtual void setFloat			(const std::wstring& key, double							val) = 0;
	/**
	 * Sets a string value.
	 * @param key The key of the string.
	 * @param val The string to set.
	 */
	virtual void setString			(const std::wstring& key, const std::wstring&				val) = 0;

	/**
	 * Sets a boolean array.
	 * @param key The key of the array.
	 * @param val Vector with the values to set.
	 */
	virtual void setBoolArray		(const std::wstring& key, const BoolVector&					val) = 0;
	/**
	 * Sets an integer array.
	 * @param key The key of the array.
	 * @param val Vector with the values to set.
	 */
	virtual void setIntArray		(const std::wstring& key, const Int32Vector&				val) = 0;
	/**
	 * Sets a float array.
	 * @param key The key of the array.
	 * @param val Vector with the values to set.
	 */
	virtual void setFloatArray		(const std::wstring& key, const DoubleVector&				val) = 0;
	/**
	 * Sets a string array.
	 * @param key The key of the array.
	 * @param val Vector with the values to set.
	 */
	virtual void setStringArray		(const std::wstring& key, const WStringVector&				val) = 0;

	/**
	 * Sets a blind data value.
	 * @param key The key of the value.
	 * @param val The blind data value to set.
	 */
	virtual void setBlindData		(const std::wstring& key, void*								val) = 0;

protected:
	AttributableSetter() {}
	virtual ~AttributableSetter() {}
};


} // namespace prtx


#endif /* PRTX_ATTRIBUTABLE_H_ */
