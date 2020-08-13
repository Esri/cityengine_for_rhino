#include "PRTContext.h"

#include "Logger.h"

#include <memory>

std::unique_ptr<PRTContext>& PRTContext::get() {
	static std::unique_ptr<PRTContext> prtCtx = std::make_unique<PRTContext>();
	return prtCtx;
}

PRTContext::PRTContext(prt::LogLevel minimalLogLevel) {
	mLogHandler = prt::ConsoleLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT);
	mFileLogHandler = prt::FileLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT, L"C:/Windows/Temp/rhino_log.txt");
	prt::addLogHandler(mLogHandler);
	prt::addLogHandler(mFileLogHandler);

	// create the list of extension path dynamicaly using getDllLocation
	std::wstring dll_location = pcu::getDllLocation();
	size_t idx = dll_location.find(L"Release_gh");

	if (idx != std::wstring::npos) {
		dll_location = dll_location.substr(0, idx);
		dll_location = dll_location.append(L"Release\\");
	}
	std::wstring rhino_codecs_dir = dll_location.append(L"codecs_rhino.dll");

	// Get the esri_sdk location
	auto id = dll_location.find(L"x64");
	if (id == std::wstring::npos) {
		LOG_ERR << L"Invalid target platform, must be x64.";
		return;
	}
	std::wstring root_dir = dll_location.substr(0, id);
	std::wstring esri_sdk_dir = root_dir.append(L"esri_sdk\\lib");
	const wchar_t* prt_path[2] = { esri_sdk_dir.c_str(), rhino_codecs_dir.c_str() };
	
	prt::Status status = prt::STATUS_UNSPECIFIED_ERROR;
	mPRTHandle.reset(prt::init(prt_path, 2, minimalLogLevel, &status));

	if (!mPRTHandle || status != prt::STATUS_OK) {
		LOG_ERR << L"Could not initilize PRT";
		mPRTHandle.reset();
	}
}

PRTContext::~PRTContext() {
	// shutdown PRT
	mPRTHandle.reset();

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