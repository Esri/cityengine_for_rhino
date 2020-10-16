#pragma once

#include "utils.h"
#include "Logger.h"

#include <map>
#include <filesystem>

class ResolveMapCache {
public:
	using KeyType = std::wstring;

	explicit ResolveMapCache(const std::experimental::filesystem::path& unpackPath) : mUnpackPath{ unpackPath } {}
	ResolveMapCache(const ResolveMapCache&) = delete;
	ResolveMapCache(ResolveMapCache&&) = delete;
	ResolveMapCache& operator=(ResolveMapCache const&) = delete;
	ResolveMapCache& operator=(ResolveMapCache&&) = delete;
	~ResolveMapCache();

	enum class CacheStatus { HIT, MISS };
	using LookupResult = std::pair<pcu::ResolveMapSPtr, CacheStatus>;
	LookupResult get(const std::experimental::filesystem::path& rpk);

private:
	struct ResolveMapCacheEntry {
		pcu::ResolveMapSPtr mResolveMap;
		std::chrono::system_clock::time_point mTimeStamp;
		std::wstring mUUID; // The unique subdirectory in which the rpk is extracted
	};
	using Cache = std::map<KeyType, ResolveMapCacheEntry>;
	Cache mCache;

	std::experimental::filesystem::path mUnpackPath;

	const std::experimental::filesystem::path getUniqueSubdir(const ResolveMapCacheEntry& rmce);
};

using ResolveMapCacheUPtr = std::unique_ptr<ResolveMapCache>;