#pragma once

#include <windef.h>

#include <ui_win32.h>

#define entryHeight 14

#define GLI(what, buf, n) GetLocaleInfoEx (LOCALE_NAME_USER_DEFAULT, what, buf, n)

struct uiDateTimePicker
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onChanged) (uiDateTimePicker *, void *);

  void *onChangedData;
};
