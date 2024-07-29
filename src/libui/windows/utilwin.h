#pragma once

#include <windef.h>

extern HWND utilWindow;

extern const char *initUtilWindow (HICON hDefaultIcon, HCURSOR hDefaultCursor);

extern void uninitUtilWindow ();
