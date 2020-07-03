#include "PRTPropertyPanel.h"

PRTPropertyPanel::PRTPropertyPanel(): IRhinoPropertiesPanelPage(), mEnglishName(L"PRT Parameters") {
	mWindow = new PRTPropertyWindow(mEnglishName);
}

const wchar_t * PRTPropertyPanel::EnglishTitle() const
{
	return this->mEnglishName.c_str();
}

IRhinoWindow * PRTPropertyPanel::Window()
{
	return mWindow;
}

RHINO_WINDOW_IMAGE_HANDLE PRTPropertyPanel::Image(ON_2iSize size)
{
	return RHINO_WINDOW_IMAGE_HANDLE();
}

void PRTPropertyPanel::Release()
{
}

bool PRTPropertyPanel::IncludeInNavigationControl(IRhinoPropertiesPanelPageEventArgs & args) const
{
	return false;
}

CRhinoCommand::result PRTPropertyPanel::RunScript(IRhinoPropertiesPanelPageEventArgs & args)
{
	return CRhinoCommand::result();
}

void PRTPropertyPanel::UpdatePage(IRhinoPropertiesPanelPageEventArgs & e)
{
}
