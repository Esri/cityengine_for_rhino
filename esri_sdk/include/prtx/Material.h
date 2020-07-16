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

#ifndef PRTX_MATERIAL_H_
#define PRTX_MATERIAL_H_

#include "prtx/prtx.h"
#include "prtx/Types.h"
#include "prtx/Attributable.h"
#include "prtx/Builder.h"
#include "prtx/Content.h"
#include "prtx/Shader.h"
#include "prtx/Texture.h"
#include "prtx/generated/BuiltinMaterialAttributes.h"

#include <vector>
#include <memory>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4275)
#endif


class MaterialImpl;


namespace prtx {


class Material;
typedef std::shared_ptr<Material> MaterialPtr;			///< shared Material pointer
typedef std::vector<MaterialPtr>  MaterialPtrVector;		///< vector of shared Material pointers

/**
 * The read-only Material class stores values for the keys defined by @cgaref{attr_material.html,CGA material shape attributes} on the one hand and
 * by the attached Shader on the other.
 * The values can be accessed via the Attributable interface or via the BuiltinMaterialAttributes interface.
 * While the former is a generic standard interface, the later is a set of named functions and significantly faster
 * in most cases.
 * For example, getting the red color component can be done in a few ways:
 * @verbatim
    CGA code:                  material.color.r
    Attributable interface:    mat.getFloat("color.r")
    BuiltinMaterialAttributes: mat.color_r()
                               mat.diffuseColor()[0]
   @endverbatim
 *
 * The Attributable interface is extended with gettets for the Texture type.
 *
 *
 * Below is a list of all supported keys. Please note that the two key sets are two different views on the same values.
 * The CGA keys correspond 1:1 to the material attributes in CGA while the shader keys are designed for ease of use in
 * conjunction with he typical formats encountered in decoder/encoder programming.
 *
 *
 * Valid keys:
 *  * CGA, float type:
 *   * ambient.b
 *   * ambient.g
 *   * ambient.r
 *   * bumpValue
 *   * bumpmap.rw
 *   * bumpmap.su
 *   * bumpmap.sv
 *   * bumpmap.tu
 *   * bumpmap.tv
 *   * color.a
 *   * color.b
 *   * color.g
 *   * color.r
 *   * colormap.rw
 *   * colormap.su
 *   * colormap.sv
 *   * colormap.tu
 *   * colormap.tv
 *   * dirtmap.rw
 *   * dirtmap.su
 *   * dirtmap.sv
 *   * dirtmap.tu
 *   * dirtmap.tv
 *   * emissive.b
 *   * emissive.g
 *   * emissive.r
 *   * emissivemap.rw
 *   * emissivemap.su
 *   * emissivemap.sv
 *   * emissivemap.tu
 *   * emissivemap.tv
 *   * metallic
 *   * metallicmap.rw
 *   * metallicmap.su
 *   * metallicmap.sv
 *   * metallicmap.tu
 *   * metallicmap.tv
 *   * normalmap.rw
 *   * normalmap.su
 *   * normalmap.sv
 *   * normalmap.tu
 *   * normalmap.tv
 *   * occlusionmap.rw
 *   * occlusionmap.su
 *   * occlusionmap.sv
 *   * occlusionmap.tu
 *   * occlusionmap.tv
 *   * opacity
 *   * opacitymap.cutoff
 *   * opacitymap.rw
 *   * opacitymap.su
 *   * opacitymap.sv
 *   * opacitymap.tu
 *   * opacitymap.tv
 *   * reflectivity
 *   * roughness
 *   * roughnessmap.rw
 *   * roughnessmap.su
 *   * roughnessmap.sv
 *   * roughnessmap.tu
 *   * roughnessmap.tv
 *   * shininess
 *   * specular.b
 *   * specular.g
 *   * specular.r
 *   * specularmap.rw
 *   * specularmap.su
 *   * specularmap.sv
 *   * specularmap.tu
 *   * specularmap.tv
 *  * CGA, string type:
 *   * bumpmap
 *   * colormap
 *   * dirtmap
 *   * emissivemap
 *   * metallicmap
 *   * name
 *   * normalmap
 *   * occlusionmap
 *   * opacitymap
 *   * opacitymap.mode
 *   * roughnessmap
 *   * shader
 *   * specularmap
 *
 *  * Default shader, float type:
 *   * bumpValue         (identical to CGA key: bumpValue)
 *   * metallic          (identical to CGA key: metallic)
 *   * opacity           (identical to CGA key: opacity)
 *   * roughness         (identical to CGA key: roughness)
 *   * reflectivity      (identical to CGA key: reflectivity)
 *   * shininess         (identical to CGA key: shininess)
 *   * opacityMap.cutoff (identical to CGA key: opacitymap.cutoff)
 *  * Default shader, string type:
 *   * name             (identical to CGA key: name)
 *   * opacityMap.mode  (identical to CGA key: opacity.mode)
 *  * Default shader, float array type:
 *   * ambientColor      (identical to CGA keys: { ambient.r, ambient.g, ambient.b } )
 *   * bumpmapTrafo      (identical to CGA keys: { bumpmap.rw, bumpmap.su, bumpmap.sv, bumpmap.tu, bumpmap.tv } )
 *   * colormapTrafo     (identical to CGA keys: { colormap.rw, colormap.su, colormap.sv, colormap.tu, colormap.tv } )
 *   * diffuseColor      (identical to CGA keys: { color.r, color.g, color.b } )
 *   * dirtmapTrafo      (identical to CGA keys: { dirtmap.rw, dirtmap.su, dirtmap.sv, dirtmap.tu, dirtmap.tv } )
 *   * emissiveColor     (identical to CGA keys: { emissive.r, emissive.g, emissive.b } )
 *   * emissivemapTrafo  (identical to CGA keys: { emissivemap.rw, emissivemap.su, emissivemap.sv, emissivemap.tu, emissivemap.tv } )
 *   * metallicmapTrafo  (identical to CGA keys: { metallicmap.rw, metallicmap.su, metallicmap.sv, metallicmap.tu, metallicmap.tv } )
 *   * normalmapTrafo    (identical to CGA keys: { normalmap.rw, normalmap.su, normalmap.sv, normalmap.tu, normalmap.tv } )
 *   * occlusionmapTrafo (identical to CGA keys: { occlusionmap.rw, occlusionmap.su, occlusionmap.sv, occlusionmap.tu, occlusionmap.tv } )
 *   * opacitymapTrafo   (identical to CGA keys: { opacitymap.rw, opacitymap.su, opacitymap.sv, opacitymap.tu, opacitymap.tv } )
 *   * roughnessmapTrafo (identical to CGA keys: { roughnessmap.rw, roughnessmap.su, roughnessmap.sv, roughnessmap.tu, roughnessmap.tv } )
 *   * specularColor     (identical to CGA keys: { specular.r, specular.g, specular.b } )
 *   * specularmapTrafo  (identical to CGA keys: { specularmap.rw, specularmap.su, specularmap.sv, specularmap.tu, specularmap.tv } )
 *  * Default shader, Texture array type:
 *   * bumpMap          (identical to CGA key:  { resolved bumpmap } )
 *   * diffuseMap       (identical to CGA keys: { resolved colormap, resolved dirtmap } )
 *   * emissiveMap      (identical to CGA key:  { resolved emissivemap } )
 *   * metallicMap      (identical to CGA key:  { resolved metallicmap } )
 *   * normalMap        (identical to CGA key:  { resolved normalmap } )
 *   * occlusionMap     (identical to CGA key:  { resolved occlusionmap } )
 *   * opacityMap       (identical to CGA key:  { resolved opacitymap } )
 *   * roughnessMap     (identical to CGA key:  { resolved roughnessmap } )
 *   * specularMap      (identical to CGA key:  { resolved specularmap } )
 *
 * @sa @cgaref{attr_material.html,CGA material shape attributes}
 */
class PRTX_EXPORTS_API Material : public Attributable, public ComparableContent, public BuiltinMaterialAttributes {
public:
	/**
	 * Additional primitive types for Material
	 */
	static const PrimitiveType PT_TEXTURE		= PT_BLIND_DATA_ARRAY + 1;
	static const PrimitiveType PT_TEXTURE_ARRAY	= PT_TEXTURE + 1;
	static const PrimitiveType PT_COUNT			= prtx::Attributable::PT_COUNT + 2;

