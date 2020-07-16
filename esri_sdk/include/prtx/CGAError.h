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

#ifndef PRTX_CGAERROR_H_
#define PRTX_CGAERROR_H_

#include "prtx/URI.h"

#include "prt/CGAErrorLevel.h"

#include <vector>
#include <string>
#include <memory>


namespace prtx {


class CGAError;
typedef std::shared_ptr<CGAError> CGAErrorPtr;
typedef std::vector<CGAErrorPtr>    CGAErrorPtrVector;


/**
 * Class to describe a CGA runtime error.
 */
class CGAError {
public:
	CGAError(prt::CGAErrorLevel lev, int32_t mId, int32_t pc, const std::wstring& message) :
		mLevel(lev), mMethodId(mId), mPC(pc), mErrorMessage(message) {}

	prt::CGAErrorLevel mLevel;			//!< The error level.
	int32_t 		   mMethodId;       //!< The method id.
	int32_t 		   mPC;             //!< The SPU program counter.
	std::wstring	   mErrorMessage;   //!< The error message.
};

class AssetError;
typedef std::shared_ptr<AssetError> AssetErrorPtr;
typedef std::vector<AssetErrorPtr>    AssetErrorPtrVector;

/**
 * Class to describe an Asset error. These are typically issued by a decoder.
 */
class AssetError {
public:
	AssetError(prt::CGAErrorLevel lev, const std::wstring& key, const prtx::URIPtr& uri, const std::wstring& message) :
		mLevel(lev), mKey(key), mURI(uri), mErrorMessage(message) {}

	prt::CGAErrorLevel mLevel;      //!< The error level.
	std::wstring mKey;              //!< The culprit key into the ResolveMap.
	prtx::URIPtr mURI;              //!< The culprit URI.
	std::wstring mErrorMessage;     //!< The error message.
};

}

#endif /* PRTX_CGAERROR_H_ */
