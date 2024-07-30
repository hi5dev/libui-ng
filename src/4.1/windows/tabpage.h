#pragma once

#include <windef.h>

#include <ui/control.h>

#define tabMargin 7

struct tabPage
{
  HWND       hwnd;
  uiControl *child;
  BOOL       margined;
};

extern tabPage *newTabPage (uiControl *child);

extern void tabPageDestroy (tabPage *tp);

extern void tabPageMinimumSize (const tabPage *tp, int *width, int *height);
