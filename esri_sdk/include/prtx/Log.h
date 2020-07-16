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

#pragma once

#include "prtx/prtx.h"
#include "prtx/Types.h"
#include "prtx/URI.h"
#include "prtx/OpaquePtr.h"

#include "prt/LogLevel.h"
#include "prt/LogHandler.h"
#include "prt/Status.h"

#include <string>
#include <ostream>
#include <exception>


#ifndef PRTX_LOG_ENABLED
#	define PRTX_LOG_ENABLED 1			// enable prtx logger by default
#endif

#ifndef PRTX_LOG_ENFORCE_TRACING
#	define PRTX_LOG_ENFORCE_TRACING 0	// set to 1 if you want tracing even in PRT_CC_OPT or PRT_CC_OPT_SYM
#endif

#if (PRTX_LOG_ENFORCE_TRACING == 1) && defined(PRT_BC_REL)
#	error "enforcing tracing in release build configuration is not allowed"
#endif


namespace prtx {


class BoundingBox;
class Reports;
typedef std::shared_ptr<Reports> ReportsPtr;

/**
 * LogFormatter is a log message formatter with support for both char and wchar_t arguments.
 * Internally, it uses boost::format syntax to format the log message with "%1%, %2%, ..." syntax.
 * See http://www.boost.org/doc/libs/1_66_0/libs/format/doc/format.html#examples for details.
 *
 * Each log message has an associated log level. The message will be logged
 * if its level is equal or greater than the globally set log level (see prt::init()).
 *
 * The logging is triggered when the LogFormatter instance is destroyed, i.e. is going out of scope.
 *
 * It is recommended to use LogFormatter through these convenience macros (char / wchar_t version):
 * - log_fatal
 * - log_error
 * - log_warn
 * - log_info
 * - log_debug
 *
 * Important: In order to output log messages to the console or into a log file,
 * a log handler must be added with prt::addLogHandler(). There are two default log handlers for your
 * convenience: prt::ConsoleLogHandler (uses std::wlog/wcout with UTF-16) and prt::FileLogHandler (UTF-8).
 *
 * A typical use case:
 * \code{.cpp}
 *  // at initialization time
 *  prt::LogHandler* lh = prt::ConsoleLogHandler::create(prt::LogHandler::ALL, prt::LogHandler::ALL_COUNT); // must be destroyed!
 *  prt::addLogHandler(lh);
 *
 *  int myInt = 6;
 *  std::string myString = "hello";
 *  log_info("my log message: %1%, %2%") % myInt % myString
 *
 *  // above code produces this log line:
 *  // [timestamp] [info] my log message: 6, hello
 * \endcode
 */
class PRTX_EXPORTS_API LogFormatter {
public:
	/**
	 * Generates a log message with the implicit formatting string "%1%".
	 * Useful to just log a single string, e.g. \verbatim log_info() % myString \endverbatim
	 */
	explicit LogFormatter(prt::LogLevel l);

	/**
	 * Generates a log message with the formatting string s. This is the most commonly used constructor.
	 */
	LogFormatter(prt::LogLevel l, const std::string& s);
	LogFormatter(prt::LogLevel l, const std::wstring& s);

	/**
	 * Generates a log message with the formatting string s and an optional prefix p.
	 * Useful for marking log messages from a submodule or class.
	 */
	LogFormatter(prt::LogLevel l, const std::string& s, const std::string& p);
	LogFormatter(prt::LogLevel l, const char* s, const char* p = 0);
	LogFormatter(prt::LogLevel l, const std::wstring& s, const std::wstring& p);
	LogFormatter(prt::LogLevel l, const std::wstring& s, const std::string& p);
	LogFormatter(prt::LogLevel l, const wchar_t* s, const wchar_t* p = 0);

	/**
	 * Convenience constructor to directly log an exception message.
	 */
	LogFormatter(prt::LogLevel l, const std::exception& e);

