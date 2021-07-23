#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "AssetCache.h"
#include "Logger.h"

#include <cassert>
#include <fstream>
#include <functional>
#include <ostream>
#include <string_view>

namespace {

bool writeCacheEntry(const std::filesystem::path& assetPath, const uint8_t* buffer, size_t size) noexcept {
	std::ofstream stream(assetPath, std::ofstream::binary | std::ofstream::trunc);
	if (!stream)
		return false;
	stream.write(reinterpret_cast<const char*>(buffer), size);
	if (!stream)
		return false;
	stream.close();
	return true;
}

void removeCacheEntry(const std::filesystem::path& expiredAssetPath) {
	if (std::error_code removeError; !std::filesystem::remove(expiredAssetPath, removeError))
		LOG_WRN << "Failed to delete expired asset cache entry " << expiredAssetPath << ": " << removeError.message();
}

} // namespace

AssetCache::AssetCache(const std::filesystem::path& cacheRootPath) : mCacheRootPath(cacheRootPath) {}

std::filesystem::path AssetCache::put(const wchar_t* name, const uint8_t* buffer, size_t size) {
	assert(name != nullptr);

	std::lock_guard<std::mutex> lock(mMutex);

	const std::string_view bufferView(reinterpret_cast<const char*>(buffer), size);
	const size_t hash = std::hash<std::string_view>{}(bufferView);

	const auto it = std::find_if(mCache.begin(), mCache.end(),
	                             [&name](const auto& p) { return (std::wcscmp(p.first.c_str(), name) == 0); });
	if ((it != mCache.end()) && (it->second.second == hash)) {
		const std::filesystem::path& assetPath = it->second.first;
		return assetPath;
	}

	const std::filesystem::path newAssetPath = getCachedPath(name, hash);

	if (!writeCacheEntry(newAssetPath, buffer, size)) {
		LOG_ERR << "Failed to put asset into cache, skipping asset: " << newAssetPath;
		return {};
	}

	if (it == mCache.end()) {
		mCache.emplace(name, std::make_pair(newAssetPath, hash));
	}
	else { // hash mismatch
		const std::filesystem::path expiredAsset = getCachedPath(name, it->second.second);
		removeCacheEntry(expiredAsset);
		it->second = std::make_pair(newAssetPath, hash);
	}

	return newAssetPath;
}

std::filesystem::path AssetCache::getCachedPath(const wchar_t* name, size_t hash) const {
	const std::filesystem::path path = mCacheRootPath / (std::to_wstring(hash) + L"_" + name);
	return path;
}
