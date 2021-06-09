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