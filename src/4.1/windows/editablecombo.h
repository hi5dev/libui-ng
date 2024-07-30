#pragma once

#include <windef.h>

#include <ui_win32.h>

#define comboboxWidth  107
#define comboboxHeight 14

struct uiEditableCombobox
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onChanged) (uiEditableCombobox *, void *);

  void *onChangedData;
};
