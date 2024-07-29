/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
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

#pragma once

#include "prt/API.h"

#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

namespace logging {

// log through the prt logger
template <prt::LogLevel L>
struct PRTLogger {
	PRTLogger() {}
	virtual ~PRTLogger() {
		prt::log(wstr.str().c_str(), L);
	}
	PRTLogger<L>& operator<<(std::wostream& (*x)(std::wostream&)) {
		wstr << x;
		return *this;
	}
	PRTLogger<L>& operator<<(const std::string& x) {
		std::copy(x.begin(), x.end(), std::ostream_iterator<char, wchar_t>(wstr));
		return *this;
	}
	template <typename T>
	PRTLogger<L>& operator<<(const T& x) {
		wstr << x;
		return *this;
	}
	std::wostringstream wstr;
};

// choose your logger (PRTLogger or StreamLogger)
template <prt::LogLevel L>
using LT = PRTLogger<L>;

using _LOG_DBG = LT<prt::LOG_DEBUG>;
using _LOG_INF = LT<prt::LOG_INFO>;
using _LOG_WRN = LT<prt::LOG_WARNING>;
using _LOG_ERR = LT<prt::LOG_ERROR>;

} // namespace logging

#define LOG_DBG logging::_LOG_DBG()
#define LOG_INF logging::_LOG_INF()
#define LOG_WRN logging::_LOG_WRN()
#define LOG_ERR logging::_LOG_ERR()