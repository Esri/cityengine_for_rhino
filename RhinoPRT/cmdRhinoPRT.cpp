// cmdRhinoPRT.cpp : command file
//

#include "StdAfx.h"
#include "RhinoPRTPlugIn.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN RhinoPRT command
//

#pragma region RhinoPRT command

// Do NOT put the definition of class CCommandRhinoPRT in a header
// file. There is only ONE instance of a CCommandRhinoPRT class
// and that instance is the static theRhinoPRTCommand that appears
// immediately below the class definition.

class CCommandRhinoPRT : public CRhinoCommand
{
public:
  // The one and only instance of CCommandRhinoPRT is created below.
  // No copy constructor or operator= is required.
  // Values of member variables persist for the duration of the application.

  // CCommandRhinoPRT::CCommandRhinoPRT()
  // is called exactly once when static theRhinoPRTCommand is created.
  CCommandRhinoPRT() = default;

  // CCommandRhinoPRT::~CCommandRhinoPRT()
  // is called exactly once when static theRhinoPRTCommand is destroyed.
  // The destructor should not make any calls to the Rhino SDK. 
  // If your command has persistent settings, then override 
  // CRhinoCommand::SaveProfile and CRhinoCommand::LoadProfile.
  ~CCommandRhinoPRT() = default;

  // Returns a unique UUID for this command.
  // If you try to use an id that is already being used, then
  // your command will not work. Use GUIDGEN.EXE to make unique UUID.
  UUID CommandUUID() override
  {
    // {C58CD8EE-318B-4C8B-8668-39F5EC9CF8A8}
    static const GUID RhinoPRTCommand_UUID =
    { 0xC58CD8EE, 0x318B, 0x4C8B, { 0x86, 0x68, 0x39, 0xF5, 0xEC, 0x9C, 0xF8, 0xA8 } };
    return RhinoPRTCommand_UUID;
  }

  // Returns the English command name.
  // If you want to provide a localized command name, then override 
  // CRhinoCommand::LocalCommandName.
  const wchar_t* EnglishCommandName() override { return L"RhinoPRT"; }

  // Rhino calls RunCommand to run the command.
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};

// The one and only CCommandRhinoPRT object
// Do NOT create any other instance of a CCommandRhinoPRT class.
static class CCommandRhinoPRT theRhinoPRTCommand;

CRhinoCommand::result CCommandRhinoPRT::RunCommand(const CRhinoCommandContext& context)
{
  // CCommandRhinoPRT::RunCommand() is called when the user
  // runs the "RhinoPRT".

  // TODO: Add command code here.

  // Rhino command that display a dialog box interface should also support
  // a command-line, or scriptable interface.

  ON_wString str;
  str.Format(L"The \"%s\" command is under construction.\n", EnglishCommandName());
  const wchar_t* pszStr = static_cast<const wchar_t*>(str);
  if (context.IsInteractive())
    RhinoMessageBox(pszStr, RhinoPRTPlugIn().PlugInName(), MB_OK);
  else
    RhinoApp().Print(pszStr);

  // TODO: Return one of the following values:
  //   CRhinoCommand::success:  The command worked.
  //   CRhinoCommand::failure:  The command failed because of invalid input, inability
  //                            to compute the desired result, or some other reason
  //                            computation reason.
  //   CRhinoCommand::cancel:   The user interactively canceled the command 
  //                            (by pressing ESCAPE, clicking a CANCEL button, etc.)
  //                            in a Get operation, dialog, time consuming computation, etc.

  return CRhinoCommand::success;
}

#pragma endregion

//
// END RhinoPRT command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
