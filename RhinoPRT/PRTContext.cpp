#include "PRTContext.h"

#include "Logger.h"

#include <memory>

std::unique_ptr<PRTContext>& PRTContext::get() {
	static std::unique_ptr<PRTContext> prtCtx = std::make_unique<PRTContext>();
	return prtCtx;
}

PRTContext::PRTContext(prt::LogLevel minimalLogLevel)
	: mLogHandler(prt::ConsoleLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT)),
	  mFileLogHandler(prt::FileLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT, L"C:/Windows/Temp/rhino_log.txt")),
	  mPRTCache{prt::CacheObject::create(prt::CacheObject::CACHE_TYPE_DEFAULT)},
	  mResolveMapCache{new ResolveMap::ResolveMapCache(pcu::getTempDir(L"prt_temp"))}
{
	prt::addLogHandler(mLogHandler);
	prt::addLogHandler(mFileLogHandler);

	// create the list of extension path dynamicaly using getDllLocation
	std::wstring dll_location = pcu::getDllLocation();

	// build root dir
	auto id = dll_location.find(L"bin");
	if (id == std::wstring::npos) {
		LOG_ERR << L"Invalid build directory layout.";
		return;
	}
	std::wstring esri_sdk_dir = dll_location.substr(0, id).append(L"lib");
	const wchar_t* prt_path[1] = { esri_sdk_dir.c_str() };
	
	prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
	mPRTHandle.reset(prt::init(prt_path, 1, minimalLogLevel, &status));
	LOG_INF << prt::getStatusDescription(status);

	alreadyInitialized = true;

	if (status == prt::STATUS_ALREADY_INITIALIZED) {
		mPRTHandle.reset();
	}
	else if (!mPRTHandle || status != prt::STATUS_OK) {
		alreadyInitialized = false;
		mPRTHandle.reset();
	}
}

PRTContext::~PRTContext() {
	mResolveMapCache.reset();
	LOG_INF << "Released RPK Cache";

	mPRTCache.reset();
	LOG_INF << "Released PRT Cache";

	// shutdown PRT
	mPRTHandle.reset();
	LOG_INF << "Shutdown PRT";

	prt::removeLogHandler(mFileLogHandler);
	prt::removeLogHandler(mLogHandler);
	if (mLogHandler) {
		mLogHandler->destroy();
	}
	mLogHandler = nullptr;

	if (mFileLogHandler) {
		mFileLogHandler->destroy();
	}
	mFileLogHandler = nullptr;
}

ResolveMap::ResolveMapCache::LookupResult PRTContext::getResolveMap(const std::experimental::filesystem::path& rpk)
{
	auto lookupResult = mResolveMapCache->get(rpk);
	if (lookupResult.second == ResolveMap::ResolveMapCache::CacheStatus::MISS) {
		mPRTCache->flushAll();
	}

	return lookupResult;
}