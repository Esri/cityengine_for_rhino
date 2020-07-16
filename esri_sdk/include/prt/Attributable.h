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

#ifndef PRT_ATTRIBUTABLE_H_
#define PRT_ATTRIBUTABLE_H_

#include "prt/prt.h"
#include "prt/Status.h"

#include <cstddef>
#include <cstdint>


namespace prt {


/**
 * Interface to provide read-only access to attributes of the basic types.
 * All member functions are thread-safe.
 *  * undefined keys result in result prt::PRT_KEY_NOT_FOUND
 *  * each key is unique over all types
 */

class PRT_EXPORTS_API Attributable {
public:

	/**
	 * The PrimitiveType enum enumerates the types which can be used in an Attributable.
	 */
	enum PrimitiveType {
		PT_UNDEFINED,			///< Undefined type.
		PT_STRING,				///< String type.
		PT_FLOAT,				///< Float type.
		PT_BOOL,				///< Boolean type.
		PT_INT,					///< Integer type.
		PT_STRING_ARRAY,		///< String array type.
		PT_FLOAT_ARRAY,			///< Float array type.
		PT_BOOL_ARRAY,			///< Boolean array type.
		PT_INT_ARRAY,			///< Integer array type.
		PT_BLIND_DATA,			///< Blind data type.
		PT_BLIND_DATA_ARRAY,	///< Blind data array type.
		PT_COUNT				///< Number of primitive types.
	};

	/**
	 * Get all used keys.
	 * The returned pointer stays valid until destruction of the Attributable.
	 *
	 * @param[out] count      Pointer to return the number of keys.
	 * @param[out] stat       Optional pointer to return the status.
	 * @returns Pointer to the array with all keys on success, 0 on error.
	 *
	 */
	virtual const wchar_t* const*	getKeys			(size_t* count, Status* stat = 0) const = 0;

	/**
	 * Checks whether a key is set.
	 *
	 * @param      key        Key to look for.
	 * @param[out] stat       Optional pointer to return the status.
	 * @returns true if the key is set, false otherwise.
	 */
	virtual bool			hasKey			(const wchar_t* key, Status* stat = 0) const = 0;

	/**
	 * Gets the type of the value associated to a key.
	 *
	 * @param      key        Key to look for.
	 * @param[out] stat       Optional pointer to return the status.
	 * @returns The type of the key's value or PT_UNDEFINED if the key is not found.
	 */
	virtual PrimitiveType	getType			(const wchar_t* key, Status* stat = 0) const = 0;

	/**
	 * Gets a boolean value.
	 *
	 * @param      key        Key to look for.
	 * @param[out] stat       Optional pointer to return the status.
	 *                        Will be set to STATUS_KEY_NOT_FOUND if the key is not found or if the value is not a boolean.
	 * @returns The value associated with a key or false if key not found or if the value is not a boolean.
	 */
	virtual bool			getBool			(const wchar_t* key, Status* stat = 0) const = 0;

	/**
	 * Gets an integer value.
	 *
	 * @param      key        Key to look for.
	 * @param[out] stat       Optional pointer to return the status.
	 *                        Will be set to STATUS_KEY_NOT_FOUND if the key is not found or if the value is not an int.
	 * @returns The value associated with a key or 0 if key not found or if the value is not a int.
	 */
	virtual int32_t			getInt			(const wchar_t* key, Status* stat = 0) const = 0;

	/**
	 * Gets a float value.
	 *
	 * @param      key        Key to look for.
	 * @param[out] stat       Optional pointer to return the status.
	 *                        Will be set to STATUS_KEY_NOT_FOUND if the key is not found or if the value is not a float.
	 * @returns The value associated with a key or 0 if key not found or if the value is not a float.
	 */
	virtual double			getFloat		(const wchar_t* key, Status* stat = 0) const = 0;

	/**
	 * Gets a string.
	 * The returned pointer stays valid until destruction of the Attributable.
	 *
	 * @param      key        Key to look for.
	 * @param[out] stat       Optional pointer to return the status.
	 *                        Will be set to STATUS_KEY_NOT_FOUND if the key is not found or if the value is not a string.
	 * @returns The pointer to a string associated with a key or 0 if key not found or if the value is not a string.
	 */
	virtual const wchar_t*	getString		(const wchar_t* key, Status* stat = 0) const = 0;

	/**
	 * Gets an array of booleans.
	 * The returned pointer stays valid until destruction of the Attributable.
	 *
	 * @param      key        Key to look for.
	 * @param[out] count      Pointer to return the number of booleans in the array.
	 * @param[out] stat       Optional pointer to return the status.
	 *                        Will be set to STATUS_KEY_NOT_FOUND if the key is not found or if the value is not an array of booleans.
	 * @returns Pointer to the array associated with the key or 0 if key not found or if the value is not an array of booleans.
	 */
	virtual const bool*		getBoolArray	(const wchar_t* key, size_t* count, Status* stat = 0) const = 0;

