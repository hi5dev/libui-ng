#pragma once

#include <windef.h>

#define INFINITE_HEIGHT 0x7FFF

extern DWORD getExStyle (HWND hwnd);

extern DWORD getStyle (HWND hwnd);

extern HWND getDlgItem (HWND hwnd, int id);

extern HWND parentOf (HWND child);

extern HWND parentToplevel (HWND child);

extern int windowClassOf (HWND hwnd, ...);

extern void clientSizeToWindowSize (HWND hwnd, int *width, int *height, BOOL hasMenubar);

extern void invalidateRect (HWND hwnd, const RECT *r, BOOL erase);

extern void mapWindowRect (HWND from, HWND to, RECT *r);

extern void setExStyle (HWND hwnd, DWORD exstyle);

extern void setStyle (HWND hwnd, DWORD style);

extern void setWindowInsertAfter (HWND hwnd, HWND insertAfter);
