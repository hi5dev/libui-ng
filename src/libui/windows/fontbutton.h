#pragma once

#include <windef.h>

#include <ui_win32.h>

#include "fontdialog.h"

#define buttonHeight 14

struct uiFontButton
{
  uiWindowsControl c;

  HWND hwnd;

  fontDialogParams params;

  BOOL already;

  void (*onChanged) (uiFontButton *, void *);

  void *onChangedData;
};
