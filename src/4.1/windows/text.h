#pragma once

#include <windef.h>

#define labelHeight 8

extern WCHAR *windowTextAndLen (HWND hwnd, LRESULT *len);

extern WCHAR *windowText (HWND hwnd);

extern void setWindowText (HWND hwnd, const WCHAR *wtext);
