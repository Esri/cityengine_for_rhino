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

#ifndef PRT_CALLBACKS_H_
#define PRT_CALLBACKS_H_

#include "prt/prt.h"
#include "prt/Status.h"
#include "prt/ContentType.h"
#include "prt/CGAErrorLevel.h"

#include <cstddef>
#include <cstdint>


namespace prt {
	class AttributeMap;


/**
 * Callbacks are used to return data such as errors or reports during generation, to lookup coordinates
 * dependent on the client's coordinate system and to report progress and query for cancelation.
 *
 * A client must either use one of the provided implementations (FileOutputHandler, MemoryOutputHandler)
 * or implement this interface in order to use the generate() functions.
 *
 * An instance of a Callbacks subclass is instantiated & destroyed by the client. It usually lives for
 * a number of sequential generate() calls. All member functions must be thread-safe (can be called simultaneously by a number
 * of internal threads). One instance of a Callbacks must not be used by more than one concurrent generate() calls.
 */
class PRT_EXPORTS_API Callbacks {
public:
	/**
	 * Reports to the client that an application error occurred during generation of one of the initial shapes.
	 *
	 * @param isIndex Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param status  The erroneous status encountered during generation / encoding of the initial shape.
	 * @param message An error message.
	 * @returns client status (this value is currently ignored by PRT).
	 */
	virtual Status generateError(size_t isIndex, Status status, const wchar_t* message) = 0;
	/**
	 * Reports to the client that one of the assets used by the rule was erroneous.
	 * Called by com.esri.prt.core.CGAErrorEncoder (potentially by any encoder).
	 *
	 * @sa generate(), InitialShape, ResolveMap
	 *
	 * @param isIndex Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param level   Error/warning/info.
	 * @param key     The asset's key (symbolic name) into the initial shape's ResolveMap.
	 * @param uri     The asset's URI from the initial shape's ResolveMap.
	 * @param message An error message.
	 * @returns status.
	 */
	virtual Status assetError(size_t isIndex, CGAErrorLevel level, const wchar_t* key, const wchar_t* uri, const wchar_t* message) = 0;
	/**
	 * Reports to the client that a runtime cga error occuried during generation of a model.
	 * Called by com.esri.prt.core.CGAErrorEncoder (potentially by any encoder).
	 *
	 * @sa generate()
	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param level    Error/warning/info.
	 * @param methodId CGB-internal method id in which the error occurred.
	 * @param pc       SPU-internal program counter at which the error occured.
	 * @param message  An error message.
	 * @returns status.
	 */
	virtual Status cgaError(size_t isIndex, int32_t shapeID, CGAErrorLevel level, int32_t methodId, int32_t pc, const wchar_t* message) = 0;
	/**
	 * Reports to the client that a cga print operation/function was called during generation of a model.
	 * Called by com.esri.prt.core.CGAPrintEncoder (potentially by any encoder).
	 *
	 * @sa generate()
	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param txt      The printed text.
	 * @returns status.
	 */
	virtual Status cgaPrint(size_t isIndex, int32_t shapeID, const wchar_t* txt) = 0;
	/**
	 * Reports to the client that a cga report operation was called during generation of a model.
	 * Called by com.esri.prt.core.CGAReportEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key in the report operation.
	 * @param value    The value in the report operation.
	 * @returns status.
	 */
	virtual Status cgaReportBool(size_t isIndex, int32_t shapeID, const wchar_t* key, bool value) = 0;
	/**
	 * Reports to the client that a cga report operation was called during generation of a model.
	 * Called by com.esri.prt.core.CGAReportEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key in the report operation.
	 * @param value    The value in the report operation.
	 * @returns status.
	 */
	virtual Status cgaReportFloat(size_t isIndex, int32_t shapeID, const wchar_t* key, double value) = 0;
	/**
	 * Reports to the client that a cga report operation was called during generation of a model.
	 * Called by com.esri.prt.core.CGAReportEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key in the report operation.
	 * @param value    The value in the report operation.
	 * @returns status.
	 */
	virtual Status cgaReportString(size_t isIndex, int32_t shapeID, const wchar_t* key, const wchar_t* value) = 0;

	/**
	 * Reports the value of an evaluated attribute to the client.
	 * Called by com.esri.prt.core.AttributeEvalEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key of the attribute.
	 * @param value    The evaluated value.
	 * @returns status.
	 */
	virtual Status attrBool(size_t isIndex, int32_t shapeID, const wchar_t* key, bool value) = 0;
	/**
	 * Reports the value of an evaluated attribute to the client.
	 * Called by com.esri.prt.core.AttributeEvalEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key of the attribute.
	 * @param value    The evaluated value.
	 * @returns status.
	 */
	virtual Status attrFloat(size_t isIndex, int32_t shapeID, const wchar_t* key, double value) = 0;
	/**
	 * Reports the value of an evaluated attribute to the client.
	 * Called by com.esri.prt.core.AttributeEvalEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key of the attribute.
	 * @param value    The evaluated value.
	 * @returns status.
	 */
	virtual Status attrString(size_t isIndex, int32_t shapeID, const wchar_t* key, const wchar_t* value) = 0;


