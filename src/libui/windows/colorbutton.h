#pragma once

#include <windef.h>

#include <ui_win32.h>

#define buttonHeight 14

struct uiColorButton
{
  uiWindowsControl c;

  HWND hwnd;

  double r;

  double g;

  double b;

  double a;

  void (*onChanged) (uiColorButton *, void *);

  void *onChangedData;
};
