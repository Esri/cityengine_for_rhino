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

#include "Logger.h"
#include "utils.h"

#include <filesystem>
#include <map>

namespace ResolveMap {

class ResolveMapCache {
public:
	using KeyType = std::wstring;

	ResolveMapCache() = default;
	ResolveMapCache(const ResolveMapCache&) = delete;
	ResolveMapCache(ResolveMapCache&&) = delete;
	ResolveMapCache& operator=(ResolveMapCache const&) = delete;
	ResolveMapCache& operator=(ResolveMapCache&&) = delete;
	~ResolveMapCache() = default;

	enum class CacheStatus { HIT, MISS };
	using LookupResult = std::pair<pcu::ResolveMapSPtr, CacheStatus>;
	LookupResult get(const std::filesystem::path& rpk);

private:
	struct ResolveMapCacheEntry {
		pcu::ResolveMapSPtr mResolveMap;
		std::chrono::system_clock::time_point mTimeStamp;
	};
	using Cache = std::map<KeyType, ResolveMapCacheEntry>;
	Cache mCache;
};

using ResolveMapCacheUPtr = std::unique_ptr<ResolveMapCache>;

const static pcu::ResolveMapSPtr RESOLVE_MAP_NONE;

} // namespace ResolveMap