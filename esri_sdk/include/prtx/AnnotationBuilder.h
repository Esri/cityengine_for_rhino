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

#ifndef PRTX_ANNOTATIONBUILDER_H_
#define PRTX_ANNOTATIONBUILDER_H_

#include "prt/Annotation.h"

#include "prtx.h"
#include "prtx/Builder.h"

#include <string>
#include <vector>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4251 4275)
#endif


namespace prtx {


/**
 * Use this builder class to annotate attributes or encoder options.
 */
class PRTX_EXPORTS_API AnnotationBuilder : public Builder<const prt::Annotation> {
public:
	AnnotationBuilder();
	AnnotationBuilder(const AnnotationBuilder&) = delete;
	AnnotationBuilder& operator=(const AnnotationBuilder&) = delete;
	virtual ~AnnotationBuilder();

	/**
	 * Sets the annotation name.
	 */
	void setName(const std::wstring& name);

	/**
	 * Adds any annotation argument created by the AnnotationArgumentBuilder.
	 */
	void addArgument(const prt::AnnotationArgument* arg);

	/**
	 * Convenience member function to add a string annotation argument.
	 */
	void addString(const std::wstring& key, const std::wstring& val);

	/**
	 * Convenience member function to add a double annotation argument.
	 */
	void addFloat(const std::wstring& key, double val);

	/**
	 * Convenience member function to add a bool annotation argument.
	 */
	void addBool(const std::wstring& key, bool val);

	/**
	 * Creates an annotation instance but does not change the state of the builder.
	 */
	virtual const prt::Annotation* create(std::wstring* warnings = 0) const;

	/**
	 * Creates an annotation instance and resets the builder.
	 */
	virtual const prt::Annotation* createAndReset(std::wstring* warnings = 0);

	/**
	 * Convenience member function to create an annotation without any parameters.
	 */
	static const prt::Annotation* createAnnotation(const std::wstring& name);

	/**
	 * Convenience member function to create an annotation with a single string parameter.
	 */
	static const prt::Annotation* createAnnotation(const std::wstring& name, const std::wstring& key, const std::wstring& val);

	/**
	 * Convenience member function to create an annotation with a single double parameter.
	 */
	static const prt::Annotation* createAnnotation(const std::wstring& name, const std::wstring& key, double val);

	/**
	 * Convenience member function to create an annotation with a single boolean parameter.
	 */
	static const prt::Annotation* createAnnotation(const std::wstring& name, const std::wstring& key, bool val);

private:
	std::wstring mName;
	std::vector<const prt::AnnotationArgument*> mArguments;
};


/**
 * @namespace prtx::AnnotationArgumentBuilder
 * @brief Helper functions to create combinations of annotation parameters.
 */
namespace AnnotationArgumentBuilder {

prt::AnnotationArgument* createStringArgument(const std::wstring& key, const std::wstring& val);
prt::AnnotationArgument* createFloatArgument (const std::wstring& key, double val);
prt::AnnotationArgument* createBoolArgument  (const std::wstring& key, bool val);

} // namespace AnnotationArgumentBuilder


} // namespace prtx


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_ANNOTATIONBUILDER_H_ */