	/**
	 * Reports the value of an evaluated attribute to the client.
	 * Called by com.esri.prt.core.AttributeEvalEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key of the attribute.
	 * @param ptr      Pointer to the array.
	 * @param size     Size of the array.
	 * @param nRows    Number of rows in the array. This is 1 for 1d arrays. CGA arrays can actually be 2d!
	 * @returns status.
	 */
	virtual Status attrBoolArray(size_t isIndex, int32_t shapeID, const wchar_t* key, const bool* ptr, size_t size, size_t nRows) = 0;
	/**
	 * Reports the value of an evaluated attribute to the client.
	 * Called by com.esri.prt.core.AttributeEvalEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key of the attribute.
	 * @param ptr      Pointer to the array.
	 * @param size     Size of the array.
	 * @param nRows    Number of rows in the array. This is 1 for 1d arrays. CGA arrays can actually be 2d!
	 * @returns status.
	 */
	virtual Status attrFloatArray(size_t isIndex, int32_t shapeID, const wchar_t* key, const double* ptr, size_t size, size_t nRows) = 0;
	/**
	 * Reports the value of an evaluated attribute to the client.
	 * Called by com.esri.prt.core.AttributeEvalEncoder (potentially by any encoder).
 	 *
	 * @param isIndex  Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param shapeID  Unique shape id to identify the shape in the shape tree. See prtx::Shape.
	 * @param key      The key of the attribute.
	 * @param ptr      Pointer to the array.
	 * @param size     Size of the array.
	 * @param nRows    Number of rows in the array. This is 1 for 1d arrays. CGA arrays can actually be 2d!
	 * @returns status.
	 */
	virtual Status attrStringArray(size_t isIndex, int32_t shapeID, const wchar_t* key, const wchar_t* const* ptr, size_t size, size_t nRows) = 0;

	/**
	 * Geographical coordinates which might be looked up by cga.
	 */
	enum CoordSelector {
		PROJ_X,				///< The x coordinate of the projected coordinate system (i.e. cga's x axis).
		PROJ_Y,				///< The y coordinate of the projected coordinate system (i.e. cga's z axis).
		WGS84_LAT,			///< The WGS84 latitude coordinates.
		WGS84_LON,			///< The WGS84 longitude coordinates.
		WGS84_ALT_METER,	///< The WGS84 altitude in meters.
		LAYER_X,			///< The layer x coordinate. Layer coordinates are not used internally by cga but some encoders might rely on this.
		LAYER_Y,			///< The layer y coordinate.
		LAYER_Z				///< The layer z coordinate.
	};

	/**
	 * Function to convert world coordinates from the cga coordinate system (right handed y-up, meters) to
	 * a Cartesian scene coordinate system, geographical WGS84 coordinates or client-dependent layer coordinates.
	 * <b>Note:</b> The conversion to WGS84 coordinates is only correct if the input is in Web Mercator coordinates.
	 *
	 * Some cga functions (getGeoCord) rely on this. Additionally, some encoders use this functioanlity as well.
	 *
	 * @param      isIndex Initial shape index (relative to the initialShapes array in the generate() call)
	 * @param      sel     Selector for the desired coordinate.
	 * @param      x       X world coordinate in cga system.
	 * @param      y       Y world coordinate in cga system.
	 * @param      z       Z world coordinate in cga system.
     * @param[out] stat    Optional pointer to return the status.
	 * @returns The desired coordinate.
	 */
	virtual double cgaGetCoord(size_t isIndex, CoordSelector sel, double x, double y, double z, Status* stat = 0);

	/**
	 * Continuation possibilities.
	 */
	enum Continuation {
		CONTINUE,			///< Go on.
		CANCEL_AND_FINISH,	///< Cancel generation but finish encoding the already generated models.
		CANCEL_ASAP			///< Cancel generation and return as soon as possible, do not finalize encoding.
	};

