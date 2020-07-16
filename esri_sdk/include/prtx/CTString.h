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

#ifndef SRC_PRTX_CTSTRING_H_
#define SRC_PRTX_CTSTRING_H_

#include "prtx/prtx.h"
#include "prtx/Content.h"

#include <vector>
#include <memory>
#include <string>

namespace prtx {


class CTString;
typedef std::shared_ptr<CTString> CTStringPtr;
typedef std::vector<CTStringPtr> CTStringPtrVector;

class PRTX_EXPORTS_API CTString : public Content {
public:
	virtual ~CTString();

	virtual std::shared_ptr<const std::wstring> getString() const = 0;

protected:
	CTString();
};


} /* namespace prtx */





#endif /* SRC_PRTX_STRING_H_ */
