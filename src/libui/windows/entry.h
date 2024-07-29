#pragma once

#include <windef.h>

#include <ui_win32.h>

#define entryWidth 107
#define entryHeight 14

struct uiEntry
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onChanged) (uiEntry *, void *);

  void *onChangedData;

  BOOL inhibitChanged;
};
