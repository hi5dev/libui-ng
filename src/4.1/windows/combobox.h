#pragma once

#include <windef.h>

#include <ui_win32.h>

#define comboboxWidth  107
#define comboboxHeight 14

struct uiCombobox
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onSelected) (uiCombobox *, void *);

  void *onSelectedData;
};
