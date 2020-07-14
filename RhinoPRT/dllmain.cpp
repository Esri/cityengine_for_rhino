// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

//#include "RhinoPRT.h"

bool isInitialized = false;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		isInitialized = true;
		break;
    case DLL_THREAD_ATTACH:
		isInitialized = true;
		break;
    case DLL_THREAD_DETACH:
		isInitialized = false;
		break;
    case DLL_PROCESS_DETACH:
		isInitialized = false;
        break;
    }
    return TRUE;
}

