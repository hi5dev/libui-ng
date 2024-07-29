#pragma once

#include <windef.h>
#include <winuser.h>

#include <ui_win32.h>

#define windowClass  L"libui_uiWindowClass"
#define windowMargin 7

#define INFINITE_HEIGHT 0x7FFF

/// @implements uiWindow
struct uiWindow
{
  uiWindowsControl c;

  HWND hwnd;

  HMENU menubar;

  uiControl *child;

  BOOL shownOnce;

  int visible;

  int margined;

  int resizeable;

  BOOL hasMenubar;

  BOOL changingSize;

  int fullscreen;

  WINDOWPLACEMENT fsPrevPlacement;

  int borderless;

  int focused;

  int (*onClosing) (uiWindow *, void *);

  void *onClosingData;

  void (*onContentSizeChanged) (uiWindow *, void *);

  void *onContentSizeChangedData;

  void (*onFocusChanged) (uiWindow *, void *);

  void *onFocusChangedData;

  void (*onPositionChanged) (uiWindow *, void *);

  void *onPositionChangedData;

  BOOL changingPosition;
};

extern ATOM registerWindowClass (HICON, HCURSOR);

extern void unregisterWindowClass ();

extern void ensureMinimumWindowSize (uiWindow *);

extern void disableAllWindowsExcept (const uiWindow *which);

extern void enableAllWindowsExcept (const uiWindow *which);
