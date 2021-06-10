#pragma once

#include "ResolveMapCache.h"
#include "utils.h"

#include "prt/ContentType.h"
#include "prt/LogLevel.h"

/**
 * Helper struct to manage PRT lifetime
 */
class PRTContext final {
public:
	static std::unique_ptr<PRTContext>& get();

	explicit PRTContext(prt::LogLevel minimalLogLevel = prt::LogLevel::LOG_DEBUG);
	~PRTContext();

	ResolveMap::ResolveMapCache::LookupResult getResolveMap(const std::filesystem::path& rpk);
	bool isAlive() const;

	pcu::ConsoleLogHandlerPtr mLogHandler;
	pcu::FileLogHandlerPtr mFileLogHandler;
	pcu::ObjectPtr mPRTHandle;
	pcu::CachePtr mPRTCache;
	ResolveMap::ResolveMapCacheUPtr mResolveMapCache;
};