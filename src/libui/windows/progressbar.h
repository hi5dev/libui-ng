#pragma once

#include <windef.h>

#include <ui_win32.h>

#define pbarWidth  237
#define pbarHeight 8

#define indeterminate(p) ((getStyle (p->hwnd) & PBS_MARQUEE) != 0)

struct uiProgressBar
{
  uiWindowsControl c;

  HWND hwnd;
};
