#pragma once

#include <windows.h>

#include <d2d1.h>

#include <ui_win32.h>

extern void getSizing (HWND hwnd, uiWindowsSizing *sizing, HFONT font);

extern D2D1_SIZE_F realGetSize (ID2D1RenderTarget *rt);
