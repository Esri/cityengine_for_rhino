#pragma once

#include "stdafx.h"

#include "rhinoSdk.h"

class PRTPropertyWindow: public IRhinoWindow
{
public:
	PRTPropertyWindow(std::wstring window_title): IRhinoWindow(), mTitle(window_title) {}
	~PRTPropertyWindow() {}

	// Inherited via IRhinoWindow
	virtual bool Create(const CRhinoWindowCreateOptions & options) override;
	virtual bool Destroy() override;
	virtual const wchar_t * EnglishCaption() const override;
	virtual void SetParent(RHINO_WINDOW_HANDLE parent) override;
	virtual RHINO_WINDOW_HANDLE Parent() const override;
	virtual bool Created() const override;
	virtual void Release() override;
	virtual ON_2iPoint Location() const override;
	virtual void SetLocation(ON_2iPoint location) override;
	virtual ON_2iSize Size() const override;
	virtual void SetSize(ON_2iSize size, bool redraw = false, bool redrawBorder = false) override;
	virtual void SetIsVisible(bool visible) override;
	virtual bool IsVisible() const override;
	virtual void SetIsEnabled(bool enable) override;
	virtual bool IsEnabled() const override;
	virtual void SetFocused() override;
	virtual void Refresh(bool immediate = false) override;
	virtual void GetScreenCoords(ON_4iRect & rectOut) const override;
	virtual RHINO_WINDOW_HANDLE Handle(void) override;

private:
	std::wstring mTitle;
};

