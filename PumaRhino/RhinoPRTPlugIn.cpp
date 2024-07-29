/**
 * ArcGIS CityEngine for Rhino
 *
 * See https://esri.github.io/cityengine/rhino for documentation.
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

 // RhinoPRTPlugIn.cpp : defines the initialization routines for the plug-in.
//

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 26451)
#	pragma warning(disable : 26495)
#endif
#include "stdafx.h"
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

#include "Resource.h"
#include "RhinoPRTPlugIn.h"
#include "rhinoSdkPlugInDeclare.h"
#include "version.h"

// The plug-in object must be constructed before any plug-in classes derived
// from CRhinoCommand. The #pragma init_seg(lib) ensures that this happens.
#pragma warning(push)
#pragma warning(disable : 4073)
#pragma init_seg(lib)
#pragma warning(pop)

// Rhino plug-in declaration
RHINO_PLUG_IN_DECLARE

// Rhino plug-in name
// Provide a short, friendly name for this plug-in.
RHINO_PLUG_IN_NAME(L"Puma");

// Rhino plug-in id
// Provide a unique uuid for this plug-in.
RHINO_PLUG_IN_ID(L"1B8E4CAD-7A8F-4F05-B0A2-3BE0EC98C800");

// Rhino plug-in version
// Provide a version number string for this plug-in.
RHINO_PLUG_IN_VERSION(VER_PRODUCT_VERSION_STR);

// Rhino plug-in description
// Provide a description of this plug-in.
RHINO_PLUG_IN_DESCRIPTION(L"Puma enables the use of CityEngine CGA rules for the generation of procedural buildings "
                          L"within Rhino and Grasshopper.");

// Rhino plug-in icon resource id
// Provide an icon resource this plug-in.
// Icon resource should contain 16, 24, 32, 48, and 256-pixel image sizes.
RHINO_PLUG_IN_ICON_RESOURCE_ID(IDI_ICON);

// Rhino plug-in developer declarations
RHINO_PLUG_IN_DEVELOPER_ORGANIZATION(L"Esri Online LLC");
RHINO_PLUG_IN_DEVELOPER_ADDRESS(L"380 New York St, Redlands, CA 92373");
RHINO_PLUG_IN_DEVELOPER_COUNTRY(L"United States");
RHINO_PLUG_IN_DEVELOPER_PHONE(L"+1 909 793 2853");
RHINO_PLUG_IN_DEVELOPER_FAX(L"");
RHINO_PLUG_IN_DEVELOPER_EMAIL(L"cityengine-info@esri.com");
RHINO_PLUG_IN_DEVELOPER_WEBSITE(L"https://esri.github.io/cityengine/rhino");
RHINO_PLUG_IN_UPDATE_URL(L"https://github.com/esri/puma/releases");

// The one and only CRhinoPRTPlugIn object
static class CRhinoPRTPlugIn thePlugIn;

/////////////////////////////////////////////////////////////////////////////
// CRhinoPRTPlugIn definition

CRhinoPRTPlugIn& RhinoPRTPlugIn() {
	// Return a reference to the one and only CRhinoPRTPlugIn object
	return thePlugIn;
}

CRhinoPRTPlugIn::CRhinoPRTPlugIn() {
	// Description:
	//   CRhinoPRTPlugIn constructor. The constructor is called when the
	//   plug-in is loaded and "thePlugIn" is constructed. Once the plug-in
	//   is loaded, CRhinoPRTPlugIn::OnLoadPlugIn() is called. The
	//   constructor should be simple and solid. Do anything that might fail in
	//   CRhinoPRTPlugIn::OnLoadPlugIn().

	m_plugin_version = RhinoPlugInVersion();
}

/////////////////////////////////////////////////////////////////////////////
// Required overrides

const wchar_t* CRhinoPRTPlugIn::PlugInName() const {
	// Description:
	//   Plug-in name display string.  This name is displayed by Rhino when
	//   loading the plug-in, in the plug-in help menu, and in the Rhino
	//   interface for managing plug-ins.

	return RhinoPlugInName();
}

const wchar_t* CRhinoPRTPlugIn::PlugInVersion() const {
	// Description:
	//   Plug-in version display string. This name is displayed by Rhino
	//   when loading the plug-in and in the Rhino interface for managing
	//   plug-ins.

	return m_plugin_version;
}

GUID CRhinoPRTPlugIn::PlugInID() const {
	// Description:
	//   Plug-in unique identifier. The identifier is used by Rhino to
	//   manage the plug-ins.

	return ON_UuidFromString(RhinoPlugInId());
}

/////////////////////////////////////////////////////////////////////////////
// Additional overrides

BOOL CRhinoPRTPlugIn::OnLoadPlugIn() {
	// Description:
	//   Called after the plug-in is loaded and the constructor has been
	//   run. This is a good place to perform any significant initialization,
	//   license checking, and so on.  This function must return TRUE for
	//   the plug-in to continue to load.

	// Remarks:
	//    Plug-ins are not loaded until after Rhino is started and a default document
	//    is created.  Because the default document already exists
	//    CRhinoEventWatcher::On????Document() functions are not called for the default
	//    document.  If you need to do any document initialization/synchronization then
	//    override this function and do it here.  It is not necessary to call
	//    CPlugIn::OnLoadPlugIn() from your derived class.

	return RhinoPRT::get().InitializeRhinoPRT();
}

void CRhinoPRTPlugIn::OnUnloadPlugIn() {
	// Description:
	//    Called one time when plug-in is about to be unloaded. By this time,
	//    Rhino's mainframe window has been destroyed, and some of the SDK
	//    managers have been deleted. There is also no active document or active
	//    view at this time. Thus, you should only be manipulating your own objects.
	//    or tools here.

	RhinoPRT::get().ShutdownRhinoPRT();
}
