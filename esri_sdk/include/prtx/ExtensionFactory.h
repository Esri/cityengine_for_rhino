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

#ifndef PRTX_EXTENSIONFACTORY_H_
#define PRTX_EXTENSIONFACTORY_H_

#include "prtx/prtx.h"
#include "prtx/URI.h"
#include "prtx/Extension.h"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4275)
#endif


namespace prtx {


/**
 * This is the base class for all extension factories. Usually it should
 * not be sub-classed directly, use StreamAdaptorFactory, ResolveMapProviderFactory,
 * DecoderFactory, or EncoderFactory for your own extensions.
 *
 * All ExtensionFactory subclasses must be used as singletons and are
 * typically directly passed into prtx::ExtensionManager::addFactory.
 *
 * ExtensionFactories and subclasses must be immutable.
 *
 * \sa prtx::ExtensionManager
 * \sa prtx::ClassUtils::Singleton
 */

class PRTX_EXPORTS_API ExtensionFactory {
protected:
	ExtensionFactory() = default;

public:
	ExtensionFactory(const ExtensionFactory&) = delete;
	ExtensionFactory& operator=(const ExtensionFactory&) = delete;
	virtual ~ExtensionFactory() = default;

	/**
	 * Subclasses must override this member function to create an instance of the corresponding extension.
	 */
	virtual Extension* create() const = 0;

	/**
	 * Returns the unique ID of the extension instances this factory will create.
	 */
	virtual const std::wstring& getID() const = 0;

	/**
	 * Returns the human readable name of the extension.
	 */
	virtual const std::wstring& getName() const = 0;

	/**
	 * Returns a human readable description of the extension
	 */
	virtual const std::wstring& getDescription() const = 0;

	/**
	 * Returns the content type of this extension.
	 */
	virtual prt::ContentType getContentType() const = 0;

	/**
	 * Returns the type of this extension.
	 */
	virtual prtx::Extension::ExtensionType getExtensionType() const = 0;

	/**
	 * An extension can override and increase getMerit to influence
	 * the resolve order of an URI. Extensions with higher merits have
	 * higher priority.
	 */
	virtual float getMerit() const { return getDefaultMerit(); }

	/**
	 * Returns the default merit for all extensions (1.0).
	 */
	static float getDefaultMerit();

	/**
	 * Returns true, if the extension can handle the resource described by the URI.
	 * Returns false, if the uri is invalid.
	 */
	virtual bool canHandleURI(prtx::URIPtr uri) const = 0;
};


} /* namespace prtx */


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_EXTENSIONFACTORY_H_ */
