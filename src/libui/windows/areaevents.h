#pragma once

#include <winuser.h>

#include <ui/area.h>
#include <ui/areaevents.h>

extern BOOL areaFilter (MSG *);

extern BOOL areaDoEvents (uiArea *a, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult);
