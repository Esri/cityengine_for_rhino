/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021-2024 Esri R&D Center Zurich
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

#include "AttrEvalCallbacks.h"
#include "Logger.h"

namespace {
constexpr bool DBG = false;
constexpr const wchar_t* CGA_ANNOTATION_HIDDEN = L"@Hidden";

bool isHiddenAttribute(const pcu::RuleFileInfoPtr& ruleFileInfo, const wchar_t* key) {
	for (size_t ai = 0, numAttrs = ruleFileInfo->getNumAttributes(); ai < numAttrs; ai++) {
		const auto attr = ruleFileInfo->getAttribute(ai);
		if (std::wcscmp(key, attr->getName()) == 0) {
			for (size_t k = 0, numAnns = attr->getNumAnnotations(); k < numAnns; k++) {
				if (std::wcscmp(attr->getAnnotation(k)->getName(), CGA_ANNOTATION_HIDDEN) == 0)
					return true;
			}
			return false;
		}
	}
	return false;
}
} // namespace

prt::Status AttrEvalCallbacks::generateError(size_t /*isIndex*/, prt::Status /*status*/, const wchar_t* /*message*/) {
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::assetError(size_t /*isIndex*/, prt::CGAErrorLevel /*level*/, const wchar_t* /*key*/,
                                          const wchar_t* /*uri*/, const wchar_t* /*message*/) {
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::cgaError(size_t /*isIndex*/, int32_t /*shapeID*/, prt::CGAErrorLevel /*level*/,
                                        int32_t /*methodId*/, int32_t /*pc*/, const wchar_t* message) {
	LOG_ERR << message;
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::cgaPrint(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*txt*/) {
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::cgaReportBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                             bool /*value*/) {
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::cgaReportFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                              double /*value*/) {
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::cgaReportString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* /*key*/,
                                               const wchar_t* /*value*/) {
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::attrBool(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, bool value) {
	if (DBG)
		LOG_DBG << "attrBool: isIndex = " << isIndex << ", key = " << key << " = " << value;
	if (mRuleFileInfo && !isHiddenAttribute(mRuleFileInfo, key))
		mAMBS[isIndex]->setBool(key, value);
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::attrFloat(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, double value) {
	if (DBG)
		LOG_DBG << "attrFloat: isIndex = " << isIndex << ", key = " << key << " = " << value;
	if (mRuleFileInfo && !isHiddenAttribute(mRuleFileInfo, key))
		mAMBS[isIndex]->setFloat(key, value);
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::attrString(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
                                          const wchar_t* value) {
	if (DBG)
		LOG_DBG << "attrString: isIndex = " << isIndex << ", key = " << key << " = " << value;
	if (mRuleFileInfo && !isHiddenAttribute(mRuleFileInfo, key))
		mAMBS[isIndex]->setString(key, value);
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::attrBoolArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key, const bool* ptr,
                                             size_t size, size_t /*nRows*/) {
	if (DBG)
		LOG_DBG << "attrBoolArray: isIndex = " << isIndex << ", key = " << key << " = " << *ptr << " size = " << size;
	if (mRuleFileInfo && !isHiddenAttribute(mRuleFileInfo, key))
		mAMBS[isIndex]->setBoolArray(key, ptr, size);
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::attrFloatArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
                                              const double* ptr, size_t size, size_t /*nRows*/) {
	if (DBG)
		LOG_DBG << "attrFloatArray: isIndex = " << isIndex << ", key = " << key << " = " << *ptr << " size = " << size;
	if (mRuleFileInfo && !isHiddenAttribute(mRuleFileInfo, key))
		mAMBS[isIndex]->setFloatArray(key, ptr, size);
	return prt::STATUS_OK;
}

prt::Status AttrEvalCallbacks::attrStringArray(size_t isIndex, int32_t /*shapeID*/, const wchar_t* key,
                                               const wchar_t* const* ptr, size_t size, size_t /*nRows*/) {
	if (DBG)
		LOG_DBG << "attrStringArray: isIndex = " << isIndex << ", key = " << key << " = " << *ptr << " size = " << size;
	if (mRuleFileInfo && !isHiddenAttribute(mRuleFileInfo, key))
		mAMBS[isIndex]->setStringArray(key, ptr, size);
	return prt::STATUS_OK;
}
