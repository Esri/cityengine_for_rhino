/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021-2023 Esri R&D Center Zurich
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

#pragma once

#include "utils.h"

#include "prt/AttributeMap.h"

#include "prtx/PRTUtils.h"

#include <map>
#include <set>
#include <string>

class GeneratedModel;

namespace Materials {

struct MaterialAttribute {
	size_t mMatId;

	std::map<std::wstring, std::wstring> mTexturePaths;

	ON_Color mDiffuseCol;
	ON_Color mAmbientCol;
	ON_Color mSpecularCol;

	double mShininess;
	double mOpacity;
};

const std::set<std::wstring> TEXTURE_KEYS = {
        L"colorMap",    // colormap
        L"bumpMap",     // bumpmap
        L"diffuseMap",  // dirtmap
        L"specularMap", // specularmap
        L"opacityMap",  // opacitymap
        L"normalMap"    // normalmap
};

using MaterialsMap = std::map<size_t, MaterialAttribute>;

ON_Color extractColor(const wchar_t* key, const prt::AttributeMap* attrMap);
MaterialAttribute extractMaterials(const size_t faceRangeId, const prt::AttributeMap* attrMap);

} // namespace Materials
