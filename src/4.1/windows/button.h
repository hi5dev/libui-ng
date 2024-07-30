#pragma once

#include <windef.h>

#include <ui_win32.h>

#define buttonHeight 14

struct uiButton
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onClicked) (uiButton *, void *);

  void *onClickedData;
};
