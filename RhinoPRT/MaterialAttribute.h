#pragma once

#include "utils.h"

#include "prt/AttributeMap.h"

#include "prtx/PRTUtils.h"

#include <string>
#include <map>
#include <set>

class GeneratedModel;

namespace Materials {

	struct MaterialAttribute
	{
		size_t mInitialShapeId;
		size_t mMatId;

		std::map<std::wstring, std::wstring> mTexturePaths;

		ON_Color mDiffuseCol;
		ON_Color mAmbientCol;
		ON_Color mSpecularCol;
	};

	const std::set<std::wstring> TEXTURE_KEYS = { 
		L"colormap", // colormap
		L"bumpmap", // bumpmap
		L"diffusemap", // dirtmap
		L"specularmap", // specularmap
		L"opacitymap", // opacitymap
		L"normalmap" // normalmap
	};

	using MaterialsMap = std::map<size_t, MaterialAttribute>;

	ON_Color extractColor(const wchar_t* key, const prt::AttributeMap* attrMap);
	void extractMaterials(const size_t initialShapeIndex, const size_t faceRangeId, const prt::AttributeMap* attrMap, MaterialsMap& matMap);

} // namespace Materials

