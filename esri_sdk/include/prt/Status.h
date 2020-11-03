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

#ifndef PRT_STATUS_H_
#define PRT_STATUS_H_

#include "prt/prt.h"


namespace prt {


/**
 * The Status enum is used to indicate whether a function was successful or not.
 * @sa getStatusDescription()
 */
enum PRT_EXPORTS_API Status {
	STATUS_OK,							    ///< Ok.
	STATUS_UNSPECIFIED_ERROR,			    ///< Unspecified error.
	STATUS_OUT_OF_MEM,					    ///< Out of memory.
	STATUS_NO_LICENSE,					    ///< No license.

	STATUS_NOT_ALL_IS_GENERATED,		    ///< Not all initial shapes could be generated. Check generate errors.
	STATUS_INCOMPATIBLE_IS,				    ///< Initial shape not compatible with encoder.

	STATUS_FILE_NOT_FOUND,				    ///< File not found.
	STATUS_FILE_ALREADY_EXISTS,			    ///< File already exists.
	STATUS_COULD_NOT_OPEN_FILE,			    ///< Could not open file.
	STATUS_COULD_NOT_CLOSE_FILE,		    ///< Could not close file.
	STATUS_FILE_WRITE_FAILED,			    ///< Could not write to file.
	STATUS_FILE_SEEK_FAILED,			    ///< Could not seek in file.
	STATUS_FILE_TELL_FAILED,			    ///< Could not tell write position in file.
	STATUS_NO_SEEK,						    ///< Seeking not supported by callback object.
	STATUS_EMPTY_FILE,					    ///< Empty file.
	STATUS_INVALID_URI,					    ///< Invalid URI.

	STATUS_STREAM_ADAPTOR_NOT_FOUND,	    ///< StreamAdaptor not found.
	STATUS_RESOLVEMAP_PROVIDER_NOT_FOUND,	///< ResolveMapProvider not found.
	STATUS_DECODER_NOT_FOUND,			    ///< Decoder not found.
	STATUS_ENCODER_NOT_FOUND,			    ///< Encoder not found.
	STATUS_UNABLE_TO_RESOLVE,			    ///< Unable to resolve.
	STATUS_CHECK_ERROR_PARAM,			    ///< Check error parameter.
	STATUS_KEY_NOT_FOUND,				    ///< Key not found.
	STATUS_KEY_ALREADY_TAKEN,			    ///< Key already taken.
	STATUS_KEY_NOT_SUPPORTED,			    ///< Key not supported.
	STATUS_STRING_TRUNCATED,			    ///< String truncated.
	STATUS_ILLEGAL_CALLBACK_OBJECT,		    ///< Illegal callback object.
	STATUS_ILLEGAL_LOG_HANDLER,			    ///< Illegal log handler.
	STATUS_ILLEGAL_LOG_LEVEL,			    ///< Illegal log level.
	STATUS_ILLEGAL_VALUE,				    ///< Illegal value.
	STATUS_NO_RULEFILE,					    ///< No rule file.
	STATUS_NO_INITIAL_SHAPE,			    ///< No initial shape.
	STATUS_CGB_ERROR,					    ///< CGB error.
	STATUS_NOT_INITIALIZED,				    ///< Not initialized.
	STATUS_ALREADY_INITIALIZED,			    ///< Already initialized.
	STATUS_INCONSISTENT_TEXTURE_PARAMS,	    ///< Inconsistent texture parameters.
	STATUS_CANCELED,					    ///< Canceled.
	STATUS_UNKNOWN_ATTRIBUTE,			    ///< Unknown attribute.
	STATUS_UNKNOWN_RULE,				    ///< Unknown rule.
	STATUS_ARGUMENTS_MISMATCH,			    ///< Arguments mismatch.
	STATUS_BUFFER_TO_SMALL,				    ///< Buffer to small.
	STATUS_UNKNOWN_FORMAT,				    ///< Unknown format.
	STATUS_ENCODE_FAILED,				    ///< Encode failed.
	STATUS_ATTRIBUTES_ALREADY_SET,		    ///< Attributes already set.
	STATUS_ATTRIBUTES_NOT_SET,			    ///< Attributes not set.
	STATUS_GEOMETRY_ALREADY_SET,		    ///< Geometry already set.
	STATUS_GEOMETRY_NOT_SET,			    ///< Geometry not set.
	STATUS_ILLEGAL_GEOMETRY,			    ///< Illegal geometry.
	STATUS_NO_GEOMETRY				    	///< No geometry.
};


} // namespace prt


#endif /* PRT_STATUS_H_ */
