#pragma once

#include <windef.h>

#include <ui_win32.h>

#define checkboxHeight                      10
#define checkboxXFromLeftOfBoxToLeftOfLabel 12

struct uiCheckbox
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onToggled) (uiCheckbox *, void *);

  void *onToggledData;
};
