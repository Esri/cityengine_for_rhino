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

#ifndef PRTX_BUILTINATTRIBUTES_H_
#define PRTX_BUILTINATTRIBUTES_H_

#include "prtx/prtx.h"
#include "prtx/Types.h"

#include "prt/Attributable.h"

#include <string>


namespace prtx {


/**
 * Base class for builtin (cga) attributes.
 */
class PRTX_EXPORTS_API BuiltinAttributes {
protected:
	BuiltinAttributes();
	virtual ~BuiltinAttributes();

	virtual void fillBuiltinKeys(WStringVector& keys) const = 0;
	virtual prt::Attributable::PrimitiveType getBuiltinType(const std::wstring& k) const = 0;
};


/**
 * Base class for builtin (cga) attribute getters.
 */
class PRTX_EXPORTS_API BuiltinAttributesGetters : public BuiltinAttributes {
protected:
	BuiltinAttributesGetters() {}
	virtual ~BuiltinAttributesGetters() {}

	virtual bool getBuiltinBool(const std::wstring& k, bool& found) const = 0;
	virtual double getBuiltinFloat(const std::wstring& k, bool& found) const = 0;
	virtual const std::wstring& getBuiltinString(const std::wstring& k, bool& found) const = 0;
};


/**
 * Base class for builtin (cga) attribute setters.
 */
class PRTX_EXPORTS_API BuiltinAttributesSetters /*: public BuiltinAttributes*/ {
protected:
	BuiltinAttributesSetters() {}
	virtual ~BuiltinAttributesSetters() {}

	virtual bool setBuiltinBool(const std::wstring& k, bool val) = 0;
	virtual bool setBuiltinFloat(const std::wstring& k, double val) = 0;
	virtual bool setBuiltinString(const std::wstring& k, const std::wstring& found) = 0;
};

} // namespace prtx


#endif /* PRTX_BUILTINATTRIBUTES_H_ */
