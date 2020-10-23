#pragma once

// Wrapper for PRT classes

#include "utils.h"
#include "ResolveMapCache.h"

#include "prt/LogLevel.h"
#include "prt/ContentType.h"

/**
* Helper struct to manage PRT lifetime
*/
struct PRTContext final {
	static std::unique_ptr<PRTContext>& get();

	explicit PRTContext(prt::LogLevel minimalLogLevel = prt::LogLevel::LOG_DEBUG);

	~PRTContext();

	ResolveMap::ResolveMapCache::LookupResult getResolveMap(const std::experimental::filesystem::path& rpk);

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