/*
  COPYRIGHT (c) 2012-2020 Esri R&D Center Zurich
  TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
  Unpublished material - all rights reserved under the
  Copyright Laws of the United States and applicable international
  laws, treaties, and conventions.

  For additional information, contact:
  Environmental Systems Research Institute, Inc.
  Attn: Contracts and Legal Services Department
  380 New York Street
  Redlands, California, 92373
  USA

  email: contracts@esri.com
*/

#ifndef PRT_LOGHANDLER_H_
#define PRT_LOGHANDLER_H_

#include "prt/prt.h"
#include "prt/Object.h"
#include "prt/LogLevel.h"
#include "prt/Status.h"


namespace prt {


/**
 * Interface for log event handling. Implement this if you want to handle log events yourself.
 *
 * \sa prtx::LogFormatter
 */

class PRT_EXPORTS_API LogHandler {
public:
	static const size_t ALL_COUNT = 6;
	static const prt::LogLevel ALL[ALL_COUNT];

	/**
	 * This function will be called for each log event which happens on the specified levels
	 */
	virtual void handleLogEvent(const wchar_t* msg, LogLevel level) = 0;

	/**
	 * Implement this function to specify the handled log levels
	 * @param[out] count Pointer to return number of returned log levels
	 * @returns Pointer to array with handled levels.
	 */
	virtual const LogLevel* getLevels(size_t* count) = 0;

	/**
	 * Implement this function to specify the format of the log message.
	 * @param[out] dateTime Pointer to return whether the date and time should be added to the message.
	 * @param[out] level    Pointer to return whether the level should be added to the message.
	 */
	virtual void getFormat(bool* dateTime, bool* level) = 0;

protected:
	LogHandler();
	virtual ~LogHandler();
};


/**
 * Convenience console logger, will print to stdout.
 *
 * \sa prtx::LogFormatter
 */

class PRT_EXPORTS_API ConsoleLogHandler : public LogHandler, public Object {
public:
	/**
	 * Creates a ConsoleLogHandler instance, must be deleted by the client.
	 *
	 * @param levels An array with log levels on which this handler should listen.
	 * @param count The number of log levelsin the levels array.
	 * @returns New instance of ConsoleLogHandler.
	 */
	static ConsoleLogHandler* create(const LogLevel* levels, size_t count);

protected:
	ConsoleLogHandler();
	virtual ~ConsoleLogHandler();
};


/**
 * Convenience file logger, will print to specified file.
 *
 * \sa prtx::LogFormatter
 */

class PRT_EXPORTS_API FileLogHandler : public LogHandler, public Object {
public:

	/**
	 * Creates a FileLogHandler instance, must be deleted by the client.
	 *
	 * @param levels An array with log levels on which this handler should listen.
	 * @param count The number of log levelsin the levels array.
	 * @param path The path accepts date/time format commands,
	 * see http://www.boost.org/doc/libs/1_59_0/doc/html/date_time/date_time_io.html. path must exist.
	 * @returns New instance of ConsoleLogHandler.
	 */
	static FileLogHandler* create(const LogLevel* levels, size_t count, const wchar_t* path);
	virtual const wchar_t* getPath() const = 0;

protected:
	FileLogHandler();
	virtual ~FileLogHandler();
};


} /* namespace prt */


#endif /* PRT_LOGHANDLER_H_ */
