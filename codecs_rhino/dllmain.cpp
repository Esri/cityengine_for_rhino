// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "RhinoEncoder.h"

#include "prtx/ExtensionManager.h"

#include <iostream>

#define RHENC_EXPORTS_API __declspec(dllexport)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
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

	RHENC_EXPORTS_API void unregisterExtensionFactories(prtx::ExtensionManager* /*manager*/) { }

	RHENC_EXPORTS_API int getVersionMajor() {
		return 2;
	}

	RHENC_EXPORTS_API int getVersionMinor() {
		return 2;
	}

}
