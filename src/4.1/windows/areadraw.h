#pragma once

#include <windef.h>

#include <ui/area.h>

extern BOOL areaDoDraw (uiArea *a, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult);

extern void areaDrawOnResize (uiArea *, RECT *);
