#pragma once

#include <ui_win32.h>
#include <windef.h>

#include <ui/control.h>

#include <vector>

struct boxChild
{
  uiControl *c;

  int stretchy;

  int width;

  int height;
};

struct uiBox
{
  uiWindowsControl c;

  HWND hwnd;

  std::vector<boxChild> *controls;

  int vertical;

  int padded;
};
