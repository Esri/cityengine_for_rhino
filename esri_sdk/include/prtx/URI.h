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

#ifndef PRTX_URI_H_
#define PRTX_URI_H_

#include "prtx/prtx.h"

#include <string>
#include <vector>
#include <memory>
#include <map>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4251 4231 4660)
#endif


namespace prtx {


class URI;
typedef std::shared_ptr<URI> URIPtr;		///< shared pointer implementation of prtx::URI
typedef std::vector<URIPtr>  URIPtrVector;	///< vector of shared pointers to prtx::URI


/**
 * URI class based on RFC 3986.
 * URI instances are immutable. See prtx::URIUtils namespace for factory member functions
 * and operations on URI.
 *
 * The observer member functions return UTF-16 strings *without* percent-encoding if not stated otherwise.
 * Additionally, all wchar_t/std::wstring arguments are encoded as UTF-16 if not stated otherwise.
 *
 * This class supports URI nesting inspired by the Java JAR syntax defined in:
 * https://www.iana.org/assignments/uri-schemes/prov/jar
 *
 * Definitions:
 * - composite URI: an URI which contains a nested URI
 * - nested URI: an URI which is embedded like this:
 *   \verbatim<scheme>:<nested URI>!/outer/path/file.ext\endverbatim
 */

class PRTX_EXPORTS_API URI {
public:
	static const std::wstring	SCHEME_FILE;			///< "file" scheme, e.g. file:/local/file/system/file.ext
	static const std::wstring	SCHEME_SAMBA;			///< samba "smb" scheme, e.g. smb://host/share syntax
	static const std::wstring	SCHEME_UNC;				///< UNC scheme, e.g. \\\host\share syntax
	static const std::wstring	SCHEME_MEMORY;			///< "memory" scheme, see prtx::URIUtils::createMemoryURI()
	static const std::wstring	SCHEME_RPK;				///< rule package "rpk" scheme, see prtx::URIUtils::createCompositeURI()
	static const std::wstring	SCHEME_ZIP;				///< "zip" scheme
	static const std::wstring	SCHEME_DATA;			///< "data" scheme, see http://tools.ietf.org/html/rfc2397
	static const std::wstring	SCHEME_BUILTIN;			///< "builtin" scheme

	static const std::wstring	QUERY_TEXTURE_WIDTH;
	static const std::wstring	QUERY_TEXTURE_HEIGHT;
	static const std::wstring	QUERY_TEXTURE_FORMAT;
	static const std::wstring	QUERY_TEXTURE_NAME;

	static const std::wstring	DELIM_SCHEME;			///< scheme delimiting character ':'
	static const std::wstring	DELIM_QUERY_ITEM;		///< query item delimiting character '&'
	static const std::wstring	DELIM_FRAGMENT_ITEM;	///< fragment item delimiting character '&'
	static const std::wstring	DELIM_ENTRY;			///< composite/nested URI delimiting character '!'

public:
	/**
	 * Factory member function to create an empty URI.
	 * Note: an empty URI will return false for isValid().
	 */
	static URIPtr create();


	/**
	 * Factory member function to create an absolute URI.
	 * @param fullURI fullURI is expected to be an percent-encoded UTF8 string.
	 */
	static URIPtr create(const std::wstring& fullURI);


	// -- STATE OBSERVERS

	/**
	 * Returns false, if parsing failed and/or RFC is violated by the input at construction time.
	 */
	virtual bool isValid() const = 0;

	/**
	 * Returns true, if this URI is pointing to a file i.e. "file:/path/to/ff.ext"
	 */
	virtual bool isFilePath() const = 0;


	// -- COMPOSITE SUPPORT

	/**
	 * Returns true, if this URI contains a valid nested URI. Else, returns false.
	 */
	virtual bool isComposite() const = 0;

	/**
	 * Returns the nested URI or URIPtr() if there is no nested URI.
	 * For example, for the URI "rpk:file:/path/to/package.rpk!/data/file.txt",
	 * it returns an URI object for "file:/path/to/package.rpk".
	 */
	virtual URIPtr getNestedURI() const = 0;


	// -- STRING OUTPUT

	/**
	 * Returns a reference to the full URI string in UTF-16.
	 * NOTE: only contains characters from the allowed set defined by RFC 3986,
	 * other characters are percent-encoded.
	 */
	virtual const std::wstring& wstring() const = 0;

	/**
	 * Returns a copy of the full URI string in UTF-8.
	 * NOTE: only contains characters from the allowed set defined by RFC 3986,
	 * other characters are percent-encoded.
	 */
	virtual std::string string() const = 0;

	/**
	 * Get a platform-dependent representation of the URI based on scheme,
	 * e.g. on windows, the uri "unc://host/share/path" will be returned as "\\host\share\path"
	 */
	virtual std::wstring getNativeFormat() const = 0;


