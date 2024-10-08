/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
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
#include "Logger.h"

#include <filesystem>
#include <memory>

namespace {

constexpr prt::LogLevel PRT_MINIMAL_LOG_LEVEL = prt::LogLevel::LOG_WARNING;

const std::wstring PUMA_TEMP_DIR_NAME(L"puma");

} // namespace

std::unique_ptr<PRTContext>& PRTContext::get() {
	static std::unique_ptr<PRTContext> prtCtx = std::make_unique<PRTContext>(PRT_MINIMAL_LOG_LEVEL);
	return prtCtx;
}

const std::filesystem::path& PRTContext::getGlobalTempDir() {
	static const std::filesystem::path globalTempDir = std::filesystem::temp_directory_path() / PUMA_TEMP_DIR_NAME;
	return globalTempDir;
}

const std::filesystem::path& PRTContext::getLogFilePath() {
	static const std::filesystem::path logFilePath = getGlobalTempDir() / "puma.log";
	return logFilePath;
}

PRTContext::PRTContext(prt::LogLevel minimalLogLevel)
    : mLogHandler(prt::ConsoleLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT)),
      mFileLogHandler(prt::FileLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT,
                                                  getLogFilePath().wstring().c_str())),
      mPRTCache{prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT)},
      mResolveMapCache{new ResolveMap::ResolveMapCache()} {
	prt::addLogHandler(mLogHandler.get());
	prt::addLogHandler(mFileLogHandler.get());

	// create the list of extension path dynamicaly using getDllLocation
	const std::filesystem::path pumaPath = pcu::getDllLocation();
	const std::filesystem::path prtExtensionPath = pumaPath / "lib";
	std::vector<const wchar_t*> prtExtensionPaths = {prtExtensionPath.c_str()};

	prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
	mPRTHandle.reset(prt::init(prtExtensionPaths.data(), prtExtensionPaths.size(), minimalLogLevel, &status));

	if (status == prt::STATUS_ALREADY_INITIALIZED) {
		mPRTHandle.reset();
	}
	else if (!mPRTHandle || status != prt::STATUS_OK) {
		mPRTHandle.reset();
	}

	LOG_INF << "PRT has been initialized.";
}

PRTContext::~PRTContext() {
	mResolveMapCache.reset();
	LOG_INF << "Released RPK Cache";

	mPRTCache.reset();
	LOG_INF << "Released PRT Cache";

	// shutdown PRT
	mPRTHandle.reset();
	LOG_INF << "Shutdown PRT";

	prt::removeLogHandler(mFileLogHandler.get());
	prt::removeLogHandler(mLogHandler.get());
	mFileLogHandler.reset();
	mLogHandler.reset();
}

ResolveMap::ResolveMapCache::LookupResult PRTContext::getResolveMap(const std::filesystem::path& rpk) {
	auto lookupResult = mResolveMapCache->get(rpk);
	if (lookupResult.second == ResolveMap::ResolveMapCache::CacheStatus::MISS) {
		mPRTCache->flushAll();
	}

	return lookupResult;
}

bool PRTContext::isAlive() const {
	return !!mPRTHandle;
}

AssetCache& PRTContext::getAssetCache() const {
	static const std::filesystem::path assetCacheParentPath = [] {
		const auto p = PRTContext::getGlobalTempDir() / "asset_cache";
		try {
			std::filesystem::remove_all(p);
			std::filesystem::create_directories(p);
		}
		catch (std::exception& e) {
			LOG_ERR << "Error while cleaning the asset cache at " << p << ": " << e.what();
		}
		return p;
	}();
	static AssetCache theCache(assetCacheParentPath);
	return theCache;
}
