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

#ifndef PRTX_ITEXTURE_H_
#define PRTX_ITEXTURE_H_

#include "prtx/prtx.h"
#include "prtx/URI.h"
#include "prtx/Attributable.h"
#include "prtx/Content.h"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4275 4251)
#endif

#include <memory>


namespace prtx {


class Texture;
typedef std::shared_ptr<Texture> TexturePtr;			///< shared Texture pointer
typedef std::vector<TexturePtr>  TexturePtrVector;		///< vector of shared Texture pointers


/**
 * The read-only Texture class allows for Texture access.
 * Every texture supports the essential attributes
 *  * "width"         - PT_INT
 *  * "height"        - PT_INT
 *  * "bytesPerPixel" - PT_INT     (used to be "depth", deprecated)
 *  * "format"        - PT_STRING  (which is PixelFormat as wstring)
 *
 * Some textures also support some geo-referencing attributes:
 *  * "offset"        - PT_FLOAT_ARRAY[3]
 *  * "scale"         - PT_FLOAT_ARRAY[3]
 *  * "noDataValue"   - PT_FLOAT_ARRAY[3]
 *  * "geoTransform"  - PT_FLOAT_ARRAY[6]           (see GDALDataset::GetGeoTransform() documentation)
*   * "unit"          - PT_STRING
 *  * "projectionRef" - PT_STRING
 *
 */
class PRTX_EXPORTS_API Texture : public Attributable, public ComparableContent {
protected:
	Texture();

public:
	virtual ~Texture();

	/**
	 * Supported pixel formats.
	 */
	enum PixelFormat {
		GREY8	= 1, ///< 1 band, 1 byte per pixel, type UInt8, little-endian
		GREY16	= 2, ///< 1 band, 2 bytes per pixel, type UInt16, little-endian
		RGB8	= 3, ///< 3 bands, 3 bytes per pixel, type UInt8, little-endian
		RGBA8	= 4, ///< 4 bands, 4 bytes per pixel, type Uint8, little-endian
		FLOAT32 = 5, ///< 1 band, 4 bytes per pixel, type Float32, little-endian
	};

	static const std::wstring METADATA_KEY_WIDTH;			///< metadata attribute "width" (PT_INT), corresponds to getWidth()
	static const std::wstring METADATA_KEY_HEIGHT;			///< metadata attribute "height" (PT_INT), corresponds to getHeight()
	static const std::wstring METADATA_KEY_BYTESPERPIXEL;	///< metadata attribute "bytesPerPixel" (PT_INT), corresponds to getBytesPerPixel()
	static const std::wstring METADATA_KEY_FORMAT;			///< metadata attribute "format" (PT_STRING), i.e. PixelFormat as wstring

	static const std::wstring METADATA_KEY_OFFSET;			///< metadata attribute "offset"
	static const std::wstring METADATA_KEY_SCALE;			///< metadata attribute "scale"
	static const std::wstring METADATA_KEY_NODATAVALUE;		///< metadata attribute "noDataValue"
	static const std::wstring METADATA_KEY_GEOTRANSFORM;	///< metadata attribute "geoTransform", see GDALDataset::GetGeoTransform() documentation
	static const std::wstring METADATA_KEY_UNIT;			///< metadata attribute "unit"
	static const std::wstring METADATA_KEY_PROJECTIONREF;	///< metadata attribute "projectionRef"

	// deprecated, do not use anymore
	static const std::wstring METADATA_KEY_DEPTH;			///< (deprecated) metadata attribute "depth" (PT_INT), corresponds to getBytesPerPixel()

	static const double		  METADATA_NODATAVALUE_NONE;	///< sentinel value for the METADATA_KEY_NODATAVALUE attribute = "there is *no* noDataValue").