	// --- COMPONENT OBSERVERS

	/**
	 * Returns a reference to the URI scheme in UTF-16.
	 * (e.g. "builtin", "file", "memory", "rpk", "zip", "http", ...)
	 */
	virtual const std::wstring& getScheme() const = 0;

	/**
	 * Returns a reference to the hostname in UTF-16 (potentially an empty string).
	 */
	virtual const std::wstring& getHost() const = 0;

	/**
	 * Returns a copy to the path component in UTF-16.
	 * Note: omits the leading slash on windows if path starts with a drive letter.
	 */
	virtual std::wstring getPath() const = 0;

	/**
	 * Returns a copy of the base name part of the path component in UTF-16.
	 * Note: Result is undefined for non-file schemes.
	 */
	virtual std::wstring getBaseName() const = 0;

	/**
	 * Returns the path extension, i.e. the remainder of the path
	 * component including the last '.'
	 */
	virtual const std::wstring& getExtension() const = 0;

	/**
	 * Returns true if the query part has non-zero length.
	 */
	virtual bool hasQuery() const = 0;

	/**
	 * Returns the whole query component or an empty string.
	 */
	virtual const std::wstring& getQuery() const = 0;

	/**
	 * Returns the query component specified by key or an empty string.
	 *
	 * @param key If key is not empty, look for key=val part in query component
	 * and return val, else return empty string.
	 * Note: key must not contain percent-encoded characters.
	 */
	virtual std::wstring getQuery(const std::wstring& key) const = 0;

	/**
	 * Returns true if the fragment part has non-zero length.
	 */
	virtual bool hasFragment() const = 0;

	/**
	 * Returns the fragment part (potentially an empty string).
	 */
	virtual const std::wstring& getFragment() const = 0;

