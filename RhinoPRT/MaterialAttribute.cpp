#include "MaterialAttribute.h"

#include "Logger.h"

void Materials::extractMaterials(const size_t initialShapeIndex, const size_t faceRangeId, const prt::AttributeMap* attrMap, MaterialsMap & matMap)
{
	MaterialAttribute ma;
	ma.mInitialShapeId = initialShapeIndex;
	ma.mMatId = faceRangeId;

	// process all keys
	size_t keyCount(0);
	const auto& keys = attrMap->getKeys(&keyCount);

	for (size_t i = 0; i < keyCount; ++i) {
		const auto& key = keys[i];

		const prt::AttributeMap::PrimitiveType type = attrMap->getType(key);

		switch (type) {
		case prt::AttributeMap::PT_STRING:
			//This is probably a texture path. check the key against the different allowed textures.
			// for now only colormap
			if (std::wstring(key) == L"colormap")
			{
				const std::wstring txPath = std::wstring(attrMap->getString(key));
				LOG_DBG << "Extracting texture colormap: " << txPath;

				ON_Texture tex;
				tex.m_image_file_reference.SetRelativePath(txPath.c_str());
				tex.m_type = ON_Texture::TYPE::bitmap_texture;
				tex.m_bOn = true;
				tex.m_mode = ON_Texture::MODE::modulate_texture;

				int texID = ma.mRhinoMat.AddTexture(tex);
				ma.mColormapTexID = texID;
			}
			else {
				LOG_DBG << "Ignoring unsupported texture " << key << ": " << attrMap->getString(key);
			}
			break;
		default:
			LOG_DBG << "Ignoring unsupported key: " << key;
		}
	}

	matMap.insert_or_assign(faceRangeId, ma);
}
