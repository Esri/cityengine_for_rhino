#include "ResolveMapCache.h"

#include <filesystem>
#include <chrono>

namespace {

	const std::chrono::system_clock::time_point INVALID_TIMESTAMP;

	ResolveMap::ResolveMapCache::KeyType createCacheKey(const std::filesystem::path& rpk)
	{
		return rpk.wstring();
	}

	template <typename TP>
	std::time_t to_time_t(TP tp)
	{
		auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - TP::clock::now() + std::chrono::system_clock::now());
		return std::chrono::system_clock::to_time_t(sctp);
	}

	std::chrono::system_clock::time_point getFileModificationTime(const std::filesystem::path& p)
	{
		bool fileExists(std::filesystem::exists(p));
		bool isRegularFIle(std::filesystem::is_regular_file(p));

		if (!p.empty() && fileExists && isRegularFIle)
		{
			const std::filesystem::file_time_type last_write = std::filesystem::last_write_time(p);
			return std::chrono::system_clock::from_time_t(to_time_t(last_write));
		}
		else
			return INVALID_TIMESTAMP;
	}

	struct PathRemover {
		void operator()(std::filesystem::path const* p) {
			if (p && std::filesystem::exists(*p)) {
				std::filesystem::remove(*p);
				LOG_DBG << "Removed file " << *p;
				delete p;
			}
		}
	};
	using ScopedPath = std::unique_ptr<std::filesystem::path, PathRemover>;

} // namespace

namespace ResolveMap {

	ResolveMapCache::~ResolveMapCache()
	{
		if (std::filesystem::remove_all(mUnpackPath) == -1) {
			LOG_ERR << L"Error while removing the temp directory";
		}
		else {
			LOG_INF << "Removed RPK unpack directory";
		}
	}

	ResolveMapCache::LookupResult ResolveMapCache::get(const std::filesystem::path& rpk)
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
				const auto cnt = std::filesystem::remove_all(getUniqueSubdir(it->second));
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

	const std::filesystem::path ResolveMapCache::getUniqueSubdir(const ResolveMapCacheEntry& rmce)
	{
		std::filesystem::path path = mUnpackPath / rmce.mUUID;

		if (!std::filesystem::exists(path))
		{
			bool status = std::filesystem::create_directory(path);
			if (!status)
			{
				LOG_ERR << "Unable to create the extract directory: " << path << ". Fallback to default temp dir.";
				return mUnpackPath;
			}
		}

		return path;
	}

} // namepsace ResolveMap