#pragma once

#include <ui_win32.h>

#include <ui/multiline_entry.h>

#define entryWidth  107
#define entryHeight 14

struct uiMultilineEntry
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onChanged) (uiMultilineEntry *, void *);

  void *onChangedData;

  BOOL inhibitChanged;
};
