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

#pragma once

#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>

class AssetCache {
public:
	explicit AssetCache(const std::filesystem::path& cacheRootPath);

	const std::filesystem::path& put(const wchar_t* name, const uint8_t* buffer, size_t size);

private:
	const std::filesystem::path& get(const wchar_t* name) const;

	std::unordered_map<std::wstring, std::filesystem::path> mCache;
	const std::filesystem::path& mCacheRootPath;
	std::mutex mMutex;
};
