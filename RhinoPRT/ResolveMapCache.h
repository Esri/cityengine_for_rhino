#pragma once

#include "utils.h"
#include "Logger.h"

#include <map>
#include <filesystem>

namespace ResolveMap {

	class ResolveMapCache {
	public:
		using KeyType = std::wstring;

		explicit ResolveMapCache(const std::filesystem::path& unpackPath) : mUnpackPath{ unpackPath } {}
		ResolveMapCache(const ResolveMapCache&) = delete;
		ResolveMapCache(ResolveMapCache&&) = delete;
		ResolveMapCache& operator=(ResolveMapCache const&) = delete;
		ResolveMapCache& operator=(ResolveMapCache&&) = delete;
		~ResolveMapCache();

		enum class CacheStatus { HIT, MISS, FAILURE };
		using LookupResult = std::pair<pcu::ResolveMapSPtr, CacheStatus>;
		LookupResult get(const std::filesystem::path& rpk);

	private:
		struct ResolveMapCacheEntry {
			pcu::ResolveMapSPtr mResolveMap;
			std::chrono::system_clock::time_point mTimeStamp;
			std::filesystem::path mExtractionPath;
		};
		using Cache = std::map<KeyType, ResolveMapCacheEntry>;
		Cache mCache;

		std::filesystem::path mUnpackPath;
	};

	using ResolveMapCacheUPtr = std::unique_ptr<ResolveMapCache>;

	const static pcu::ResolveMapSPtr RESOLVE_MAP_NONE;
	const static ResolveMapCache::LookupResult LOOKUP_FAILURE = { RESOLVE_MAP_NONE, ResolveMapCache::CacheStatus::FAILURE };

} // namespace ResolveMap