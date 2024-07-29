/**
 * ArcGIS CityEngine for Rhino
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

#include "PRTContext.h"
#include "ResolveMapCache.h"

#include <chrono>
#include <filesystem>

namespace {

constexpr bool DBG_TIME = false;

const std::chrono::system_clock::time_point INVALID_TIMESTAMP;

ResolveMap::ResolveMapCache::KeyType createCacheKey(const std::filesystem::path& rpk) {
	return rpk.generic_wstring();
}

template <typename TP>
std::time_t to_time_t(TP tp) {
	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - TP::clock::now() +
	                                                                              std::chrono::system_clock::now());
	return std::chrono::system_clock::to_time_t(sctp);
}

std::chrono::system_clock::time_point getFileModificationTime(const std::filesystem::path& p) {
	bool fileExists(std::filesystem::exists(p));
	bool isRegularFIle(std::filesystem::is_regular_file(p));

	if (!p.empty() && fileExists && isRegularFIle) {
		const std::filesystem::file_time_type last_write = std::filesystem::last_write_time(p);
		return std::chrono::system_clock::from_time_t(to_time_t(last_write));
	}
	else
		return INVALID_TIMESTAMP;
}

} // namespace

namespace ResolveMap {

ResolveMapCache::LookupResult ResolveMapCache::get(const std::filesystem::path& rpk) {
	const auto cacheKey = createCacheKey(rpk);

	const auto timeStamp = getFileModificationTime(rpk);
	if constexpr (DBG_TIME)
		LOG_DBG << "rpk: current timestamp: "
		        << std::chrono::duration_cast<std::chrono::nanoseconds>(timeStamp.time_since_epoch()).count() << "ns";

	if (timeStamp == INVALID_TIMESTAMP)
		throw std::invalid_argument("Cannot get time stamp of " + rpk.generic_string() +
		                            ", make sure to specify an existing and valid rule package.");

	CacheStatus cs = CacheStatus::HIT;
	auto it = mCache.find(cacheKey);
	if (it != mCache.end()) {
		const ResolveMapCacheEntry& rmce = it->second;
		if constexpr (DBG_TIME)
			LOG_DBG << "rpk: cache timestamp: "
			        << std::chrono::duration_cast<std::chrono::nanoseconds>(rmce.mTimeStamp.time_since_epoch()).count()
			        << "ns";
		if (rmce.mTimeStamp != timeStamp) {
			mCache.erase(it);
			cs = CacheStatus::MISS;
		}
	}
	else
		cs = CacheStatus::MISS;

	if (cs == CacheStatus::MISS) {
		const std::string rpkURI = pcu::toFileURI(rpk);
		const std::wstring wRpkURI = pcu::toUTF16FromUTF8(rpkURI);

		ResolveMapCacheEntry rmce;
		rmce.mTimeStamp = timeStamp;

		prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
		LOG_DBG << "createResolveMap from " << rpkURI;
		rmce.mResolveMap.reset(prt::createResolveMap(wRpkURI.c_str(), nullptr, &status), pcu::PRTDestroyer());
		if (status != prt::STATUS_OK)
			throw std::runtime_error("Failed to read rule package at " + rpk.generic_string() +
			                         ", please check the log file at " + PRTContext::getLogFilePath().generic_string());

		it = mCache.emplace(cacheKey, std::move(rmce)).first;
	}

	return {it->second.mResolveMap, cs};
}

} // namespace ResolveMap