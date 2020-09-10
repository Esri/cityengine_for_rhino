#include "MaterialAttribute.h"

#include "PRTUtilityModels.h"

#include "Logger.h"

ON_Color Materials::extractColor(const wchar_t* key, const prt::AttributeMap* attrMap)
{
	size_t count(0);
	const double* color = attrMap->getFloatArray(key, &count);

	if (count != 3) {
		LOG_ERR << L"Attribute " << key << ": array has not size 3.";
		return ON_Color::White;
	}

	ON_Color on_color;
	on_color.SetFractionalRGB(color[0], color[1], color[2]);

	return on_color;
}

void Materials::extractMaterials(const size_t initialShapeIndex, const size_t instanceIndex, const prt::AttributeMap* attrMap, MaterialsMap& matMap)
{
	MaterialAttribute ma;
	ma.mInitialShapeId = initialShapeIndex;
	ma.mMatId = instanceIndex;

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
			if (Materials::TEXTURE_KEYS.count(strKey) > 0)
			{
				auto tex = std::wstring(attrMap->getString(key));
				if(tex.length() > 0)
					ma.mTexturePaths.insert_or_assign(strKey, tex);
			}
			else {
				LOG_DBG << "Ignoring unsupported texture " << key << ": " << attrMap->getString(key);
			}
			break;
		case prt::AttributeMap::PT_FLOAT_ARRAY:
			// Check for different type of colors
			if (strKey == L"diffuseColor")
			{
				ma.mDiffuseCol = extractColor(key, attrMap);
			}
			else if (strKey == L"ambientColor")
			{
				ma.mAmbientCol = extractColor(key, attrMap);
			}
			else if (strKey == L"specularColor")
			{
				ma.mSpecularCol = extractColor(key, attrMap);
			}

			break;
		default:
			LOG_DBG << "Ignoring unsupported key: " << key;
		}
	}

	matMap.insert_or_assign(instanceIndex, ma);
}
