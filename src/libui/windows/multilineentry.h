#pragma once

#include <ui_win32.h>

#include <ui/multiline_entry.h>

struct uiMultilineEntry
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onChanged) (uiMultilineEntry *, void *);

  void *onChangedData;

  BOOL inhibitChanged;
};
