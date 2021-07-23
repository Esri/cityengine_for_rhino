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

#include <cassert>
#include <fstream>
#include <ostream>

namespace {

const std::filesystem::path NONE;

} // namespace

AssetCache::AssetCache(const std::filesystem::path& cacheRootPath) : mCacheRootPath(cacheRootPath) {}

const std::filesystem::path& AssetCache::put(const wchar_t* name, const uint8_t* buffer, size_t size) {
	assert(name != nullptr);

	std::lock_guard<std::mutex> lock(mMutex);

	const std::filesystem::path& assetPath = get(name);
	if (assetPath != NONE)
		return assetPath;

	std::filesystem::path newAssetPath = mCacheRootPath / name;

	std::ofstream stream(newAssetPath, std::ofstream::binary | std::ofstream::trunc);
	if (!stream) {
		return NONE;
	}
	stream.write(reinterpret_cast<const char*>(buffer), size);
	if (!stream) {
		return NONE;
	}
	stream.close();

	const auto it = mCache.emplace(name, newAssetPath);

	return it.first->second;
}

const std::filesystem::path& AssetCache::get(const wchar_t* name) const {
	const auto it = std::find_if(mCache.begin(), mCache.end(),
	                             [&name](const auto& p) { return (std::wcscmp(p.first.c_str(), name) == 0); });
	if (it != mCache.end())
		return it->second;
	else
		return NONE;
}
