#include "ResolveMapCache.h"

#include <filesystem>
#include <chrono>

ResolveMapCache::~ResolveMapCache()
{
	std::error_code error;
	if (std::experimental::filesystem::remove_all(mUnpackPath, error) == -1) {
		LOG_ERR << L"Error while removing the temp directory: " << error.message();
	}
	else {
		LOG_INF << "Removed RPK unpack directory";
	}
}

ResolveMapCache::LookupResult ResolveMapCache::get(const std::wstring& rpk)
{
	const auto cacheKey = createCacheKey(rpk);

	const auto timeStamp = getFileModificationTime(rpk);
	LOG_DBG << "rpk: current timestamp: " << std::chrono::duration_cast<std::chrono::nanoseconds>(timeStamp.time_since_epoch()).count() << "ns";

	// verify timestamp
	if (timeStamp == INVALID_TIMESTAMP)
		return LOOKUP_FAILURE;

	CacheStatus cs = CacheStatus::HIT;
	auto it = mCache.find(cacheKey);
	if (it != mCache.end())
	{
		LOG_DBG << "rpk: cache timestamp: " << std::chrono::duration_cast<std::chrono::nanoseconds>(it->second.mTimeStamp.time_since_epoch()).count() << "ns";
		if (it->second.mTimeStamp != timeStamp)
		{
			mCache.erase(it);
			// clear the unpack path of the rpk.
		}
	}

}