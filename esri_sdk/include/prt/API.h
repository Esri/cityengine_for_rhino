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

#ifndef PRT_API_H_
#define PRT_API_H_

#include "prt/prt.h"
#include "prt/Status.h"
#include "prt/InitialShape.h"
#include "prt/RuleFileInfo.h"
#include "prt/EncoderInfo.h"
#include "prt/DecoderInfo.h"
#include "prt/LogLevel.h"
#include "prt/LogHandler.h"
#include "prt/Callbacks.h"
#include "prt/Version.h"
#include "prt/OcclusionSet.h"


/** @namespace prt
 * @brief The Procedural Runtime API namespace.
 * The prt namespace contains the top level entry points into the Procedural Runtime API.
 */

namespace prt {


// --- MAIN PRT API CALLS


/**
 * Performs global Procedural Runtime initialization. Must only be called once per process,
 * else calls will fail with STATUS_ALREADY_INITIALIZED.
 * Is thread-safe.
 * Blocks until initialization is done.
 *
 * @param prtPlugins      List of paths to shared libraries or paths of directories to scan for shared libraries.
 * @param prtPluginsCount Number of plugin paths provided.
 * @param logLevel        #LogLevel to use.
 * @param[out] stat       Optional pointer to return the #Status.
 * @return                A "library handle" object which must be destroyed to unload the library and release resources.
 */
PRT_EXPORTS_API const Object* init(
		const wchar_t* const* prtPlugins,
		size_t                prtPluginsCount,
		LogLevel              logLevel,
		Status*               stat = nullptr
);

/**
 * Generate and encode procedural models for a number of initial shapes.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 *
 *
 * @sa Callbacks.
 *
 * @param initialShapes        The initial shapes for which the procedural models have to be generated.
 * @param initialShapeCount    The number of initial shapes in the array.
 * @param occlusionHandles     One occlusion handle for each initial shape. Set to nullptr for no inter-model occlusion tests.
 *                             Use generateOccluders() to obtain the occlusion handles.
 * @param encoders             A number (>= 1) of encoders to use to encode the generated model. The encoders are identified by their ID, see EncoderInfo::getID().
 * @param encodersCount        The number of encoder ids in the array.
 * @param encoderOptions       One AttributeMap with the enocder options for each encoder.
 * @param callbacks            Pointer to an instance of an implementation of the Callbacks interface.
 * @param cache                Pointer to an instance of an implementation of the Cache interface. See CacheObject.
 * @param occlSet              The OcclusionSet which contains the occlusionHandles. Set to nullptr for no inter-model occlusion tests.
 * @param generateOptions      An optional AttributeMap to set advanced generate options.<br>
 *                             int  "numberWorkerThreads": number of worker threads to internally use for generation. 0 = no multithreading = default<br>
                               bool "cacheShapeTree": cache the generated shapetrees. default = false.
 * @return A #Status code.
 */
PRT_EXPORTS_API Status generate(
		const InitialShape* const*  initialShapes,
		size_t                      initialShapeCount,
		const OcclusionSet::Handle* occlusionHandles,
		const wchar_t* const*       encoders,
		size_t                      encodersCount,
		const AttributeMap* const*  encoderOptions,
		Callbacks*                  callbacks,
		Cache*                      cache,
		const OcclusionSet*         occlSet,
		const AttributeMap*         generateOptions = nullptr
);

/**
 * Generate and encode procedural models for a number of initial shapes.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 *
 *
 * @sa Callbacks.
 *
 * @param initialShapes        The initial shapes for which the procedural models have to be generated.
 * @param initialShapeCount    The number of initial shapes in the array.
 * @param occlusionHandles     One occlusion handle for each initial shape. Set to nullptr for no inter-model occlusion tests.
 *                             Each occlusion handle corresponds to one of the given occlusion sets.
 *                             Use generateOccluders() to obtain the occlusion handles.
 * @param encoders             A number (>= 1) of encoders to use to encode the generated model. The encoders are identified by their ID, see EncoderInfo::getID().
 * @param encodersCount        The number of encoder ids in the array.
 * @param encoderOptions       One AttributeMap with the enocder options for each encoder.
 * @param callbacks            Pointer to an instance of an implementation of the Callbacks interface.
 * @param cache                Pointer to an instance of an implementation of the Cache interface. See CacheObject.
 * @param occlSets             The OcclusionSets which contain the occlusionHandles. Set to nullptr for no inter-model occlusion tests.
 *                             OcclusionSets are stored subsequently for each initial shape.
 *                             Each initial shape can have 0, 1 or >1 OcclusionSets.
 *                             The number of OcclusionSets of an initial shape is stored in occlSetCounts.
 * @param occlSetCounts        The number of OcclusionSets in occlSets for each initial shape. Set to nullptr for no inter-model occlusion tests.
 * @param generateOptions      An optional AttributeMap to set advanced generate options.<br>
 *                             int  "numberWorkerThreads": number of worker threads to internally use for generation. 0 = no multithreading = default<br>
                               bool "cacheShapeTree": cache the generated shapetrees. default = false.
 * @return A #Status code.
 */
PRT_EXPORTS_API Status generate(
		const InitialShape* const*  initialShapes,
		size_t                      initialShapeCount,
		const OcclusionSet::Handle* occlusionHandles,
		const wchar_t* const*       encoders,
		size_t                      encodersCount,
		const AttributeMap* const*  encoderOptions,
		Callbacks*                  callbacks,
		Cache*                      cache,
		const OcclusionSet* const*  occlSets,
		const size_t*               occlSetCounts,
		const AttributeMap*         generateOptions = nullptr
);

/**
 * Populate an OcclusionSet with occluder geometries for each initial shape and return an occlusion handle for each one.
 * These handles (and the OcclusionSet) can then be used to generate() models based on rules with inter-model
 * context and occlusion queries.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 *
 * @sa Callbacks.
 *
 * @param initialShapes         The initial shapes for which the occluders have to be generated.
 * @param initialShapeCount     The number of initial shapes in the array.
 * @param[out] occlusionHandles Must be preallocated to hold initialShapeCount occlusion Handles, one for each initial shape.
 *                              occlusionHandles[i] will receive the occlusion handle for initialShapes[i].
 * @param encoders              Optional encoders to run on the generated shapetree. Set to nullptr if no extra encoders
 *                              should be used.
 *                              Note that many encoders only consider leaf shapes and do not capture
 *                              the correct occluder geometry; these are typically the encoders from the
 *                              com.esri.prt.codecs plugin.
 *                              Encoders which traverse the whole shapetree (such as com.esri.prt.core.CGAPrintEncoder,
 *                              com.esri.prt.core.CGAErrorEncoder or com.esri.prt.core.CGAReportEncoder) can safely be used here.
 * @param encodersCount         The number of encoder ids in the array.
 * @param encoderOptions        Options for the additional encoders. Use nullptr if no encoders are used.
 * @param callbacks             Pointer to an instance of an implementation of the Callbacks interface.
 * @param cache                 Pointer to an instance of an implementation of the Cache interface. See CacheObject.
 * @param occlSet               The OcclusionSet which will be filled with the generated occlusionHandles.
 * @param generateOptions       An optional AttributeMap to set generate options.
 * @return A #Status code.
 */
PRT_EXPORTS_API Status generateOccluders(
		const InitialShape* const* initialShapes,
		size_t                     initialShapeCount,
		OcclusionSet::Handle*      occlusionHandles,
		const wchar_t* const*      encoders,
		size_t                     encodersCount,
		const AttributeMap* const* encoderOptions,
		Callbacks*                 callbacks,
		Cache*                     cache,
		OcclusionSet*              occlSet,
		const AttributeMap*        generateOptions = nullptr
);

/**
 * Creates a RuleFileInfo instance for a cgb file.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 * The returned instance must be destroyed by the caller.
 *
 * @param ruleFileURI URI to the cgb file, see \ref AppNotes for URI creation.
 * @param cache       Optional Cache instance.
 * @param[out] stat   Optional pointer to return the #Status.
 * @returns Pointer to a RuleFileInfo instance on success or 0 on failure.
 */
PRT_EXPORTS_API RuleFileInfo const* createRuleFileInfo(
		const wchar_t* ruleFileURI,
		Cache*         cache = nullptr,
		Status*        stat = nullptr
);

/**
 * Create a ResolveMap for a rpk. The entries will point into the rpk.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 * The returned instance must be destroyed by the caller.
 *
 * @param rpkURI               URI to the rpk, see \ref AppNotes for URI creation.
 * @param unpackFileSystemPath Optional. If non-null, the rpk gets unpacked to the given directory in the filesystem
 *                             and the ResolveMap entries will point to those files.
 * @param[out] stat            Optional pointer to return the #Status.
 * @returns Pointer to ResolveMap instance on success or 0 on failure.
 *
 */
PRT_EXPORTS_API ResolveMap const* createResolveMap(
		const wchar_t* rpkURI,
		const wchar_t* unpackFileSystemPath = nullptr,
		Status*        stat = nullptr
);


// --- EXTENSIONS

/**
 * Lists all registered encoders.
 * If the result does not fit into the allocated buffer, the result is truncated. the INOUT parameter resultSize must be compared to
 * the original buffer size after the call to detect this situation.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 *
 * @sa EncoderInfo, createEncoderInfo
 *
 * @param[in,out] result     Pointer to pre-allocated string to receive the semicolon-separated list of encoder ids.
 * @param[in,out] resultSize Pointer to size of reserved buffer; receives the size of the actual result (incl. terminating 0).
 * @param[out]    stat       Optional pointer to return the #Status.
 * @returns result.
 */
PRT_EXPORTS_API wchar_t* listEncoderIds(wchar_t* result, size_t* resultSize, Status* stat = nullptr);

/**
 * Lists all registered decoders.
 * If the result does not fit into the allocated buffer, the result is truncated. the INOUT parameter resultSize must be compared to
 * the original buffer size after the call to detect this situation.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 *
 * @sa DecoderInfo, createDecoderInfo
 *
 * @param[in,out] result     Pointer to pre-allocated string to receive the semicolon-separated list of decoder ids.
 * @param[in,out] resultSize Pointer to size of reserved buffer; receives the size of the actual result (incl. terminating 0).
 * @param[out]    stat       Optional pointer to return the #Status.
 * @returns result.
 */
PRT_EXPORTS_API wchar_t* listDecoderIds(wchar_t* result, size_t* resultSize, Status* stat = nullptr);

/**
 * Creates an EncoderInfo instance for the given encoder id.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 * The returned instance must be destroyed by the caller.
 *
 * @sa EncoderInfo, listEncoderIds
 *
 * @param encoderId  ID of encoder to get EncoderInfo for.
 * @param[out] stat  Optional pointer to return the #Status.
 * @returns Pointer to EncoderInfo instance on success or 0 on failure.
  */
PRT_EXPORTS_API EncoderInfo const* createEncoderInfo(const wchar_t* encoderId, Status* stat = nullptr);

/**
 * Creates an DecoderInfo instance for the given decoder id.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 * The returned instance must be destroyed by the caller.
 *
 * @sa DecoderInfo, listDecoderIds
 *
 * @param decoderId  ID of decoder to get DecoderInfo for.
 * @param[out] stat  Optional pointer to return the #Status.
 * @returns Pointer to DecoderInfo instance on success or 0 on failure.
  */
PRT_EXPORTS_API DecoderInfo const* createDecoderInfo(const wchar_t* decoderId, Status* stat = nullptr);


// --- TEXTURE HANDLING

/**
 * Creates a AttributeMap with the metadata for a given texture URI.
 * See prtx::Texture for a list of supported metadata attributes.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 * The returned instance must be destroyed by the caller.
 *
 * @sa getTexturePixeldata(), Attributable::PrimitiveType, prtx::Texture
 *
 * @param uri         URI of the texture, see \ref AppNotes for URI creation.
 * @param cache       Optional Cache instance.
 * @param[out] stat   Optional pointer to return the #Status.
 * @returns Pointer to a AttributeMap instance on success or 0 on failure.
 *
 */
PRT_EXPORTS_API const AttributeMap* createTextureMetadata(
		const wchar_t* uri,
		Cache*         cache = nullptr,
		Status*        stat = nullptr
);

/**
 * Reads and decodes a texture and copies the pixel data to the passed buffer.
 *
 * Must be called after init(), will return STATUS_NOT_INITIALIZED else.
 * Is thread-safe.
 *
 * @param uri         URI of the texture, see \ref AppNotes for URI creation.
 * @param[out] buffer Pointer to pre-allocated buffer. The needed size can be calculated with createTextureMetadata() (width*height*bytesPerPixel).
 * @param bufferSize  Size of the pre-allocated buffer.
 * @param cache       Optional Cache instance.
 * @returns A #Status code.
 *
 */
PRT_EXPORTS_API Status getTexturePixeldata(
		const wchar_t* uri,
		uint8_t*       buffer,
		size_t         bufferSize,
		Cache*         cache = nullptr
);

/**
 * Encodes the passed texture and writes the result to the passed SimpleOutputCallbacks instance.
 *
 * @param metadata       The textures metadata, must contain width/height/format attributes.
 *                       Other attributes are optional and it depends on the encoder whether they are supported.
 *                       See prtx::Texture for a list of supported metadata attributes.
 * @param pixeldata      The pixeldata to encode
 * @param encoderId      Id of the encoder to use.
 * @param encoderOptions Options for the encoder. This includes the filename.
 * @param soc            The SimpleOutputCallbacks instance which will receive the encoded texture.
 * @returns A #Status code.
 */
PRT_EXPORTS_API Status encodeTexture(
		const AttributeMap*    metadata,
		const uint8_t*         pixeldata,
		const wchar_t*         encoderId,
		const AttributeMap*    encoderOptions,
		SimpleOutputCallbacks* soc
);


// --- UTILS

/**
 * Returns a human readable description based on a #Status value.
 */
PRT_EXPORTS_API const char* getStatusDescription(Status stat);

/**
 * Returns the Version struct with information about the current Procedural Runtime.
 */
PRT_EXPORTS_API Version const* getVersion();


// --- LOGGING

/**
 * Register a log handler, can be called at any time
 * (also before init()).
 * Is thread-safe.
 *
 * @param logHandler LogHandler to register.
 * @returns A #Status code.
 */
PRT_EXPORTS_API Status	addLogHandler(LogHandler* logHandler);

/**
 * Remove a previously registered log handler, can be called
 * at any time (also before init()).
 * Is thread-safe.
 *
 * @param logHandler LogHandler to remove.
 * @returns A #Status code.
 */
PRT_EXPORTS_API Status	removeLogHandler(LogHandler* logHandler);

/**
 * Injects a log message into the PRT log dispatcher, can be called
 * at any time (also before init()).
 * Is thread-safe.
 *
 * @param msg   Message to log.
 * @param level #LogLevel of the message.
 * @returns A #Status code.
 */
PRT_EXPORTS_API Status	log(const wchar_t* msg, LogLevel level);

/**
 * Change the minimally active #LogLevel.
 */
PRT_EXPORTS_API Status	setLogLevel(LogLevel level);

/**
 * Query the minimally active #LogLevel.
 */
PRT_EXPORTS_API LogLevel getLogLevel(Status* status = nullptr);

} /* namespace prt */


#endif /* PRT_API_H_ */
