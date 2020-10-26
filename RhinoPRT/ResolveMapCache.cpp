#include "ResolveMapCache.h"

#include <filesystem>
#include <chrono>

namespace {

	const std::chrono::system_clock::time_point INVALID_TIMESTAMP;

	ResolveMap::ResolveMapCache::KeyType createCacheKey(const std::experimental::filesystem::path& rpk)
	{
		return rpk.wstring();
	}

	std::chrono::system_clock::time_point getFileModificationTime(const std::experimental::filesystem::path& p)
	{
		bool fileExists(std::experimental::filesystem::exists(p));
		bool isRegularFIle(std::experimental::filesystem::is_regular_file(p));

		if (!p.empty() && fileExists && isRegularFIle)
		{
			const auto last_write = std::experimental::filesystem::last_write_time(p);
			return std::chrono::system_clock::from_time_t(std::experimental::filesystem::file_time_type::clock::to_time_t(last_write));
		}
		else
			return INVALID_TIMESTAMP;
	}

	struct PathRemover {
		void operator()(std::experimental::filesystem::path const* p) {
			if (p && std::experimental::filesystem::exists(*p)) {
				std::experimental::filesystem::remove(*p);
				LOG_DBG << "Removed file " << *p;
				delete p;
			}
		}
	};
	using ScopedPath = std::unique_ptr<std::experimental::filesystem::path, PathRemover>;

} // namespace

namespace ResolveMap {

	ResolveMapCache::~ResolveMapCache()
	{
		if (std::experimental::filesystem::remove_all(mUnpackPath) == -1) {
			LOG_ERR << L"Error while removing the temp directory";
		}
		else {
			LOG_INF << "Removed RPK unpack directory";
		}
	}

	ResolveMapCache::LookupResult ResolveMapCache::get(const std::experimental::filesystem::path& rpk)
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
				const auto cnt = std::experimental::filesystem::remove_all(getUniqueSubdir(it->second));
				LOG_INF << "RPK change detected, forcing reload and clearing cache for " << rpk << " (removed " << cnt << " files)";
				cs = CacheStatus::MISS;
			}
		}
		else
			cs = CacheStatus::MISS;

		if (cs == CacheStatus::MISS)
		{
			const auto rpkURI = pcu::toFileURI(rpk);
			auto converted_str = pcu::toUTF16FromUTF8(rpkURI).c_str();

			ResolveMapCacheEntry rmce;
			rmce.mTimeStamp = timeStamp;
			rmce.mUUID = pcu::getUUID();

			prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
			LOG_DBG << "createResolveMap from " << rpkURI;
			rmce.mResolveMap.reset(prt::createResolveMap(converted_str, getUniqueSubdir(rmce).wstring().c_str(), &status), pcu::PRTDestroyer());
			if (status != prt::STATUS_OK)
				return LOOKUP_FAILURE;

			it = mCache.emplace(cacheKey, std::move(rmce)).first;
			LOG_INF << "Unpacked RPK " << rpk << " to " << mUnpackPath;
		}

		return { it->second.mResolveMap, cs };
	}

	const std::experimental::filesystem::path ResolveMapCache::getUniqueSubdir(const ResolveMapCacheEntry& rmce)
	{
		std::experimental::filesystem::path path = mUnpackPath / rmce.mUUID;

		if (!std::experimental::filesystem::exists(path))
		{
			bool status = std::experimental::filesystem::create_directory(path);
			if (!status)
			{
				LOG_ERR << "Unable to create the extract directory: " << path << ". Fallback to default temp dir.";
				return mUnpackPath;
			}
		}

		return path;
	}

} // namepsace ResolveMap