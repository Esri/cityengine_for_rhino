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

#pragma once

#include <memory>


namespace prtx {

/**
 * Internal helper template for private implementations.
 */
template<typename T>
class OpaquePtr {
public:
	OpaquePtr();
  OpaquePtr(OpaquePtr<T> const& other);
  OpaquePtr<T>& operator=(OpaquePtr<T> const& other);

	template<typename ...Args>
	OpaquePtr(Args&&...);

	~OpaquePtr();

	T* operator->();
	T const* operator->() const;
	T& operator*();

private:

	std::unique_ptr<T> m;
};

} // namespace prtx


// helper macros to suppress MSVC warnings

#ifdef _MSC_VER

#	define BEGIN_SUPPRESS_EXPORT_WARNINGS \
	__pragma(warning(push)) \
	__pragma(warning (disable : 4251))

#	define END_SUPPRESS_EXPORT_WARNINGS \
	__pragma(warning(pop))

#else
#	define BEGIN_SUPPRESS_EXPORT_WARNINGS
#	define END_SUPPRESS_EXPORT_WARNINGS
#endif
