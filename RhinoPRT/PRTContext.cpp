#include "PRTContext.h"

#include "Logger.h"

#include <memory>

PRTContext& PRTContext::get() {
	static PRTContext prtCtx;
	return prtCtx;
}

PRTContext::PRTContext(prt::LogLevel minimalLogLevel) {
	mLogHandler = prt::ConsoleLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT);
	mFileLogHandler = prt::FileLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT, L"C:/Windows/Temp/rhino_log.txt");
	prt::addLogHandler(mLogHandler);
	prt::addLogHandler(mFileLogHandler);

	// TODO: create the list of extension path dynamicaly using getDllLocation
	const wchar_t* prt_path[2] = { L"C:/Users/lor11212/Documents/Rhino/rhino-plugin-prototype/esri_sdk/lib", L"C:/Users/lor11212/Documents/Rhino/rhino-plugin-prototype/x64/Release/codecs_rhino.dll" };
	
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
	mLogHandler->destroy();
	mLogHandler = nullptr;
	mFileLogHandler->destroy();
	mFileLogHandler = nullptr;
}