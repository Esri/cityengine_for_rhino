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

#ifndef PRTX_FILEEXTENSIONS_H_
#define PRTX_FILEEXTENSIONS_H_

#include "prtx/prtx.h"
#include "prtx/Types.h"

#include <string>
#include <vector>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4251)
#endif


namespace prtx {


/**
 * Helper class representing a list of file extensions.
 * Instances of this class are immutable.
 */
class PRTX_EXPORTS_API FileExtensions {
public:
	FileExtensions();
	virtual ~FileExtensions();

	/**
	 * Initializes the instance with exactly one extension.
	 * e must begin with a dot.
	 */
	FileExtensions(const std::wstring& e);

	/**
	 * Initializes the instance with an arbitrary number of extensions.
	 * All extensions must begin with a dot.
	 */
	FileExtensions(const WStringVector& v);

	/**
	 * Returns a string representation of the extension list.
	 * Each extension is delimited with a semi-colon, also if there is only one extension.
	 */
	const std::wstring& toString() const;

	/**
	 * Returns true if e is found in the list of known extensions.
	 * e must begin with a dot.
	 */
	bool contains(const std::wstring& e) const;

	/**
	 * Returns a reference to the extension list.
	 */
	const WStringVector& items() const;

	/**
	 * Helper to get reference to one extension item.
	 */
	const std::wstring& item(size_t i) const;

private:
	WStringVector mExts;
	mutable std::wstring* mStrCache;
};


} // namespace prtx

#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#endif /* PRTX_FILEEXTENSIONS_H_ */