	virtual ~Material();

	/**@{
	 * Attributable Interface
	 */
	virtual const WStringVector&	getKeys()										const = 0;
	virtual bool					hasKey				(const std::wstring& key)	const = 0;
	virtual PrimitiveType			getType				(const std::wstring& key)	const = 0;
	virtual Bool					getBool				(const std::wstring& key)	const = 0;
	virtual int32_t					getInt				(const std::wstring& key)	const = 0;
	virtual double					getFloat			(const std::wstring& key)	const = 0;
	virtual const std::wstring&		getString			(const std::wstring& key)	const = 0;
	virtual const BoolVector&		getBoolArray		(const std::wstring& key)	const = 0;
	virtual const Int32Vector&		getIntArray			(const std::wstring& key)	const = 0;
	virtual const DoubleVector&		getFloatArray		(const std::wstring& key)	const = 0;
	virtual const WStringVector&	getStringArray		(const std::wstring& key)	const = 0;
	virtual void*					getBlindData		(const std::wstring& key)	const = 0;
	/**@}*/

	/**
	 * @returns The Shader associated with this Material.
	 */
	virtual ShaderPtr getShader() const = 0;

	/**
	 * Gets a Texture value.
	 *
	 * @param key Key to look for.
	 * @returns the value associated with a key or throws an exception if key not found.
	 */
	virtual TexturePtr getTexture(const std::wstring& key) const = 0;

