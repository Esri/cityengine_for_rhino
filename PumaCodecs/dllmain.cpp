/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021 Esri R&D Center Zurich
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

#include "RhinoEncoder.h"
#include "pch.h"
#include "prtversion.h"

#include "prtx/ExtensionManager.h"

#include <iostream>

#define RHENC_EXPORTS_API __declspec(dllexport)

namespace {

const int VERSION_MAJOR = PRT_VERSION_MAJOR;
const int VERSION_MINOR = PRT_VERSION_MINOR;

} // namespace

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

extern "C" {

RHENC_EXPORTS_API void registerExtensionFactories(prtx::ExtensionManager* manager) {
	try {
		manager->addFactory(RhinoEncoderFactory::createInstance());
	}
	catch (std::exception& e) {
		std::cerr << __FUNCTION__ << " caught exception: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << __FUNCTION__ << " caught unknow exception: " << std::endl;
	}
}

RHENC_EXPORTS_API void unregisterExtensionFactories(prtx::ExtensionManager* /*manager*/) {}

RHENC_EXPORTS_API int getVersionMajor() {
	return VERSION_MAJOR;
}

RHENC_EXPORTS_API int getVersionMinor() {
	return VERSION_MINOR;
}
}
