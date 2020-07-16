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

#ifndef PRT_ANNOTATION_H_
#define PRT_ANNOTATION_H_

#include "prt/prt.h"
#include "prt/Object.h"

namespace prt {


/**
 * Enumerates types of annotation arguments.
 */
enum AnnotationArgumentType {
	AAT_VOID,	     ///< Void type.
	AAT_BOOL,	     ///< Boolean type.
	AAT_FLOAT,	     ///< Float type.
	AAT_STR,	     ///< String type.
	AAT_INT,	     ///< Integer type.
	AAT_UNKNOWN,	 ///< Unknown type.
	AAT_BOOL_ARRAY,	 ///< Boolean array type.
	AAT_FLOAT_ARRAY, ///< Float array type.
	AAT_STR_ARRAY,	 ///< String array type.
};


class PRT_EXPORTS_API AnnotationArgument : public Object {
public:
	virtual AnnotationArgumentType		getType()				const = 0;
	virtual const wchar_t*				getKey()				const = 0;
	virtual bool						getBool()				const = 0;
	virtual double						getFloat()				const = 0;
	virtual const wchar_t*				getStr()				const = 0;

protected:
	virtual ~AnnotationArgument() { }
};


class PRT_EXPORTS_API Annotation : public Object {
public:
	virtual const wchar_t*				getName()				const = 0;
	virtual size_t						getNumArguments()		const = 0;
	virtual const AnnotationArgument*	getArgument(size_t i)	const = 0;

protected:
	virtual ~Annotation() { }
};


} // namespace prt


#endif /* PRT_ANNOTATION_H_ */
