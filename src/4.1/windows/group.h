#pragma once

#include <windef.h>

#include <ui/control.h>
#include <ui_win32.h>

#define groupXMargin       6
#define groupYMarginBottom 7
#define groupYMarginTop    11

#define groupUnmarginedXMargin       4
#define groupUnmarginedYMarginBottom 3
#define groupUnmarginedYMarginTop    8

struct uiGroup
{
  uiWindowsControl c;

  HWND hwnd;

  uiControl *child;

  int margined;
};
