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

	// verify timestamp
	if (timeStamp == INVALID_TIMESTAMP)
		return LOOKUP_FAILURE;

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

		const std::wstring unpackSubDirPrefix = [&rpk]() {
			std::wstring s = rpk.stem().generic_wstring(); // +L"_";
			// static const std::wstring TEMP_DIR_CHARS =
			// L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-"; pcu::replace_not_in_range(s,
			// TEMP_DIR_CHARS, L'_');
			return s;
		}();

		ResolveMapCacheEntry rmce;
		rmce.mTimeStamp = timeStamp;
		rmce.mExtractionPath.reset(new std::filesystem::path(mUnpackPath / unpackSubDirPrefix));

		prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
		LOG_DBG << "createResolveMap from " << rpkURI;
		rmce.mResolveMap.reset(prt::createResolveMap(wRpkURI.c_str(), rmce.mExtractionPath->wstring().c_str(), &status),
		                       pcu::PRTDestroyer());
		if (status != prt::STATUS_OK)
			return LOOKUP_FAILURE;

		it = mCache.emplace(cacheKey, std::move(rmce)).first;
		LOG_DBG << "Unpacked RPK " << rpk << " to " << mUnpackPath;
	}

	return {it->second.mResolveMap, cs};
}

} // namespace ResolveMap