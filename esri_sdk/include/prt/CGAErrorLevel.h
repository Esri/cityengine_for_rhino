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

#ifndef PRT_CGAERRORLEVEL_H_
#define PRT_CGAERRORLEVEL_H_

#include "prt/prt.h"

namespace prt {
	/**
	 * Error level for CGAErrors.
	 */
	enum PRT_EXPORTS_API CGAErrorLevel {
		CGAERROR,	///< Error level. It is not possible to generate if the rule file contains errors.
		CGAWARNING,	///< Warning level. Warnings indicate potential problems and do not prevent generation.
		CGAINFO		///< Info level.
	};
};

#endif /* PRT_CGAERRORLEVEL_H_ */
