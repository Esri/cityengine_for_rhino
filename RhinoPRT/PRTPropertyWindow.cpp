#include "PRTPropertyWindow.h"

bool PRTPropertyWindow::Create(const CRhinoWindowCreateOptions & options)
{
	return true;
}

bool PRTPropertyWindow::Destroy()
{
	return false;
}

const wchar_t * PRTPropertyWindow::EnglishCaption() const
{
	return mTitle.c_str();
}

void PRTPropertyWindow::SetParent(RHINO_WINDOW_HANDLE parent)
{
}

RHINO_WINDOW_HANDLE PRTPropertyWindow::Parent() const
{
	return RHINO_WINDOW_HANDLE();
}

bool PRTPropertyWindow::Created() const
{
	return false;
}

void PRTPropertyWindow::Release()
{
}

ON_2iPoint PRTPropertyWindow::Location() const
{
	return ON_2iPoint();
}

void PRTPropertyWindow::SetLocation(ON_2iPoint location)
{
}

ON_2iSize PRTPropertyWindow::Size() const
{
	return ON_2iSize();
}

void PRTPropertyWindow::SetSize(ON_2iSize size, bool redraw, bool redrawBorder)
{
}

void PRTPropertyWindow::SetIsVisible(bool visible)
{
	
}

bool PRTPropertyWindow::IsVisible() const
{
	return true;
}

void PRTPropertyWindow::SetIsEnabled(bool enable)
{
}

bool PRTPropertyWindow::IsEnabled() const
{
	return true;
}

void PRTPropertyWindow::SetFocused()
{
}

void PRTPropertyWindow::Refresh(bool immediate)
{
}

void PRTPropertyWindow::GetScreenCoords(ON_4iRect & rectOut) const
{
}

RHINO_WINDOW_HANDLE PRTPropertyWindow::Handle(void)
{
	return RHINO_WINDOW_HANDLE();
}
