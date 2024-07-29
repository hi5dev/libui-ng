#pragma once

#include <windows.h>

extern ATOM registerD2DScratchClass (HICON hDefaultIcon, HCURSOR hDefaultCursor);

extern void unregisterD2DScratchClass ();

extern HWND newD2DScratch (HWND parent, RECT *rect, HMENU controlID, SUBCLASSPROC subclass, DWORD_PTR subclassData);
