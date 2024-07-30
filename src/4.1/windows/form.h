#pragma once

#include <windef.h>

#include <ui/control.h>

#include <ui_win32.h>
#include <vector>

#define labelHeight  8
#define labelYOffset 3

struct formChild
{
  uiControl *c;

  HWND label;

  int stretchy;

  int height;
};

struct uiForm
{
  uiWindowsControl c;

  HWND hwnd;

  std::vector<formChild> *controls;

  int padded;
};
