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

#ifndef PRTX_NAMEPREPARATOR_H_
#define PRTX_NAMEPREPARATOR_H_

#include "prtx/prtx.h"

#include <string>
#include <memory>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4251 4275)
#endif


namespace prtx {


/**
 *	NamePreparator interface used to sanitize object names.
 */
class PRTX_EXPORTS_API NamePreparator {
public:
	/**
	 * Interface to manage the state of a name preparator namespace.
	 * Typically used to manage a string pool for creating unique names.
	 */
	struct NamespaceInfo {
		virtual ~NamespaceInfo() {};

		virtual void reset() = 0;
		virtual uint32_t getType() const = 0;
	};

	typedef std::shared_ptr<NamespaceInfo> NamespacePtr;

	/**
	 * These entities control the legalize and uniquify behavior. For example, a file name
	 * has very likely a different set of legal characters than a mesh name.
	 */
	enum Entity {
		ENTITY_MATERIAL,	///< Marks the name in preparation as a material name.
		ENTITY_MESH,		///< Marks the name in preparation as a mesh name.
		ENTITY_SHAPE,		///< Marks the name in preparation as a shape name.
		ENTITY_NODE,		///< Marks the name in preparation as a scene node name.
		ENTITY_TEXTURE,		///< Marks the name in preparation as a texture name.
		ENTITY_FILE,		///< Marks the name in preparation as a file name.
		ENTITY_CUSTOM		///< Marks the name in preparation as a custom name.
	};

public:
	virtual ~NamePreparator() {}

	/**
	 * Subclasses implement this to instantiate their own NamespaceInfo implementation.
	 */
	virtual NamespacePtr newNamespace() = 0;

	/**
	 *  Sanitizes a name with a technique specified by its entity.
	 *
	 *  @param [in,out] name A name in need of sanitizing.
	 *  @param [in] entity The entity of the name.
	 *
	 *  \sa Entity
	 */
	virtual void legalize(std::wstring& name, uint32_t entity) = 0;

	/**
	 * Convenience member function to get legalized *copy* of the name string.
	 */
	virtual std::wstring legalized(const std::wstring& name, uint32_t entity) {
		std::wstring s(name);
		legalize(s, entity);
		return s;
	}

	/**
	 * Makes a name unique according to its entity and within a namespace.
	 *
	 * @param [in,out] name The name to make unique.
	 * @param [in] The entity of the name.
	 * @param [in] The namespace in which to make the name unique.
	 */
	virtual void uniquify(std::wstring& name, uint32_t entity, const NamespacePtr& ns) = 0;

	/**
	 * Convenience member function to get uniquified *copy* of the name string.
	 */
	virtual std::wstring uniquified(const std::wstring& name, uint32_t entity, const NamespacePtr& ns) {
		std::wstring s(name);
		uniquify(s, entity, ns);
		return s;
	}

	/**
	 * Convenience member function to legalize and uniquify at the same time and in-place.
	 */
	void legalizeAndUniquify(std::wstring& name, uint32_t entity, const NamespacePtr& ns) {
		legalize(name, entity);
		uniquify(name, entity, ns);
	}

	/**
	 * Convenience member function to get a legalized and uniquified *copy* of the string.
	 */
	std::wstring legalizedAndUniquified(const std::wstring& name, uint32_t entity, const NamespacePtr& ns) {
		std::wstring s(name);
		legalizeAndUniquify(s, entity, ns);
		return s;
	}
};


/**
 * Default implementation of the NamePreparator interface. Basically replaces illegal characters with underscores '_'
 * and makes the names unique by appending a numerical suffix delimited with a customizable delimiter.
 * The entity NamePreparator::ENTITY_FILE is recognized and treated specially.
 */
class PRTX_EXPORTS_API DefaultNamePreparator : public prtx::NamePreparator {
public:
	/**
	 * Constructs a default name preparator with custom delimiters.
	 */
	DefaultNamePreparator(const std::wstring& delimiter = L"_", const std::wstring& filenameDelimiter = L"_");

	DefaultNamePreparator(const DefaultNamePreparator&) = delete;
	DefaultNamePreparator& operator=(const DefaultNamePreparator&) = delete;
	virtual ~DefaultNamePreparator() = default;

	NamespacePtr newNamespace() override;

	/**
	 * If entity equals NamePreparator::ENTITY_FILE the following character check is applied:
	 *
	 * A character c is illegal and will be replaced with an underscore if:
	 * \verbatim ord(c) < 32\endverbatim and or c is identical to any of these characters: \verbatim<>:\"/\\|?*#[]@!\endverbatim
	 *
	 * For other values of entity no modification takes place.
	 *
	 * @param [in,out] name The name to legalize, each character is checked and potentially replaced with an underscore.
	 * @param [in] entity The entity of the name.
	 */
	void legalize(std::wstring& name, uint32_t entity) override;

	/**
	 * If entity == NamePreparator::ENTITY_FILE the filename delimiter will be used to
	 * make the string unique by adding a unique suffix. If the name contains a file extension, the suffix will be
	 * inserted *before* the extension.
	 *
	 * If entity != NamePreparator::ENTITY_FILE the normal delimiter will be used and no scanning for file
	 * extensions takes place.
	 */
	void uniquify(std::wstring& name, uint32_t entity, const NamespacePtr& ns) override;

	/**
	 * Sets the delimiter for appending a unique suffix to the name.
	 * The default delimiter is an underscore '_'.
	 */
	void setDelimiter(const std::wstring& delimiter);

	/**
	 * Sets the filename delimiter for appending a unique suffix to the name.
	 * The default filename delimiter is an underscore '_'.
	 */
	void setFilenameDelimiter(const std::wstring& delimiter);

private:
	std::wstring mDelimiter;
	std::wstring mFilenameDelimiter;
};


/**
 * Specialization of the default implementation. It only allows valid ASCII characters for file entities.
 * It replaces illegal characters with underscores '_' and makes the names unique as the default implementation
 * The entity NamePreparator::ENTITY_FILE is recognized and treated specially.
 */
class PRTX_EXPORTS_API AsciiFileNamePreparator : public prtx::DefaultNamePreparator {
public:
	/**
	 * Constructs a ASCII name preparator with custom delimiters.
	 */
	AsciiFileNamePreparator(const std::wstring& delimiter = L"_", const std::wstring& filenameDelimiter = L"_");

	AsciiFileNamePreparator(const AsciiFileNamePreparator&) = delete;
	AsciiFileNamePreparator& operator=(const AsciiFileNamePreparator&) = delete;
	virtual ~AsciiFileNamePreparator() = default;

	/**
	 * If entity equals NamePreparator::ENTITY_FILE the following character check is applied:
	 *
	 * A character c is illegal and will be replaced with an underscore if
	 * it is not an ASCII character or the default name preparator recognizes it as illegal.
	 *
	 * For other values of entity no modification takes place.
	 *
	 * @param [in,out] name The name to legalize, each character is checked and potentially replaced with an underscore.
	 * @param [in] entity The entity of the name.
	 */
	void legalize(std::wstring& name, uint32_t entity) override;
};


} // namespace prtx


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_NAMEPREPARATOR_H_ */
