#pragma once

#include <ui_win32.h>

#include <d2d1.h>

#include <ui/area.h>
#include <ui/areaevents.h>

#define areaClass L"libui_uiAreaClass"

struct uiArea
{
  uiWindowsControl c;
  HWND             hwnd;
  uiAreaHandler   *ah;

  BOOL scrolling;
  int  scrollWidth;
  int  scrollHeight;
  int  hscrollpos;
  int  vscrollpos;
  int  hwheelCarry;
  int  vwheelCarry;

  uiprivClickCounter cc;
  BOOL               capturing;

  BOOL inside;
  BOOL tracking;

  ID2D1HwndRenderTarget *rt;
};

extern ATOM registerAreaClass (HICON, HCURSOR);

extern void unregisterArea ();
