/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
 *
 * Copyright (c) 2021-2025 Esri R&D Center Zurich
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

 // RhinoPRT.h : main header file for the RhinoPRT DLL.
//

#pragma once

#ifndef __AFXWIN_H__
#	error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h" // main symbols

// CRhinoPRTApp
// See RhinoPRTApp.cpp for the implementation of this class
//

class CRhinoPRTApp : public CWinApp {
public:
	// CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
	// Only standard MFC DLL instance construction belongs here.
	// All other significant initialization should take place in
	// CRhinoPRTPlugIn::OnLoadPlugIn().
	CRhinoPRTApp() = default;

	// CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
	// Only standard MFC DLL instance initialization belongs here.
	// All other significant initialization should take place in
	// CRhinoPRTPlugIn::OnLoadPlugIn().
	BOOL InitInstance() override;

	// CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
	// Only standard MFC DLL instance clean up belongs here.
	// All other significant cleanup should take place in either
	// CRhinoPRTPlugIn::OnSaveAllSettings() or
	// CRhinoPRTPlugIn::OnUnloadPlugIn().
	int ExitInstance() override;

	DECLARE_MESSAGE_MAP()
};
