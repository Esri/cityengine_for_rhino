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

#ifndef PRTX_EXCEPTION_H_
#define PRTX_EXCEPTION_H_

#include "prtx/prtx.h"
#include "prt/Status.h"
#include "prt/API.h"

#include <exception>
#include <string>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#   pragma warning (disable : 4275) // non dll-interface class used as base for dll-interface class
#endif

namespace prtx {


/**
 * Specialization of std::exception to transport a prt::Status code. If a StatusException is thrown
 * within prtx, the prt function which is the entry point will report the associated prt:::Status.
 */
class PRTX_EXPORTS_API StatusException : public std::exception {
public:
	explicit StatusException(prt::Status stat) : mStatus(stat) {}
	explicit StatusException(prt::Status stat, const std::string& details);

	virtual ~StatusException() throw() {}

	virtual const char* what() const throw();

	prt::Status getStatus() const { return mStatus; }

private:
	prt::Status  mStatus;
	std::string  mWhat;
};


} // namespace prtx

#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_EXCEPTION_H_ */
