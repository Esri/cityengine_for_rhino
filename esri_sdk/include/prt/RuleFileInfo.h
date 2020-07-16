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

#ifndef PRT_RULEFILEINFO_H_
#define PRT_RULEFILEINFO_H_

#include "prt/prt.h"
#include "prt/Object.h"
#include "prt/Annotation.h"


class ClassFileReader;


namespace prt {

/**
 * The RuleFileInfo class allows for browsing through the rules and attributes contained within a rule file.
 * Both rules and attributes can be annotated. For a list of defined annotations please visit
 * the @cgaref{cga_annotations.html,CGA reference}.
 */
class PRT_EXPORTS_API RuleFileInfo : public Object {
public:
	class PRT_EXPORTS_API Parameter {
	public:
		virtual AnnotationArgumentType	getType()               const = 0;
		virtual const wchar_t*			getName()               const = 0;
		virtual size_t					getNumAnnotations()     const = 0;
		virtual const Annotation*		getAnnotation(size_t i) const = 0;

	protected:
		virtual ~Parameter() { }
	};


	class PRT_EXPORTS_API Entry {
	public:
		virtual AnnotationArgumentType	getReturnType()         const = 0;
		virtual const wchar_t*			getName()               const = 0;
		virtual size_t					getNumParameters()      const = 0;
		virtual const Parameter*		getParameter(size_t i)  const = 0;
		virtual size_t					getNumAnnotations()     const = 0;
		virtual const Annotation*		getAnnotation(size_t i) const = 0;

	protected:
		virtual ~Entry() { }
	};



	virtual size_t					getNumAttributes()      const = 0;
	virtual const Entry*			getAttribute(size_t i)  const = 0;
	virtual size_t					getNumRules()           const = 0;
	virtual const Entry*			getRule(size_t i)       const = 0;
	virtual size_t					getNumAnnotations()     const = 0;
	virtual const Annotation*		getAnnotation(size_t i) const = 0;

protected:
	virtual ~RuleFileInfo() { }
};


} /* namespace prt */


#endif /* PRT_RULEFILEINFO_H_ */
