#pragma once

#include <map>

#include "utils.h"
#include "Logger.h"

class ResolveMapCache {
public:
	using KeyType = std::wstring;

	explicit ResolveMapCache(const std::wstring& unpackPath) : mUnpackPath{ unpackPath } {}
	ResolveMapCache(const ResolveMapCache&) = delete;
	ResolveMapCache(ResolveMapCache&&) = delete;
	ResolveMapCache& operator=(ResolveMapCache const&) = delete;
	ResolveMapCache& operator=(ResolveMapCache&&) = delete;
	~ResolveMapCache();

	enum class CacheStatus { HIT, MISS };
	using LookupResult = std::pair<pcu::ResolveMapSPtr, CacheStatus>;
	LookupResult get(const std::wstring& rpk);

private:
	struct ResolveMapCacheEntry {
		pcu::ResolveMapSPtr mResolveMap;
		std::chrono::system_clock::time_point mTimeStamp;
	};
	using Cache = std::map<KeyType, ResolveMapCacheEntry>;
	Cache mCache;

	const std::wstring mUnpackPath;
};

using ResolveMapCacheUPtr = std::unique_ptr<ResolveMapCache>;