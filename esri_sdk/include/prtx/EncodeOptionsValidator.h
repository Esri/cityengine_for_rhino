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

#ifndef PRTX_ENCODEOPTIONSVALIDATOR_H_
#define PRTX_ENCODEOPTIONSVALIDATOR_H_

#include "prtx/prtx.h"
#include "prtx/PRTUtils.h"

#include "prt/EncoderInfo.h"

#include <string>
#include <vector>
#include <memory>


namespace prtx {


class OptionStates;
typedef std::shared_ptr<OptionStates> OptionStatesPtr;

/**
 * Interface to set and get the state of the encoder options after validation.
 */
class PRTX_EXPORTS_API OptionStates {
public:
	typedef std::vector<const std::wstring*> KeyRefs;

public:
	virtual const KeyRefs& getKeys() const = 0;
	virtual prt::EncoderInfo::OptionState getState(const std::wstring& key) const = 0;
	virtual void setState(const std::wstring& key, prt::EncoderInfo::OptionState state) = 0;

	virtual ~OptionStates() { }
};


class EncodeOptionsValidator;
typedef std::shared_ptr<EncodeOptionsValidator> EncodeOptionsValidatorPtr; ///< shared pointer for prtx::EncodeOptionsValidator

/**
 * Interface for encoder-specific validation of client-generated encoder options.
 */
class PRTX_EXPORTS_API EncodeOptionsValidator {
public:
	EncodeOptionsValidator() = default;
	EncodeOptionsValidator(const EncodeOptionsValidator&) = delete;
	EncodeOptionsValidator(EncodeOptionsValidator&&) = delete;
	EncodeOptionsValidator& operator=(const EncodeOptionsValidator&) = delete;
	EncodeOptionsValidator& operator=(EncodeOptionsValidator&&) = delete;
	virtual ~EncodeOptionsValidator() = default;

	/**
	 * Encoder authors may want to implement this interface for each encoder. To use an EncodeOptionsValidator
	 * implementation with a specific encoder, register an instance of it with prtx::EncoderInfoBuilder::setValidator()
	 * before creating the corresponding EncoderFactory. The validation is triggered by calling
	 * prt::EncoderInfo::createValidatedOptionsAndStates().
	 *
	 * For example, a simple validate implementation could replace all non-ascii characters of an imaginary
	 * encoder option "name" with underscore characters.
	 *
	 * \sa prt::EncoderInfo
	 * \sa prtx::OptionStates
	 */
	virtual void validate(
			const prtx::PRTUtils::AttributeMapPtr&			currValues,	///< [in] The client-generated option values.
			const prtx::PRTUtils::AttributeMapBuilderPtr&	newValues,	///< [in,out] A builder to receive the validated values.
			const prtx::OptionStatesPtr&					states		///< [out] The validated option states.
	) = 0;
};


} // namespace prtx


#endif /* PRTX_ENCODEOPTIONSVALIDATOR_H_ */
