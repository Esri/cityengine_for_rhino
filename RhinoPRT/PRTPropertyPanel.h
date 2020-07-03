#pragma once

#include "stdafx.h"

#include "rhinoSdk.h"

#include "PRTPropertyWindow.h"

class PRTPropertyPanel: public IRhinoPropertiesPanelPage
{
public:

	PRTPropertyPanel();
	~PRTPropertyPanel() {};

	// Inherited via IRhinoPropertiesPanelPage
	virtual const wchar_t * EnglishTitle() const override;
	virtual IRhinoWindow * Window() override;
	virtual RHINO_WINDOW_IMAGE_HANDLE Image(ON_2iSize size) override;
	virtual void Release() override;
	virtual bool IncludeInNavigationControl(IRhinoPropertiesPanelPageEventArgs & args) const override;
	virtual CRhinoCommand::result RunScript(IRhinoPropertiesPanelPageEventArgs & args) override;
	virtual void UpdatePage(IRhinoPropertiesPanelPageEventArgs & e) override;

private:
	std::wstring mEnglishName;
	PRTPropertyWindow* mWindow;
};

