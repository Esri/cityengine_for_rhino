#pragma once

#include "ResolveMapCache.h"
#include "utils.h"

#include "prt/ContentType.h"
#include "prt/LogLevel.h"

/**
 * Helper struct to manage PRT lifetime
 */
struct PRTContext final {
	static std::unique_ptr<PRTContext>& get();

	explicit PRTContext(prt::LogLevel minimalLogLevel = prt::LogLevel::LOG_DEBUG);

	~PRTContext();

	ResolveMap::ResolveMapCache::LookupResult getResolveMap(const std::filesystem::path& rpk);

	bool isAlive() const {
		return alreadyInitialized;
	}

	prt::ConsoleLogHandler* mLogHandler;
	prt::FileLogHandler* mFileLogHandler;
	pcu::ObjectPtr mPRTHandle;
	pcu::CachePtr mPRTCache;
	ResolveMap::ResolveMapCacheUPtr mResolveMapCache;

private:
	bool alreadyInitialized;
};