	/**
	 * Returns the fragment component specified by key or an empty string.
	 *
	 * @param key If key is not empty, look for key=val part in fragment component
	 * and return val, else return empty string.
	 * Note: key must not contain percent-encoded characters.
	 */
	virtual std::wstring getFragment(const std::wstring &key) const = 0;

public:
	virtual ~URI() = default;

protected:
	URI() = default;
	URI(const URI&) = default;
};


/**
 * @namespace prtx::URIUtils
 * @brief A collection of utility functions which create or operate on prtx::URI instances.
 */
namespace URIUtils {


/**
 * Factory call to create an absolute file system URI, e.g. "file:/my/path/file.txt"
 * NOTE: for paths starting with windows drive letters (e.g. P:/tmp), a slash is prepended if necessary.
 * @param absolutePath absolutePath is expected to be an percent-encoded UTF8 string
 */
PRTX_EXPORTS_API URIPtr createFileURI(const std::wstring& percentEncodedAbsolutePath);


/**
 * Factory call to create a builtin uri, e.g. "builtin:default"
 * @param name name is expected to be an percent-encoded UTF8 string
 */
PRTX_EXPORTS_API URIPtr createBuiltinURI(const std::wstring& name);


/**
 * Factory call to create an URI from a memory block, e.g. "memory://ffa09/5.ext",
 * i.e. the address is stored in the <host> component, the byte count is stored in the <path> component.
 * will throw std::invalid_argument if the <host> and <path> component contain unexpected values.
 * @param ext is expected to be an percent-encoded UTF8 string
 */
PRTX_EXPORTS_API URIPtr createMemoryURI(const uint8_t* ptr, size_t size, const std::wstring& ext);


/**
 * Factory call to create a data URI according to http://tools.ietf.org/html/rfc2397.
 * The URI follows the syntax 'data:<mediatype>[;base64],<data>'
 * @param mediaType	The MIME type of the data.
 * @param base64	If true, the data will be base64 encoded. If false, the data is
 * 					expected to be a *percent-encoded, null-terminated ASCII string*.
 * @param dataSize	Number of bytes in data.
 * @returns The data URI.
 */
PRTX_EXPORTS_API URIPtr createDataURI(const std::string& mediaType, bool base64, const uint8_t* data, size_t dataSize);


/**
 * Creates a composite URI from a nested/inner URI and outer components in the following manner:
 * <outerScheme>:<nestedURI>!<outerPath>
 */
PRTX_EXPORTS_API URIPtr createCompositeURI(
		const URIPtr&		nestedURI,
		const std::wstring&	outerScheme,	///< must be percent encoded
		const std::wstring&	outerPath		///< must be percent encoded, must start with a forward slash
);


/**
 * Creates a composite URI from a nested/inner URI and outer components in the following manner:
 * <outerScheme>:<nestedURI>!<outerPath>?<outerQuery>#<outerFragment>
 */
PRTX_EXPORTS_API URIPtr createCompositeURI(
		const URIPtr&		nestedURI,
		const std::wstring&	outerScheme,	///< must be percent encoded
		const std::wstring&	outerPath,		///< must be percent encoded, must start with a forward slash
		const std::wstring&	outerQuery,		///< must be percent encoded
		const std::wstring&	outerFragment	///< must be percent encoded
);


/**
 * Returns a copy where the filename (= last segment of path) in the path component
 * has been replaced with newFileName (includes the extension).
 *
 * Note: newFileName must be UTF-16 encoded and not contain percent-encoded characters.
 */
PRTX_EXPORTS_API URIPtr replaceFilename(const URIPtr& uri, const std::wstring& newFileName);


/**
 * Returns a copy where the extension in the path component has been
 * replaced with newExtWithSep (includes the separator).
 *
 * Note: newFileName must be UTF-16 encoded and not contain percent-encoded characters.
 */
PRTX_EXPORTS_API URIPtr replaceExtension(const URIPtr& uri, const std::wstring& newExtWithSep);


/**
 * Appends to the query part in the form "[&]key=value" and returns a new URI instance.
 *
 * Note: Key and value must be UTF-16 encoded and not contain percent-encoded characters.
 */
PRTX_EXPORTS_API URIPtr addQuery(const URIPtr& uri, const std::wstring& key, const std::wstring& value);


/**
 * Appends to the query part in the form "[&]key=value" and returns a new URI instance.
 *
 * Note: The map entries are used as key and value and must be UTF-16 encoded and not contain percent-encoded characters.
 */
PRTX_EXPORTS_API URIPtr addQuery(const URIPtr& uri, const std::map<std::wstring, std::wstring>& queries);


/**
 * Appends a complete query part and returns a new URI instance.
 *
 * Note: The query string must be UTF-16 encoded and not contain percent-encoded characters.
 */
PRTX_EXPORTS_API URIPtr addQuery(const URIPtr& uri, const std::wstring& query);


/**
 * Returns a copy of the URI with the the whole query component removed. If uri does not contain
 * any query component, it is returned as-is.
 */
PRTX_EXPORTS_API URIPtr removeQuery(const URIPtr& uri);


/**
 * Appends to the fragment part in the form "[&]key=value" and returns a new URI instance.
 *
 * Note: Key and value must be UTF-16 encoded and not contain percent-encoded characters.
 */
PRTX_EXPORTS_API URIPtr addFragment(const URIPtr& uri, const std::wstring& key, const std::wstring& value);


/**
 * Appends to the fragment part in the form "[&]key=value" and returns a new URI instance.
 *
 * Note: The map entries are used as key and value and must be UTF-16 encoded and not contain percent-encoded characters.
 */
PRTX_EXPORTS_API URIPtr addFragment(const URIPtr& uri, const std::map<std::wstring, std::wstring>& fragments);


/**
 * Appends a complete fragment of the form "key=value[&key1=value1&...]" and returns a new URI instance.
 *
 * Note: The fragment string must be UTF-16 encoded and not contain percent-encoded characters.
 */
PRTX_EXPORTS_API URIPtr addFragment(const URIPtr& uri, const std::wstring& fragment);


/**
 * Returns a copy of the URI with the the whole fragment component removed. If uri does not contain
 * any fragment component, it is returned as-is.
 */
PRTX_EXPORTS_API URIPtr removeFragment(const URIPtr& uri);


/**
 * Tries to parse an URI according to the memory URI syntax (memory:/<address hex>/<byte count hex>)
 * into address and byte count.
 *
 * @throw std::invalid_argument If the URI cannot be parsed.
 */
PRTX_EXPORTS_API void parseMemoryURI(
		const URIPtr&	uri,		///< [in] The URI to be parsed as memory URI. Must be valid, else an exception is thrown.
		const uint8_t*&	address,	///< [out] Receives the address stored in the memory URI.
		size_t&			byteCount	///< [out] Receives the byte count specified by the memory URI.
);


/**
 * Utility function to percent encode a string according to RFC3986.
 * @param s An UTF-8 encoded string.
 */
PRTX_EXPORTS_API std::string percentEncode(const std::string& s);


/**
 * Utility function to percent encode an UTF-16 string according to RFC3986.
 * NOTE: s will be converted to UTF-8 first (as required by the RFC).
 */
PRTX_EXPORTS_API std::wstring percentEncode(const std::wstring& s);


/**
 * Utility function to percent decode a string according to RFC3986.
 * @return An UTF-8 encoded string.
 */
PRTX_EXPORTS_API std::string percentDecode(const std::string& percentEncodedString);


/**
 * Utility function to get a unique id.
 * Can be used to uniquify a memory URI.
 * Guaranteed to be unique over the PRT process lifetime.
 */
PRTX_EXPORTS_API uint64_t getUniqueId();


} // namespace URIUtils


} // namespace prtx


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_URI_H_ */
