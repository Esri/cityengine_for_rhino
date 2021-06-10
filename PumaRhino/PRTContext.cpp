#include "PRTContext.h"

#include "Logger.h"

#include <filesystem>
#include <memory>

namespace {
const std::wstring PUMA_TEMP_DIR_NAME(L"puma");

// Returns a per-session unique temp dir for RPK extraction and log file
// Ensures separation of multiple Rhino sessions with Puma
const std::filesystem::path& getGlobalTempDir() {
	static const std::filesystem::path globalTempDir = std::filesystem::temp_directory_path() / PUMA_TEMP_DIR_NAME;
	return globalTempDir;
}

const std::filesystem::path& getLogFilePath() {
	static const std::filesystem::path logFilePath = getGlobalTempDir() / "puma.log";
	return logFilePath;
}
} // namespace

std::unique_ptr<PRTContext>& PRTContext::get() {
	static std::unique_ptr<PRTContext> prtCtx = std::make_unique<PRTContext>();
	return prtCtx;
}

PRTContext::PRTContext(prt::LogLevel minimalLogLevel)
    : mLogHandler(prt::ConsoleLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT)),
      mFileLogHandler(prt::FileLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT,
                                                  getLogFilePath().wstring().c_str())),
      mPRTCache{prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT)},
      mResolveMapCache{new ResolveMap::ResolveMapCache(getGlobalTempDir())} {
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
