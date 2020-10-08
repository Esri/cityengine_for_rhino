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

		double mShininess;
		double mOpacity;
	};

	const std::set<std::wstring> TEXTURE_KEYS = { 
		/*L"colormap",*/ L"colorMap", // colormap
		/*L"bumpmap",*/ L"bumpMap", // bumpmap
		/*L"diffusemap",*/ L"diffuseMap", // dirtmap
		/*L"specularmap",*/ L"specularMap", // specularmap
		/*L"opacitymap",*/ L"opacityMap", // opacitymap
		/*L"normalmap",*/ L"normalMap" // normalmap
	};

	using MaterialsMap = std::map<size_t, MaterialAttribute>;

	ON_Color extractColor(const wchar_t* key, const prt::AttributeMap* attrMap);
	MaterialAttribute extractMaterials(const size_t initialShapeIndex, const size_t faceRangeId, const prt::AttributeMap* attrMap);

} // namespace Materials

