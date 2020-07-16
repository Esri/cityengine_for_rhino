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

#ifndef PRTX_SHADER_H_
#define PRTX_SHADER_H_

#include "prtx/prtx.h"
#include "prtx/Types.h"
#include "prtx/Builder.h"
#include "prtx/Content.h"

#include <string>
#include <memory>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 4231 4251 4275 4660)
#endif


class CoreShader;


namespace prtx {

class Shader;
typedef std::shared_ptr<Shader> ShaderPtr;		///< shared Shader pointer

/**
 * A Shader defines the names and types of the attributes for a Material.
 * There is a default Shader called 'CityEngineShader' which can be extended using the ShaderBuilder class.
 *
 * Default shader keys:
 *  * float type:
 *   * bumpValue
 *   * opacity
 *   * reflectivity
 *   * shininess
 *  * string type:
 *   * name
 *  * float array type:
 *   * ambientColor
 *   * bumpmapTrafo
 *   * colormapTrafo
 *   * diffuseColor
 *   * dirtmapTrafo
 *   * normalmapTrafo
 *   * opacitymapTrafo
 *   * specularColor
 *   * specularmapTrafo
 *  * Texture array type:
 *   * bumpMap
 *   * diffuseMap
 *   * normalMap
 *   * opacityMap
 *   * specularMap
 *
 *
 * @sa Material, MaterialBuilder, ShaderBuilder
 */
class PRTX_EXPORTS_API Shader : public ComparableContent {
public:
	/**
	 * The types supported by the Shader and Material class.
	 */
	enum KeyType {
		KT_INT,		///< Integer type.
		KT_BOOL,	///< Boolean type.
		KT_FLOAT,	///< Float type.
		KT_STRING,	///< String type.
		KT_TEXTURE	///< Texture type.
	};

	virtual ~Shader();

	/**
	 * @returns the name of the shader.
	 */
	virtual const std::wstring& getName() const = 0;

	/**
	 * @return The index of the shader attribute key.
	 * \throws std::invalid_argument if key is not found
	 */
	virtual size_t getKey(const std::wstring& name) const = 0;

	/**
	 * @returns true if a key 'name' is present.
	 */
	virtual bool hasKey(const std::wstring& name) const = 0;

	/**
	 * @returns a vector with all defined keys.
	 */
	virtual WStringVector getKeys() const = 0;

	/**
	 * Convenience member function to get default shader
	 *
	 * @return The default shader.
	 */
	static ShaderPtr getDefault();

protected:
	Shader();
};


/**
 * The ShaderBuilder class can be used to extend the default Shader.
 */
class PRTX_EXPORTS_API ShaderBuilder : public SharedPtrBuilder<Shader> {
public:
	ShaderBuilder();
	virtual ~ShaderBuilder();

	void setName(const std::wstring& name);
	size_t addKey(const std::wstring& name, Shader::KeyType type, size_t size);

	// SharedPtrBuilder interface implementation
	virtual ShaderPtr createShared(std::wstring* warnings = 0) const;
	virtual ShaderPtr createSharedAndReset(std::wstring* warnings = 0);

private:
	CoreShader* mShader;
};


} /* namespace prtx */


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_SHADER_H_ */