	/**
	 * Gets an array of integers.
	 * The returned pointer stays valid until destruction of the Attributable.
	 *
	 * @param      key        Key to look for.
	 * @param[out] count      Pointer to return the number of integers in the array.
	 * @param[out] stat       Optional pointer to return the status.
	 *                        Will be set to STATUS_KEY_NOT_FOUND if the key is not found or if the value is not an array of integers.
	 * @returns Pointer to the array associated with the key or 0 if key not found or if the value is not an array of integers.
	 */
	virtual const int32_t*	getIntArray		(const wchar_t* key, size_t* count, Status* stat = 0) const = 0;

	/**
	 * Gets an array of floats.
	 * The returned pointer stays valid until destruction of the Attributable.
	 *
	 * @param      key        Key to look for.
	 * @param[out] count      Pointer to return the number of floats in the array.
	 * @param[out] stat       Optional pointer to return the status.
	 *                        Will be set to STATUS_KEY_NOT_FOUND if the key is not found or if the value is not an array of floats.
	 * @returns Pointer to the array associated with the key or 0 if key not found or if the value is not an array of floats.
	 */
	virtual const double*	getFloatArray	(const wchar_t* key, size_t* count, Status* stat = 0) const = 0;

	/**
	 * Gets an array of strings.
	 * The returned pointer stays valid until destruction of the Attributable.
	 *
	 * @param      key        Key to look for.
	 * @param[out] count      Pointer to return the number of strings in the array.
	 * @param[out] stat       Optional pointer to return the status.
	 *                        Will be set to STATUS_KEY_NOT_FOUND if the key is not found or if the value is not an array of strings.
	 * @returns Pointer to the array associated with the key or 0 if key not found or if the value is not an array of strings.
	 */
	virtual const wchar_t* const*	getStringArray	(const wchar_t* key, size_t* count, Status* stat = 0) const = 0;

protected:
	Attributable() { }
	virtual ~Attributable() { }
};


/**
 * Interface for write-only attribute builder classes
 */
class PRT_EXPORTS_API AttributableSetter {
public:

	/**
	 * Sets a boolean.
	 *
	 * @param key     Key to set.
	 * @param value   Value to copy.
	 * @returns STATUS_OK on success, STATUS_KEY_ALREADY_TAKEN or other status on failure.
	 */
	virtual Status				setBool			(const wchar_t* key, bool				value) = 0;

	/**
	 * Sets an integer.
	 *
	 * @param key     Key to set.
	 * @param value   Value to copy.
	 * @returns STATUS_OK on success, STATUS_KEY_ALREADY_TAKEN or other status on failure.
	 */
	virtual Status				setInt			(const wchar_t* key, int32_t			value) = 0;

	/**
	 * Sets a float value.
	 *
	 * @param key     Key to set.
	 * @param value   Value to copy.
	 * @returns STATUS_OK on success, STATUS_KEY_ALREADY_TAKEN or other status on failure.
	 */
	virtual Status				setFloat		(const wchar_t* key, double			    value) = 0;

	/**
	 * Sets a string.
	 *
	 * @param key     Key to set.
	 * @param value   Pointer to string to copy.
	 * @returns STATUS_OK on success, STATUS_KEY_ALREADY_TAKEN or other status on failure.
	 */
	virtual Status				setString		(const wchar_t* key, const wchar_t*	    value) = 0;

	/**
	 * Sets an array of booleans.
	 *
	 * @param key     Key to set.
	 * @param value   Pointer to the array to copy.
	 * @param count   Number of elements to copy.
	 * @returns STATUS_OK on success, STATUS_KEY_ALREADY_TAKEN or other status on failure.
	 */
	virtual Status				setBoolArray	(const wchar_t* key, const bool*		value, size_t count) = 0;

	/**
	 * Sets an array of integers.
	 *
	 * @param key     Key to set.
	 * @param value   Pointer to the array to copy.
	 * @param count   Number of elements to copy.
	 * @returns STATUS_OK on success, STATUS_KEY_ALREADY_TAKEN or other status on failure.
	 */
	virtual Status				setIntArray		(const wchar_t* key, const int32_t*	    value, size_t count) = 0;

	/**
	 * Sets an array of floats.
	 *
	 * @param key     Key to set.
	 * @param value   Pointer to the array to copy.
	 * @param count   Number of elements to copy.
	 * @returns STATUS_OK on success, STATUS_KEY_ALREADY_TAKEN or other status on failure.
	 */
	virtual Status				setFloatArray	(const wchar_t* key, const double*	    value, size_t count) = 0;

	/**
	 * Sets an array of strings.
	 *
	 * @param key     Key to set.
	 * @param value   Pointer to the array to copy.
	 * @param count   Number of elements to copy.
	 * @returns STATUS_OK on success, STATUS_KEY_ALREADY_TAKEN or other status on failure.
	 */
	virtual Status				setStringArray	(const wchar_t* key, const wchar_t* const* value, size_t count) = 0;

protected:
	AttributableSetter() { }
	virtual ~AttributableSetter() { }
};


} // namespace


#endif /* PRT_ATTRIBUTABLE_H_ */
