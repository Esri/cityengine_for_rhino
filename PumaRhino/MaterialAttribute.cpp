/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "MaterialAttribute.h"
#include "Logger.h"

ON_Color Materials::extractColor(const wchar_t* key, const prt::AttributeMap* attrMap) {
	size_t count(0);
	const double* color = attrMap->getFloatArray(key, &count);

	if (count != 3) {
		LOG_ERR << L"Attribute " << key << ": array does not have size = 3.";
		return ON_Color::White;
	}

	ON_Color on_color;
	on_color.SetFractionalRGB(color[0], color[1], color[2]);

	return on_color;
}

Materials::MaterialAttribute Materials::extractMaterials(const size_t instanceIndex, const prt::AttributeMap* attrMap) {
	Materials::MaterialAttribute ma;
	ma.mMatId = instanceIndex;

	// process all keys
	size_t keyCount(0);
	const auto& keys = attrMap->getKeys(&keyCount);

	for (size_t i = 0; i < keyCount; ++i) {
		const auto& key = keys[i];
		const std::wstring strKey(key);

		const prt::AttributeMap::PrimitiveType type = attrMap->getType(key);

		switch (type) {
			case prt::AttributeMap::PT_STRING_ARRAY:
				// This is probably an array of textures.
				if (Materials::TEXTURE_KEYS.count(strKey) > 0) {
					size_t count = 0;
					const auto texArray = attrMap->getStringArray(key, &count);
					if (texArray != nullptr) {
						// If key is diffuseMap: first tex is the colormap and the second is the dirtmap. DirtMap are
						// not supported, thus they are ignored.
						if (strKey == L"diffuseMap") {
							const wchar_t* texPath = texArray[0];
							if (texPath != nullptr) {
								std::wstring texStr(texPath);
								if (texStr.length() > 0) {
									ma.mTexturePaths.insert_or_assign(strKey, texStr);
								}
							}
						}
						else {
							// This case should never happen.
							LOG_DBG << L"TEXTURE ARRAY that is not diffuseMap, key is: " << strKey;
						}
					}
				}
				break;
			case prt::AttributeMap::PT_STRING:
				// This is probably a texture path. check the key against the different allowed textures.
				if (Materials::TEXTURE_KEYS.count(strKey) > 0) {
					const wchar_t* texKey = attrMap->getString(key);
					if (texKey != nullptr) {
						std::wstring tex(texKey);
						if (tex.length() > 0)
							ma.mTexturePaths.insert_or_assign(strKey, tex);
					}
				}
				else {
					LOG_DBG << "Ignoring unsupported key " << key << ": " << attrMap->getString(key);
				}
				break;
			case prt::AttributeMap::PT_FLOAT:
				if (strKey == L"shininess") {
					ma.mShininess = attrMap->getFloat(key);
				}
				else if (strKey == L"opacity") {
					ma.mOpacity = attrMap->getFloat(key);
				}

				break;
			case prt::AttributeMap::PT_FLOAT_ARRAY:
				// Check for different type of colors
				if (strKey == L"diffuseColor") {
					ma.mDiffuseCol = extractColor(key, attrMap);
				}
				else if (strKey == L"ambientColor") {
					ma.mAmbientCol = extractColor(key, attrMap);
				}
				else if (strKey == L"specularColor") {
					ma.mSpecularCol = extractColor(key, attrMap);
				}

				break;
			default:
				LOG_DBG << "Ignoring unsupported key: " << key << " Primitive type: " << type;
		}
	}

	return ma;
}