	/**
	 * Called during the generation() call. Called at least once for every initial shape in the generate() call.
	 * Allows the client to abort the generate process.
	 *
	 * @param percentageCompleted A value in [0, 100]
	 * @returns The desired way to continue.
	 */
	virtual Continuation progress(float percentageCompleted);


protected:
	Callbacks();
	virtual ~Callbacks();
};


/**
 * An extension of the Callbacks interface which adds support for basic output. This is basically an abstraction
 * of typical file-IO.
 *
 * All functions are thread-safe.
 *
 * @sa FileOutputHandler, MemoryOutputHandler
 */
class PRT_EXPORTS_API SimpleOutputCallbacks : public Callbacks {
public:
	/**
	 * String encodings.
	 */
	enum StringEncoding {
		SE_NATIVE,	///< Native encoding.
		SE_UTF8,	///< UTF8 encoding.
		SE_UTF16	///< UTF16 encoding.
	};
	/**
	 * Open modes.
	 */
	enum OpenMode {
		OPENMODE_ALWAYS,			///< Always open.
		OPENMODE_IF_NOT_EXISTING	///< Only open if not existing.
	};
	/**
	 * Seek origin.
	 */
	enum SeekOrigin {
		SO_BEGIN,	///< Seek at beginning.
		SO_CURRENT,	///< Seek at current position.
		SO_END		///< Seek at end.
	};

	/**
	 * @returns true if the implementation is seekable, false if not.
	 */
	virtual bool canSeek() const = 0;
	/**
	 * Opens a named entity.
	 *
	 * @param      encoderId    The id of the calling encoder.
	 * @param      contentType  The content type which will be written.
	 * @param      name         Name of the entity.
	 * @param      enc          Optional encoding which should be applied to written strings.
	 * @param      mode         Optional mode for opening the entity.
	 * @param[out] stat         Optional pointer to return the status.
	 * @returns a unique handle to identify
	 */
	virtual uint64_t open(const wchar_t* encoderId, const ContentType contentType, const wchar_t* name, StringEncoding enc = SE_NATIVE, OpenMode mode = OPENMODE_ALWAYS, prt::Status* stat = 0) = 0;
	/**
	 * Write text to an output entity.
	 *
	 * @param handle Handle of entity.
	 * @param string Text to write.
	 * @returns status.
	 */
	virtual Status write(uint64_t handle, const wchar_t* string) = 0;
	/**
	 * Write binary data to an output entity.
	 *
	 * @param handle Handle of entity.
	 * @param buffer Pointer to data to write.
	 * @param size   Size of data to write in bytes to write.
	 * @returns status.
	 */
	virtual Status write(uint64_t handle, const uint8_t* buffer, size_t size) = 0;
	/**
	 * Sets the position in the entity.
	 * Only call this if canSeek() returns true.
	 *
	 * @param handle Handle of entity.
	 * @param offset Offset in bytes.
	 * @param origin The offset is relative to the origin.
	 * @returns status.
	 */
	virtual Status seek(uint64_t handle, int64_t offset, SeekOrigin origin) = 0;
	/**
	 * Returns the position in the entity.
	 * Only call this if canSeek() returns true.
	 *
	 * @param      handle Handle of entity.
	 * @param[out] stat   Optional pointer to return the status.
	 * @returns current position in the entity.
	 */
	virtual uint64_t tell(uint64_t handle, Status* stat = 0) = 0;
	/**
	 * Passes meta-data about the written entity to the client (e.g. polygon count, mesh names, etc.)
	 * All arrays in the meta-data are interepreted as follows: The i-th element of each array belongs to the same object
	 * and will be grouped. Therefore all arrays need to have equal length.
	 *
	 * @param      handle      Handle of entity.
	 * @param      isIndices   All indices that contributed to the contents of this entity
	 * @param      isCount     The length of the array given in isIndices
	 * @param      contentType The type for which meta-data is passed
	 * @param      stats       The meta-data that is passed to the client
	 * @returns    status
	 */
	virtual Status logStats(uint64_t handle, const size_t* isIndices, size_t isCount, ContentType contentType, const prt::AttributeMap* stats);
	/**
	 * Closes an entity.
	 *
	 * @param handle    Handle of entity.
	 * @param isIndices Pointer to array of initial shape indices (relative to the array in the generate() call),
	 *                  which contributed to the content in this entity.
	 * @param isCount   Number of indices in the array.
	 * @returns status.
	 */
	virtual Status close(uint64_t handle, const size_t* isIndices, size_t isCount) = 0;

	/**
	 * Opens an entity for cga error reporting.
	 *
	 * @param name The name of the entity.
	 * @returns status.
	 */
	virtual Status openCGAError(const wchar_t* name) = 0;
	/**
	 * Opens an entity where cga print operations/functions write to.
	 *
	 * @param name The name of the entity.
	 * @returns status.
	 */
	virtual Status openCGAPrint(const wchar_t* name) = 0;
	/**
	 * Opens an entity where cga report operations write to.
	 *
	 * @param name The name of the entity.
	 * @returns status.
	 */
	virtual Status openCGAReport(const wchar_t* name) = 0;

	virtual Status closeCGAError() = 0;
	virtual Status closeCGAPrint() = 0;
	virtual Status closeCGAReport() = 0;

protected:
	SimpleOutputCallbacks();
	virtual ~SimpleOutputCallbacks();
};


} // namespace prt


#endif /* PRT_CALLBACKS_H_ */
