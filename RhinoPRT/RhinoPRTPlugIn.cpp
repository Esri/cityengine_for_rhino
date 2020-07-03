// RhinoPRTPlugIn.cpp : defines the initialization routines for the plug-in.
//

#include "stdafx.h"
#include "rhinoSdkPlugInDeclare.h"
#include "RhinoPRTPlugIn.h"
#include "Resource.h"

#include "PRTPropertyPanel.h"

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
RHINO_PLUG_IN_NAME(L"RhinoPRT");

// Rhino plug-in id
// Provide a unique uuid for this plug-in.
RHINO_PLUG_IN_ID(L"1B8E4CAD-7A8F-4F05-B0A2-3BE0EC98C800");

// Rhino plug-in version
// Provide a version number string for this plug-in.
RHINO_PLUG_IN_VERSION(__DATE__ "  " __TIME__)

// Rhino plug-in description
// Provide a description of this plug-in.
RHINO_PLUG_IN_DESCRIPTION(L"RhinoPRT plug-in for Rhinoceros®");

// Rhino plug-in icon resource id
// Provide an icon resource this plug-in.
// Icon resource should contain 16, 24, 32, 48, and 256-pixel image sizes.
RHINO_PLUG_IN_ICON_RESOURCE_ID(IDI_ICON);

// Rhino plug-in developer declarations
// TODO: fill in the following developer declarations with
// your company information. Note, all of these declarations
// must be present or your plug-in will not load.
//
// When completed, delete the following #error directive.
RHINO_PLUG_IN_DEVELOPER_ORGANIZATION(L"Esri");
RHINO_PLUG_IN_DEVELOPER_ADDRESS(L"Förrlibuckstrasse 110\r\n 8005 Zürich");
RHINO_PLUG_IN_DEVELOPER_COUNTRY(L"Switzerland");
RHINO_PLUG_IN_DEVELOPER_PHONE(L"123.456.7890");
RHINO_PLUG_IN_DEVELOPER_FAX(L"123.456.7891");
RHINO_PLUG_IN_DEVELOPER_EMAIL(L"laiulfi@esri.com");
RHINO_PLUG_IN_DEVELOPER_WEBSITE(L"http://www.esri.com");
RHINO_PLUG_IN_UPDATE_URL(L"http://www.mycompany.com/support");

// The one and only CRhinoPRTPlugIn object
static class CRhinoPRTPlugIn thePlugIn;

/////////////////////////////////////////////////////////////////////////////
// CRhinoPRTPlugIn definition

CRhinoPRTPlugIn& RhinoPRTPlugIn()
{
	// Return a reference to the one and only CRhinoPRTPlugIn object
	return thePlugIn;
}

CRhinoPRTPlugIn::CRhinoPRTPlugIn()
{
	// Description:
	//   CRhinoPRTPlugIn constructor. The constructor is called when the
	//   plug-in is loaded and "thePlugIn" is constructed. Once the plug-in
	//   is loaded, CRhinoPRTPlugIn::OnLoadPlugIn() is called. The
	//   constructor should be simple and solid. Do anything that might fail in
	//   CRhinoPRTPlugIn::OnLoadPlugIn().

	// TODO: Add construction code here
	m_plugin_version = RhinoPlugInVersion();
}

/////////////////////////////////////////////////////////////////////////////
// Required overrides

const wchar_t* CRhinoPRTPlugIn::PlugInName() const
{
	// Description:
	//   Plug-in name display string.  This name is displayed by Rhino when
	//   loading the plug-in, in the plug-in help menu, and in the Rhino
	//   interface for managing plug-ins.

	// TODO: Return a short, friendly name for the plug-in.
	return RhinoPlugInName();
}

const wchar_t* CRhinoPRTPlugIn::PlugInVersion() const
{
	// Description:
	//   Plug-in version display string. This name is displayed by Rhino
	//   when loading the plug-in and in the Rhino interface for managing
	//   plug-ins.

	// TODO: Return the version number of the plug-in.
	return m_plugin_version;
}

GUID CRhinoPRTPlugIn::PlugInID() const
{
	// Description:
	//   Plug-in unique identifier. The identifier is used by Rhino to
	//   manage the plug-ins.

	// TODO: Return a unique identifier for the plug-in.
	// {1B8E4CAD-7A8F-4F05-B0A2-3BE0EC98C800}
	return ON_UuidFromString(RhinoPlugInId());
}

void CRhinoPRTPlugIn::initializePRT() {
	if (!prtCtx)
		prtCtx.reset(new PRTContext(prt::LOG_DEBUG));
}

bool CRhinoPRTPlugIn::isPRTInitialized() {
	return (bool)prtCtx;
}

void CRhinoPRTPlugIn::shutdownPRT() {
	prtCtx.reset();
}

void CRhinoPRTPlugIn::AddPagesToObjectPropertiesDialog(CRhinoPropertiesPanelPageCollection & collection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	PRTPropertyPanel* newPanel = new PRTPropertyPanel();

	//auto testPanel = new CSampleObjectPropertiesPageDialog();
	collection.Add(newPanel);
}

/////////////////////////////////////////////////////////////////////////////
// Additional overrides

BOOL CRhinoPRTPlugIn::OnLoadPlugIn()
{
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

	// TODO: Add plug-in initialization code here.
	initializePRT();

	return TRUE;
}

void CRhinoPRTPlugIn::OnUnloadPlugIn()
{
	// Description:
	//    Called one time when plug-in is about to be unloaded. By this time,
	//    Rhino's mainframe window has been destroyed, and some of the SDK
	//    managers have been deleted. There is also no active document or active
	//    view at this time. Thus, you should only be manipulating your own objects.
	//    or tools here.

	// TODO: Add plug-in cleanup code here.
	shutdownPRT();
	
}
