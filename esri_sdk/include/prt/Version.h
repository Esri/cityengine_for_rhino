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

#ifndef PRT_VERSIONINFO_H_
#define PRT_VERSIONINFO_H_

#include "prt/prt.h"


namespace prt {


/**
 * Provides static version information about the API/CGA/CGAC.
 */
struct PRT_EXPORTS_API Version {
	int				mVersionMajor;		///< Major PRT version number
	int				mVersionMinor;		///< Minor PRT version number
	int				mVersionBuild;		///< Build identifier.

	const char*		mName;				///< PRT product name
	const char*		mFullName;			///< PRT product name with full version
	const char*		mVersion;			///< "major.minor.build" version string
	const char*		mBuildConfig;
	const char*		mBuildOS;
	const char*		mBuildArch;			///< PRT CPU Architecture (x86 or x86_64)
	const char*		mBuildTC;
	const char*		mBuildDate;

	const wchar_t*	mwName;				///< PRT product name
	const wchar_t*	mwFullName;			///< PRT product name with full version
	const wchar_t*	mwVersion;			///< "major.minor.build" version string
	const wchar_t*	mwBuildConfig;
	const wchar_t*	mwBuildOS;
	const wchar_t*	mwBuildArch;		///< PRT CPU Architecture (x86 or x86_64)
	const wchar_t*	mwBuildTC;
	const wchar_t*	mwBuildDate;

	int				mCGAVersionMajor;	///< Latest supported major CGA version
	int				mCGAVersionMinor;	///< Latest supported minor CGA version
	const char*		mCGAVersion;		///< Latest supported full CGA version
	const wchar_t*	mwCGAVersion;		///< Latest supported full CGA version

	int				mCGACVersionMajor;	///< Major CGA compiler version this PRT release was built from
	int				mCGACVersionMinor;	///< Minor CGA compiler version this PRT release was built from
	const char*		mCGACVersion;		///< Full CGA compiler version this PRT release was built from
	const wchar_t*	mwCGACVersion;		///< Full CGA compiler version this PRT release was built from

	Version();
	~Version();
};


} // namespace prt


#endif /* PRT_VERSIONINFO_H_ */
