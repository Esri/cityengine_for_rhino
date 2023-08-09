/**
 * Puma - CityEngine Plugin for Rhinoceros
 *
 * See https://esri.github.io/cityengine/puma for documentation.
 *
 * Copyright (c) 2021-2023 Esri R&D Center Zurich
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

#include "codecs_rhino.h"

#include "RhinoEncoder.h"

#include "prtx/ExtensionManager.h"

#include <iostream>

extern "C" {

	RHENC_EXPORTS_API void registerExtensionFactories(prtx::ExtensionManager* manager) {
		try {
			manager->addFactory(RhinoEncoderFactory::instance());
		}
		catch (std::exception& e) {
			std::cerr << __FUNCTION__ << " caught exception: " << e.what() << std::endl;
		}
		catch (...) {
			std::cerr << __FUNCTION__ << " caught unknow exception: " << std::endl;
		}
	}

	RHENC_EXPORTS_API void unregisterExtensionFactories(prtx::ExtensionManager* /*manager*/) { }

	RHENC_EXPORTS_API int getMinimalVersionMajor() {
		return 1;
	}

	RHENC_EXPORTS_API int getMinimalVersionMinor() {
		return 0;
	}

	RHENC_EXPORTS_API int getinimalVersionBuild() {
		return 0;
	}

}