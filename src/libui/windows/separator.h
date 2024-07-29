#pragma once

#include <windef.h>

#include <ui_win32.h>

#define separatorHeight 1
#define separatorWidth  1

struct uiSeparator
{
  uiWindowsControl c;

  HWND hwnd;

  BOOL vertical;
};