	/**
	 * Convenience constructor to log an exception message with a prefix string s.
	 */
	LogFormatter(prt::LogLevel l, const std::string& s, const std::exception& e);

	LogFormatter& operator%(float x);
	LogFormatter& operator%(double x);

#ifndef __linux__
	LogFormatter& operator%(long int x);
#ifndef _MSC_VER
	LogFormatter& operator%(std::size_t x);
#endif
#endif

	LogFormatter& operator%(uint8_t x);
	LogFormatter& operator%(uint16_t x);
	LogFormatter& operator%(uint32_t x);
	LogFormatter& operator%(uint64_t x);

	LogFormatter& operator%(int8_t x);
	LogFormatter& operator%(int16_t x);
	LogFormatter& operator%(int32_t x);
	LogFormatter& operator%(int64_t x);

	LogFormatter& operator%(const char* x);
	LogFormatter& operator%(const std::string& x);
	LogFormatter& operator%(const StringVector& x);

	LogFormatter& operator%(const wchar_t* x);
	LogFormatter& operator%(const std::wstring& x);
	LogFormatter& operator%(const WStringVector& x);

	LogFormatter& operator%(const prt::Status& x);
	LogFormatter& operator%(const prt::LogLevel& x);
	LogFormatter& operator%(const URIPtr& x);
	LogFormatter& operator%(const BoundingBox& x);
	LogFormatter& operator%(const DoubleVector& x);
	LogFormatter& operator%(const ReportsPtr& r);

	LogFormatter& log(uint32_t const* array, uint32_t count);

	LogFormatter() = delete;
	LogFormatter(const LogFormatter&) = delete;
	LogFormatter(LogFormatter&&) = delete;
	LogFormatter& operator=(const LogFormatter&) = delete;
	LogFormatter& operator=(LogFormatter&&) = delete;
	virtual ~LogFormatter();

private:
	BEGIN_SUPPRESS_EXPORT_WARNINGS
	class LogFormatterImpl;
	OpaquePtr<LogFormatterImpl> mLog;
	END_SUPPRESS_EXPORT_WARNINGS
};


/**
 * Logger forwarding template (logging disabled)
 */
template<bool E, prt::LogLevel L, typename LT>
class LogFwd final {
public:
	template<typename... ARGS>
	LogFwd(ARGS&&...) { }

	template<typename T>
	LogFwd& operator%(T&&) { return *this; }
};


/**
 * Logger forwarding template specialization (logging enabled).
 */
template<prt::LogLevel L, typename LT>
class LogFwd<true, L, LT> final : protected LT {
public:
	template<typename... ARGS>
	LogFwd(ARGS&&... args) : LT(L, args...) { }

	template<typename T>
	LogFwd& operator%(T&& t) { LT::operator%(t); return *this; }
};

#if (defined(PRT_CC_DBG) && (PRT_CC_DBG == 1))
constexpr bool isDBG = true;
#else
constexpr bool isDBG = false;
#endif

template<prt::LogLevel L>
constexpr bool IsLogEnabled = (PRTX_LOG_ENABLED == 1) && ((L != prt::LOG_TRACE) || ((L == prt::LOG_TRACE) && isDBG) || (PRTX_LOG_ENFORCE_TRACING == 1));

template<prt::LogLevel L>
using LogSwitch = LogFwd<IsLogEnabled<L>, L, prtx::LogFormatter>;


} // namespace prtx


using log_fatal = prtx::LogSwitch<prt::LOG_FATAL>;
using log_error = prtx::LogSwitch<prt::LOG_ERROR>;
using log_warn  = prtx::LogSwitch<prt::LOG_WARNING>;
using log_info  = prtx::LogSwitch<prt::LOG_INFO>;
using log_debug = prtx::LogSwitch<prt::LOG_DEBUG>;
using log_trace = prtx::LogSwitch<prt::LOG_TRACE>;
