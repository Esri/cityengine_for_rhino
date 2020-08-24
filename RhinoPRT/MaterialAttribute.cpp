#include "MaterialAttribute.h"

#include "Logger.h"

ON_Color Materials::extractColor(const wchar_t* key, const prt::AttributeMap* attrMap)
{
	size_t count(0);
	const double* diffuse = attrMap->getFloatArray(key, &count);

	if (count != 3) {
		LOG_ERR << L"Attribute diffuseColor array has not size 3.";
		return ON_Color::White;
	}

	ON_Color diffuseCol;
	diffuseCol.SetFractionalRGB(diffuse[0], diffuse[1], diffuse[2]);

	return diffuseCol;
}

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
		const std::wstring strKey = std::wstring(key);

		const prt::AttributeMap::PrimitiveType type = attrMap->getType(key);

		switch (type) {
		case prt::AttributeMap::PT_STRING:
			//This is probably a texture path. check the key against the different allowed textures.
			// for now only colormap
			if (strKey == L"colormap")
			{
				const std::wstring txPath = std::wstring(attrMap->getString(key));

				if (txPath.size() > 0)
				{

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
					ma.mColormapTexID = -1;
				}
			}
			else {
				LOG_DBG << "Ignoring unsupported texture " << key << ": " << attrMap->getString(key);
			}
			break;
		case prt::AttributeMap::PT_FLOAT_ARRAY:
			// Check for different type of colors
			if (strKey == L"diffuseColor")
			{
				ON_Color diffuseCol = extractColor(key, attrMap);
				ma.mRhinoMat.SetDiffuse(diffuseCol);
			}
			else if (strKey == L"ambientColor")
			{
				ON_Color ambientCol = extractColor(key, attrMap);
				ma.mRhinoMat.SetAmbient(ambientCol);
			}
			else if (strKey == L"specularColor")
			{
				ON_Color specularCol = extractColor(key, attrMap);
				ma.mRhinoMat.SetSpecular(specularCol);
			}

			break;
		default:
			LOG_DBG << "Ignoring unsupported key: " << key;
		}
	}

	matMap.insert_or_assign(faceRangeId, ma);
}
