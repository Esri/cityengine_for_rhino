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

#ifndef PRTX_ENCODERINFOBUILDER_H_
#define PRTX_ENCODERINFOBUILDER_H_

#include "prtx/prtx.h"
#include "prtx/Builder.h"
#include "prtx/EncodeOptionsValidator.h"
#include "prtx/FileExtensions.h"

#include "prt/ContentType.h"
#include "prt/EncoderInfo.h"

#include <string>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4275)
#endif


class EncoderInfoImpl;


namespace prtx {


/**
 * Builder class for EncoderInfo objects.
 *
 * See \ref AppNotes how to setup a valid encoder for CityEngine.
 */
class PRTX_EXPORTS_API EncoderInfoBuilder : public Builder<const prt::EncoderInfo> {
public:
	EncoderInfoBuilder();
	virtual ~EncoderInfoBuilder();

	/**
	 * Creates a prt::EncoderInfo instance from the current builder state.
	 *
	 * @return A new prt::EncoderInfo instance. Needs to be destroyed by the caller.
	 */
	virtual const prt::EncoderInfo* create(std::wstring* warnings = 0) const;

	/**
	 * Creates a prt::EncoderInfo instance from the current builder state and resets the builder state.
	 *
	 * @return A new prt::EncoderInfo instance. Needs to be destroyed by the caller.
	 */
	virtual const prt::EncoderInfo* createAndReset(std::wstring* warnings = 0);

	/**
	 * Specifies the content type of the corresponding encoder.
	 */
	void setType(prt::ContentType type);

	/**
	 * Specifies the ID of the corresponding encoder.
	 */
	void setID(const std::wstring& id);

	/**
	 * Specifies the human-readable name of the corresponding encoder.
	 */
	void setName(const std::wstring& name);

	/**
	 * Specifies the human-readable description of the corresponding encoder.
	 */
	void setDescription(const std::wstring& desc);

	/**
	 * Convenience member function to directly specifiy a single file extension for the corresponding encoder.
	 * @param ext The extension, must start with a '.'
	 */
	void setExtension(const std::wstring& ext);

	/**
	 * Specifies the supported file extensions by the corresponding encoder.
	 */
	void setExtensions(const prtx::FileExtensions& exts);

	/**
	 * Specifies the icon for the corresponding encoder. Useful for UI generation code.
	 */
	void setIcon(
			const char* icon	///< [in] An icon as Base64-encoded PNG byte array.
	);

	/**
	 * Specify the default options of the corresponding encoder class.
	 * Takes over the ownership of defaultOptions.
	 */
	void setDefaultOptions(prt::AttributeMap const* defaultOptions);

	/**
	 * Makes certain initial shape attribute keys known to the encoder. This can be combined with
	 * addAnnotation to give the encoder more semantic about certain initial shape attributes. For example,
	 * if an initial shape attribute 'foo' is annotated with MANDATORY, the validation and encode calls will fail
	 * if the attribute 'foo' of the specified type is missing.
	 *
	 * @param [in] attributes The attribute map with known initial shape attributes. The ownership is transferred
	 * to the EncoderInfo object under construction.
	 */
	void setInitialShapeAttributes(prt::AttributeMap const* attributes);

	/**
	 * Adds an annotation to a specific option or initial shape attribute. Please use the prtx::AnnotationBuilder
	 * to create annotations. There is also the prtx::EncodeOptionsAnnotator helper class for a more
	 * convenient way to set annotations for encoder options.
	 *
	 * Initial shape attributes can be annotated with MANDATORY to make them a strict requirement for the encoder.
	 * If such annotated attributes are missing from an initial shape the prtx::Encoder::validate
	 * and prtx::Encoder::encode calls will fail.
	 *
	 * Note: To make an encoder option appear in the CityEngine UI (version 2014.0 and later),
	 * the following annotations are mandatory: LABEL, ORDER, GROUP and DESCRIPTION
	 *
	 * @param [in] key The encoder option or initial shape attribute key to annotate.
	 * @param [in] a The annotation instance. The ownership will be transferred. All annotations will be destroyed
	 * when the encoder info instance is destroyed.
	 */
	void addAnnotation(const std::wstring& key, const prt::Annotation* a);

	/**
	 * Registers a encoder-specific validator instance for the corresponding encoder.
	 * If no validator is set, only option key validation will take place,
	 * i.e. no option value validation will happen.
	 */
	void setValidator(prtx::EncodeOptionsValidatorPtr validator);

private:
	EncoderInfoImpl* mInfo;
};


} /* namespace prtx */


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_ENCODERINFOBUILDER_H_ */
