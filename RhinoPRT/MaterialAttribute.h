#pragma once

#include "utils.h"

#include "prt/AttributeMap.h"

#include "prtx/PRTUtils.h"

#include <string>
#include <map>

namespace Materials {

	struct MaterialAttribute
	{
		size_t mInitialShapeId;
		size_t mMatId;

		ON_Material mRhinoMat;
		int mColormapTexID = -1;
	};

	using MaterialsMap = std::map<size_t, MaterialAttribute>;

	void extractMaterials(const size_t initialShapeIndex, const size_t faceRangeId, const prt::AttributeMap* attrMap, MaterialsMap& matMap);

} // namespace Materials

