#pragma once

#include <windef.h>

void uiWindowsEnsureDestroyWindow (HWND hwnd);

void uiWindowsEnsureSetParentHWND (HWND hwnd, HWND parent);

void uiWindowsEnsureAssignControlIDZOrder (HWND hwnd, LONG_PTR *controlID, HWND *insertAfter);

void uiWindowsEnsureMoveWindowDuringResize (HWND hwnd, int x, int y, int width, int height);

void uiWindowsEnsureGetClientRect (HWND hwnd, RECT *r);

void uiWindowsEnsureGetWindowRect (HWND hwnd, RECT *r);
