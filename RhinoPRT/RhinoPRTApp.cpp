// RhinoPRT.cpp : Defines the initialization routines for the DLL.
//

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning (disable : 26451)
#	pragma warning (disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "RhinoPRTApp.h"

//
//	Note!
//
//    A Rhino plug-in is an MFC DLL.
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CRhinoPRTApp

BEGIN_MESSAGE_MAP(CRhinoPRTApp, CWinApp)
END_MESSAGE_MAP()

// The one and only CRhinoPRTApp object
CRhinoPRTApp theApp;

// CRhinoPRTApp initialization

BOOL CRhinoPRTApp::InitInstance()
{
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance initialization belongs here. 
  // All other significant initialization should take place in
  // CRhinoPRTPlugIn::OnLoadPlugIn().

	CWinApp::InitInstance();

	return TRUE;
}

int CRhinoPRTApp::ExitInstance()
{
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  // Only standard MFC DLL instance clean up belongs here. 
  // All other significant cleanup should take place in either
  // CRhinoPRTPlugIn::OnSaveAllSettings() or
  // CRhinoPRTPlugIn::OnUnloadPlugIn().
  return CWinApp::ExitInstance();
}