	/**
	 * @param format PixelFormat to query.
	 * @returns number of bytes per pixel used by the format.
	 */
	static uint8_t getBytesPerPixel(PixelFormat format);
	/**
	 * @param format PixelFormat to query.
	 * @returns true if the format has an alpha channel, false if not.
	 */
	static bool hasAlpha(PixelFormat format);
	/**
	 * @param format PixelFormat to query.
	 * @returns string representation  (i.e. enum value as string) of the format.
	 */
	template<typename S> static S getFormatAsString(PixelFormat format);
	/**
	 * Throws an exception if string is invalid.
	 * @param string PixelFormat as string (i.e. enum value as string).
	 * @returns PixelFormat.
	 */
	static PixelFormat getFormatFromString(const wchar_t* string);


	/**
	 * @returns the absolute name (URI) of the texture.
	 */
	virtual prtx::URIPtr getURI() const = 0;

	/**
	 * @returns true if the texture represents a valid texture (valid URI/Name and valid data), false otherwise.
	 */
	virtual bool 				isValid() const = 0;

	/**
	 * @returns pointer to the pixel buffer.
	 */
	virtual const uint8_t*		getBuffer() const = 0;
	/**
	 * @returns size of pixel buffer in bytes.
	 */
	virtual size_t				getBufferSize() const = 0;

	/**
	 * @returns width in pixels.
	 */
	virtual uint32_t			getWidth()  const = 0;
	/**
	 * @returns height in pixels.
	 */
	virtual uint32_t			getHeight() const = 0;
	/**
	 * @returns PixelFormat.
	 */
	virtual PixelFormat			getFormat() const = 0;

	/**
	 * @returns warnings which occurred during loading of the texture.
	 */
	virtual const std::wstring&	getWarnings() const = 0;
};


template<> PRTX_EXPORTS_API std::wstring Texture::getFormatAsString<std::wstring>(PixelFormat format);
template<> PRTX_EXPORTS_API std::string Texture::getFormatAsString<std::string>(PixelFormat format);


/**
 * Builder for Texture instances. Set metadata values first and then use a create function to createXXX() an instance.
 * See Texture for valid metadata keys.
 */
class PRTX_EXPORTS_API TextureBuilder : public AttributableSetter {
public:
	TextureBuilder();
	TextureBuilder(const TextureBuilder&) = delete;
	TextureBuilder& operator=(const TextureBuilder&) = delete;
	virtual ~TextureBuilder();

	/**
	 * Creates a Texture instance with the currently set metadata.
	 * @param uri    URI of the Texture.
	 * @param width  Width of the Texture in pixels.
	 * @param height Height of the Texture in pixels.
	 * @param format PixelFormat of the Texture.
	 * @param buffer Pixelbuffer. Size must be (width*height*Texture::getBytesPerPixel(format)). Instance takes ownership.
	 *
	 */
	TexturePtr createTexture(const prtx::URIPtr& uri, uint32_t width, uint32_t height, const Texture::PixelFormat& format, uint8_t* buffer);
	/**
	 * Creates a Texture instance with the currently set metadata and resets the builder.
	 * @param uri    URI of the Texture.
	 * @param width  Width of the Texture in pixels.
	 * @param height Height of the Texture in pixels.
	 * @param format PixelFormat of the Texture.
	 * @param buffer Pixelbuffer. Size must be (width*height*Texture::getBytesPerPixel(format)). Instance takes ownership.
	 *
	 */
	TexturePtr createTextureAndReset(const prtx::URIPtr& uri, uint32_t width, uint32_t height, const Texture::PixelFormat& format, uint8_t* buffer);

	virtual void setBool			(const std::wstring& key, Bool						 val);
	virtual void setInt				(const std::wstring& key, int32_t					 val);
	virtual void setFloat			(const std::wstring& key, double					 val);
	virtual void setString			(const std::wstring& key, const std::wstring&		 val);

	virtual void setBoolArray		(const std::wstring& key, const std::vector<Bool>&	 val);
	virtual void setIntArray		(const std::wstring& key, const std::vector<int>&	 val);
	virtual void setFloatArray		(const std::wstring& key, const std::vector<double>& val);
	virtual void setStringArray		(const std::wstring& key, const WStringVector&		 val);

	virtual void setBlindData		(const std::wstring& key, void*						 val);

private:
	void* mPrivateData;
};


} /* namespace prtx */


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_ITEXTURE_H_ */
