#pragma once

#include <windef.h>

#include <ui/spinbox.h>

#include <ui_win32.h>

#define entryWidth  107
#define entryHeight 14

struct uiSpinbox
{
  uiWindowsControl c;

  HWND hwnd;

  HWND edit;

  HWND updown;

  void (*onChanged) (uiSpinbox *, void *);

  void *onChangedData;

  BOOL inhibitChanged;
};
