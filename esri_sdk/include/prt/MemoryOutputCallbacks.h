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

#ifndef PRT_MEMORYOUTPUTCALLBACKS_H_
#define PRT_MEMORYOUTPUTCALLBACKS_H_

#include "prt/prt.h"
#include "prt/Object.h"
#include "prt/Callbacks.h"
#include "prt/ContentType.h"

#include <cstddef>
#include <cstdint>


namespace prt {

/**
 * Memory block-based implementation of the SimpleOutputCallbacks interface.
 * After encoding (i.e. after the generate() call), the named blocks can be read by the client with a number of
 * dedicated calls.
 *
 * In order to free the blocks the MemoryOutputCallbacks instance has to be destroyed.
 */
class PRT_EXPORTS_API MemoryOutputCallbacks: public SimpleOutputCallbacks, public Object {
public:
	/**
	 * Creates a MemoryCallbacks instance. The returned instance must be destroyed by the caller.
	 *
	 * @param cgaOutputEnc           Optional encoding for cgaPrint, cgaReport and cgaError (string) calls.
	 * @param generateErrorBlockName Optional: Specifies the block name used to hold the contents of the
	 *                               generateError callback. If set to nullptr, no block will be created.
	 * @param[out] status            Optional pointer to return the status.
 	 * @returns a pointer to the instance or nullptr in case of error.
	 */
	static MemoryOutputCallbacks* create(StringEncoding cgaOutputEnc = SE_NATIVE,
	                                     const wchar_t* generateErrorBlockName = nullptr, Status* status = nullptr);

	/**
	 * @sa getBlock
	 * @returns number of allocated blocks.
	 */
	virtual size_t           getNumBlocks() const = 0 ;
	/**
	 * @sa getNumBlocks
	 * @param      blockNum     Index of block to get.
	 * @param[out] outBlockSize Pointer to return size of block in bytes.
	 * @returns Pointer to the block.
	 */
	virtual const uint8_t*   getBlock(size_t blockNum, size_t* outBlockSize) const = 0;
	/**
	 * @param blockNum     Index of block.
	 * @returns Encoder ID of block.
	 */
	virtual const wchar_t*   getBlockEncoderId(size_t blockNum) const = 0;
	/**
	 * @param blockNum     Index of block.
	 * @returns ContentType of block.
	 */
	virtual prt::ContentType getBlockContentType(size_t blockNum) const = 0;
	/**
	 * @sa getBlockNumInitialShapeIndices
	 * @param blockNum     Index of block.
	 * @returns Pointer to array of initial shape indices.
	 */
	virtual const size_t*    getBlockInitialShapeIndices(size_t blockNum) const = 0;
	/**
	 * @sa getBlockInitialShapeIndices
	 * @param blockNum     Index of block.
	 * @returns Number of initial shape indices for block.
	 */
	virtual size_t           getBlockNumInitialShapeIndices(size_t blockNum) const = 0;
	/**
	 * @param blockNum     Index of block.
	 * @returns name of block.
	 */
	virtual const wchar_t*   getBlockName(size_t blockNum) const = 0;

protected:
	MemoryOutputCallbacks() = default;
	virtual ~MemoryOutputCallbacks() = default;
};


} // namespace prt


#endif /* PRT_MEMORYOUTPUTCALLBACKS_H_ */