	/**
	 * Gets an array of Textures. The returned reference stays valid until destruction of the Material.
	 *
	 * @param key Key to look for.
	 * @returns reference to a vector associated with a key or throws an exception if key not found.
	 */
	virtual const TexturePtrVector& getTextureArray(const std::wstring& key) const = 0;

	/**
	 * Gets the item at index in an array of Textures.
	 *
	 * @param [in] key Key to look for.
	 * @param [in] index The index of the desired texture in the array. Throws std::out_of_range if index is out of range.
	 * @returns reference to a vector associated with a key or throws an exception if key not found.
	 * @throw std::out_of_range
	 */
	virtual TexturePtr getTextureArrayItem(const std::wstring& key, size_t index) const = 0;

	/** Get all set blind data keys. The returned reference stays valid until destruction of the Attributable.
	 *
	 * @returns reference to vector with all blind data keys.
	 */
	virtual const WStringVector& getBlindDataKeys() const = 0;

protected:
	// @cond
	Material();
	// @endcond
};


class MaterialBuilder;
typedef std::shared_ptr<MaterialBuilder> MaterialBuilderPtr;

/**
 * A class to build Material instances. A MaterialBuilder is initialized with a Shader which defines the allowed key set.
 * Analogous to the Material, there is a set of keys defined by the default Shader which can always be set (see Material for the full list)
 * via the AttributableBuilder interface, and there are direct setters in the BuiltinMaterialAttributesBuilder interface.
 *
 * The AttributableBuilder interface is extended to the Texture type.
 *
 */
class PRTX_EXPORTS_API MaterialBuilder : public SharedPtrBuilder<Material>, public AttributableSetter, public BuiltinMaterialAttributesBuilder {
public:
	/**
	 * Constructs a MaterialBuilder with the default Shader.
	 */
	MaterialBuilder();

	/**
	 * Constructs a MaterialBuilder.
	 * @param shader The Shader for the Material.
	 */
	MaterialBuilder(const ShaderPtr& shader);

	/**
	 * Constructs a MaterialBuilder from an existing Material. 
	 * @param material The existing Material.
	 */
	MaterialBuilder(const MaterialPtr& material);

	virtual ~MaterialBuilder();

	// SharedPtrBuilder interface implementation
	virtual MaterialPtr createSharedAndReset(std::wstring* warnings = 0);
	virtual MaterialPtr createShared(std::wstring* warnings = 0) const;


	/**@{
	 * AttributableSetter Interface
	 */
	virtual void setBool		(const std::wstring& key, Bool					val);
	virtual void setInt			(const std::wstring& key, int32_t				val);
	virtual void setFloat		(const std::wstring& key, double				val);
	virtual void setString		(const std::wstring& key, const std::wstring&	val);
	virtual void setBoolArray	(const std::wstring& key, const BoolVector&		val);
	virtual void setIntArray	(const std::wstring& key, const Int32Vector&	val);
	virtual void setFloatArray	(const std::wstring& key, const DoubleVector&	val);
	virtual void setStringArray	(const std::wstring& key, const WStringVector&	val);
	virtual void setBlindData	(const std::wstring& key, void*					val);
	/**@}*/

	/**
	 * Sets a Texture.
	 * @param key           The key of the Texture.
	 * @param val           The Texture to set.
	 * @param resolveMapKey The ResolveMap key for the texture.
	 */
	virtual void setTexture(const std::wstring& key, const TexturePtr& val, const std::wstring& resolveMapKey);

	/**
	 * Sets a Texture array.
	 * @param key The key of the Texture array.
	 * @param val The Texture to set.
	 * @param resolveMapKey The ResolveMap key for the texture.
	 */
	virtual void setTextureArray(const std::wstring& key, const TexturePtrVector& val, const WStringVector& resolveMapKeys);

	/**
	 * Sets a Texture array.
	 * @param key The key of the Texture array.
	 * @param val The Texture to set.
	 * @param resolveMapKey The ResolveMap key for the texture.
	 */
	virtual void setTextureArrayItem(const std::wstring& key, size_t index, const TexturePtr& val, const std::wstring& resolveMapKey);

private:
	#include "prtx/generated/BuiltinMaterialAttributesBuilder_impl_decl.h"

private:
	MaterialImpl* mMaterial;
};


} // namespace prtx


#ifdef _MSC_VER
#	pragma warning(pop)
#endif


#endif /* PRTX_MATERIAL_H_ */
