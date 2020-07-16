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

#ifndef PRT_ENCODERINFO_H_
#define PRT_ENCODERINFO_H_

#include "prt/prt.h"
#include "prt/Object.h"
#include "prt/AttributeMap.h"
#include "prt/Annotation.h"


namespace prt {


/**
 * EncoderInfo is used to return static encoder information and create a valid set of encoding options
 * and annotated initial shape attributes. Please note that some initial shape attributes may be mandated by
 * a specific encoder and the prtx::Encoder::validate/encode calls will fail if they are missing.
 *
 * Please note, that dependencies between encoding options are not explicitly managed but expressed
 * via the state of each option instead (OptionState). Please use the prtx::EncoderInfoBuilder to construct
 * an EncoderInfo instance.
 *
 * \sa prtx::EncoderInfoBuilder
 */

class PRT_EXPORTS_API EncoderInfo : public Object {
public:
	/**
	 * The option states.
	 */
	enum OptionState {
		OS_ENABLED,		///< The option is enabled and should be RW in a potential UI.
		OS_DISABLED,	///< The option is disabled and should be RO in a potential UI.
		OS_HIDDEN		///< The option is hidden and should no be visible in a potential UI.
	};

public:

	/**
     * @param[out] status  Optional pointer to return the status.
	 * @returns the ID of the corresponding encoder.
	 */
	virtual const wchar_t* getID(Status* status = 0) const = 0;

	/**
     * @param[out] status  Optional pointer to return the status.
	 * @returns the name of the corresponding encoder.
	 */
	virtual const wchar_t* getName(Status* status = 0) const = 0;

	/**
     * @param[out] status  Optional pointer to return the status.
	 * @returns the description of the corresponding encoder.
	 */
	virtual const wchar_t* getDescription(Status* status = 0) const = 0;

	/**
	 * @param      iconsCount Number of returned icons
     * @param[out] status     Optional pointer to return the status.
	 * @returns the encoder icon as base64-encoded PNG.
	 */
	virtual const char* const* getIconsBase64(size_t* iconsCount, Status* status  = 0) const = 0;

	/**
	 * returns a validated copy of the encoder options and their states:
	 * - missing keys are inserted and set to their default value
	 * - unknown keys are removed
	 * - invalid values are set to the nearest valid or default value
	 * - the state of depended options are updated (e.g. disabled
	 *   if an option is not used with current values of other options)
	 *
	 * @param optionsIn  The options to be validated. If optionsIn is 0, a copy of the default encoder options is returned.
	 * @param optionsOut The newly created object containing the validated options. Must be destroyed by the caller.
	 * @param statesOut  A new instance of an AttributeMap is created with the enabled states, must be destroyed by the caller. Ignored if 0.
	 * @return status.
	 */
	virtual Status createValidatedOptionsAndStates(
			const AttributeMap* optionsIn,
			const AttributeMap** optionsOut,
			const AttributeMap** statesOut = nullptr
	) const = 0;

	/**
	 * @param[out] count   Pointer to return number of keys.
     * @param[out] status  Optional pointer to return the status.
	 * @returns all encoder option keys.
	 */
	virtual const wchar_t* const* getOptionKeys(size_t* count, Status* status = 0) const = 0;

	/**
	 * Returns all initial shape attribute keys which are known to the encoder.
	 *
	 * \sa getAnnotation
	 * \sa getNumAnnotations
	 * \sa prt::InitialShapeBuilder::setAttributes
	 */
	virtual const wchar_t* const* getInitialShapeAttributeKeys(size_t* count, Status* status = 0) const = 0;

	/**
	 * @param		key		Name of the option or initial shape attribute.
     * @param[out]	status	Optional pointer to return the status.
	 * @returns the type of a certain encoder option or initial shape attribute.
	 */
	virtual AttributeMap::PrimitiveType	getType(const wchar_t* key, Status* status = 0) const = 0;

	/**
	 * @param		key		Name of the option or initial shape attribute.
     * @param[out]	status	Optional pointer to return the status.
	 * @returns the number of annotations defined on the encode option or initial shape attribute 'key'.
	 */
	virtual size_t getNumAnnotations(const wchar_t* key, Status* status = 0) const = 0;

	/**
	 * Returns the i-th annotation on the encode option or initial shape attribute k.
	 * The following annotations are supported:
	 * - LABEL (arg1: text)
	 * - GROUP (arg1: text)
	 * - DESCRIPTION (arg1: text)
	 * - DIRECTORY
	 * - FILE(arg1..n: valid extensions without period)
	 * - RANGE(arg1: min, arg2: max)
	 * - COLOR
	 * - ORDER(arg1: alpha-numerical ordering value)
	 * - MANDATORY
	 *
	 * @param      key   Name of the option or initial shape attribute.
	 * @param      i     Annotation index.
     * @param[out] stat  Optional pointer to return the status.
	 *
	 */
	virtual const Annotation* getAnnotation(const wchar_t* key, size_t i, Status* status = 0) const = 0;

protected:
	EncoderInfo() { }
	EncoderInfo(const EncoderInfo&) : prt::Object() { };
	virtual ~EncoderInfo() { }
};


} /* namespace prt */


#endif /* PRT_ENCODERINFO_H_ */
