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

#ifndef PRTX_EXTENSIONMANAGER_H_
#define PRTX_EXTENSIONMANAGER_H_

#include "prtx/prtx.h"
#include "prtx/URI.h"
#include "prtx/ExtensionFactory.h"
#include "prtx/StreamAdaptor.h"
#include "prtx/ResolveMapProvider.h"
#include "prtx/Decoder.h"
#include "prtx/Encoder.h"

#include "prt/AttributeMap.h"
#include "prt/EncoderInfo.h"
#include "prt/DecoderInfo.h"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4275)
#endif


namespace prtx {


/**
 * The PRT plugin (extension) manager. This class is used as a singleton, see instance().
 * Its main use case is the creation of StreamAdaptors, ResolveMapProviders, Decoders and Encoders based on their extension IDs and URIs.
 *
 * \sa prtx::Extension
 * \sa prtx::Encoder
 * \sa prtx::Decoder
 * \sa prtx::StreamAdaptor
 * \sa prtx::ResolveMapProvider
 */
class PRTX_EXPORTS_API ExtensionManager {
public:
	ExtensionManager(const ExtensionManager&) = delete;
	ExtensionManager& operator=(const ExtensionManager&) = delete;
	virtual ~ExtensionManager() = default;

	/**
	 * Access to the ExtensionManager singleton
	 */
	static ExtensionManager& instance();

	/**
	 * Registers an extension factory class. Use this member function to register any custom
	 * extensions. Factories will be released on PRT shutdown (i.e. on destruction of the PRT license handle, see prt::init).
	 *
	 * @param factory The instance of the factory to register. Ownership is transferred to the ExtensionManager.
	 */
	virtual void addFactory(prtx::ExtensionFactory* factory) = 0;

	/**
	 * List all registered extensions of type prtx::Extension::ET_ENCODER.
	 */
	virtual void listEncoderIds(std::vector<std::wstring>& result) const = 0;


	/**
	 * List all registered extensions of type prtx::Extension::ET_DECODER.
	 */
	virtual void listDecoderIds(std::vector<std::wstring>& result) const = 0;

	/**
	 * Creates an encoder info object based on an encoder ID.
	 * Throws prtx::StatusException if the id is unknown or if the corresponding EncoderFactory
	 * is not setup properly.
	 *
	 * @param [in] id The ID of the new info instance.
	 * @return A new EncoderInfo instance. Must be destroyed by the caller.
	 */
	virtual prt::EncoderInfo const* createEncoderInfo(const std::wstring& id) const = 0;

	/**
	 * Creates a decoder info object based on an decoder ID.
	 *
	 * @param [in] id The ID of the new decoder info instance.
	 * @return A new DecoderInfo instance. Must be destroyed by the caller.
	 */
	virtual prt::DecoderInfo const* createDecoderInfo(const std::wstring& id) const = 0;

	/**
	 * Creates an encoder instance based on its identifier. See listEncoderIds().
	 *
	 * @return A shared pointer to the new encoder instance.
	 */
	virtual EncoderPtr createEncoder(
			const std::wstring&			id,			///< [in] The ID of the encoder to be instantiated.
			const prt::AttributeMap*	options,	///< [in] Encoder options for the new instance.
			prt::Callbacks* const		callbacks	///< [in] Callbacks to be used by the new encoder instance.
	) const = 0;

	/**
	 * Creates a decoder instance which matches the content type and can handle a specific URI.
	 * If multiple compatible decoders are available the one with the highest merit is returned.
	 *
	 * @param [in] ct The content type of the desired decoder instance.
	 * @param [in] uri The URI which the decoder must be able to handle.
	 * @return A shared pointer to a new decoder instance.
	 *
	 * \sa prtx::Extension::getMerit()
	 */
	virtual DecoderPtr createDecoder(prt::ContentType ct, const prtx::URIPtr& uri) const = 0;

	/**
	 * Create a decoder directly by its identifier. See listDecoderIds().
	 *
	 * @return A shared pointer to the new decoder instance.
	 */
	virtual DecoderPtr createDecoder(const std::wstring& id) const = 0;

	/**
	 * Create a stream adaptor which matches the URI. If multiple stream adaptors can handle the URI
	 * the one with the highest merit is returned.
	 *
	 * @param [in] uri The uri which the stream adaptor must be able to handle.
	 * @return A shared pointer to the new stream adaptor instance.
	 */
	virtual StreamAdaptorPtr createStreamAdaptor(const prtx::URIPtr& uri) const = 0;

	/**
	 * Create a resolve map provider which matches the URI. If multiple resolve map providers can handle the URI
	 * the one with the highest merit is returned.
	 *
	 * @param [in] uri The uri which the resolve map provider must be able to handle.
	 * @return A shared pointer to the new resolve map provider instance.
	 */
	virtual ResolveMapProviderPtr createResolveMapProvider(const prtx::URIPtr& uri) const = 0;

public:
	static void setInstance(ExtensionManager* theInstance);
	virtual void init(const wchar_t* const* prtPlugins, size_t prtPluginsCount) = 0;

protected:
	ExtensionManager() = default;
};


} // namespace prtx


#ifdef _MSC_VER
#pragma warning(pop)
#endif


#endif /* PRTX_EXTENSIONMANAGER_H_ */
