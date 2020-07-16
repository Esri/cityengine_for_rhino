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

#ifndef BUILDER_H_
#define BUILDER_H_

#include <string>
#include <memory>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4275)
#endif


namespace prtx {


/**
 * Interface template for the Builder Pattern.
 * A builder is useful to create (multiple) immutable objects with similar states.
 */
template<typename C> class Builder {
public:
	Builder()                          = default;
	Builder(const Builder&)            = delete;
	Builder& operator=(const Builder&) = delete;

	/**
	 * Creates an instance of type C based on the current builder state. Does not modify the builder state.
	 *
	 * @param [out] warnings If warnings != 0, a builder implementation may emit warnings about the creation process.
	 * @return New instance of type C. Must be destroyed by the caller.
	 */
	virtual C* create(std::wstring* warnings = 0) const = 0;

	/**
	 * Creates and instance of type C based on the current builder state. Resets the builder state at the end.
	 *
	 * @param [out] warnings If warnings != 0, a builder implementation may emit warnings about the creation process.
	 * @return New instance of type C. Must be destroyed by the caller.
	 */
	virtual C* createAndReset(std::wstring* warnings = 0) = 0;
};


/**
 * Interface template for the Shared Pointer Builder Pattern.
 * Builder with additional member functions to create shared pointers for objects with base type prtx::Content
 */
template<typename C> class SharedPtrBuilder  {
public:
	SharedPtrBuilder()                                   = default;
	SharedPtrBuilder(const SharedPtrBuilder&)            = delete;
	SharedPtrBuilder& operator=(const SharedPtrBuilder&) = delete;

	/**
	 * Creates an instance of type C based on the current builder state. Does not modify the builder state.
	 *
	 * @param [out] warnings If warnings != 0, a builder implementation may emit warnings about the creation process.
	 * @return Shared pointer to a new instance of type C.
	 */
	virtual std::shared_ptr<C> createShared(std::wstring* warnings = 0) const = 0;

	/**
	 * Creates and instance of type C based on the current builder state. Resets the builder state at the end.
	 *
	 * @param [out] warnings If warnings != 0, a builder implementation may emit warnings about the creation process.
	 * @return Shared pointer to a new instance of type C.
	 *
	 */
	virtual std::shared_ptr<C> createSharedAndReset(std::wstring* warnings = 0) = 0;
};


} // namespace prtx


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* BUILDER_H_ */
