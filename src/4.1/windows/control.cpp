#include "debug.h"
#include "init.h"
#include "utilwin.h"

#include <ui_win32.h>

#define uiWindowsControlSignature 0x4D53576E

void
uiWindowsControlSyncEnableState (uiWindowsControl *c, const int enabled)
{
  (*c->SyncEnableState) (c, enabled);
}

void
uiWindowsControlSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  (*c->SetParentHWND) (c, parent);
}

void
uiWindowsControlMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  (*c->MinimumSize) (c, width, height);
}

void
uiWindowsControlMinimumSizeChanged (uiWindowsControl *c)
{
  (*c->MinimumSizeChanged) (c);
}

void
uiWindowsControlLayoutRect (uiWindowsControl *c, RECT *r)
{
  (*c->LayoutRect) (c, r);
}

void
uiWindowsControlAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  (*c->AssignControlIDZOrder) (c, controlID, insertAfter);
}

void
uiWindowsControlChildVisibilityChanged (uiWindowsControl *c)
{
  (*c->ChildVisibilityChanged) (c);
}

HWND
uiWindowsEnsureCreateControlHWND (const DWORD dwExStyle, const LPCWSTR lpClassName, const LPCWSTR lpWindowName,
                                  DWORD dwStyle, const HINSTANCE hInstance, const LPVOID lpParam,
                                  const BOOL useStandardControlFont)
{
  // don't let using the arrow keys in a uiRadioButtons leave the radio buttons
  if ((dwStyle & WS_TABSTOP) != 0)
    dwStyle |= WS_GROUP;

  const HWND hwnd = CreateWindowExW (dwExStyle, lpClassName, lpWindowName, dwStyle | WS_CHILD | WS_VISIBLE, 0, 0, 100,
                                     100, utilWindow, nullptr, hInstance, lpParam);
  if (hwnd == nullptr)
    (void)logLastError (L"error creating window");

  if (useStandardControlFont != 0)
    SendMessageW (hwnd, WM_SETFONT, reinterpret_cast<WPARAM> (hMessageFont), TRUE);

  return hwnd;
}

uiWindowsControl *
uiWindowsAllocControl (const size_t n, const uint32_t typesig, const char *typenamestr)
{
  return uiWindowsControl (uiAllocControl (n, uiWindowsControlSignature, typesig, typenamestr));
}

BOOL
uiWindowsShouldStopSyncEnableState (uiWindowsControl *c, const BOOL enabled)
{
  const int ce = uiControlEnabled (reinterpret_cast<uiControl *> (c));

  // only stop if we're going from disabled back to enabled; don't stop under any other condition
  // (if we stop when going from enabled to disabled then enabled children of a disabled control won't get disabled at
  // the OS level)
  if (ce == 0 && enabled != 0)
    return TRUE;

  return FALSE;
}

void
uiWindowsControlAssignSoleControlIDZOrder (uiWindowsControl *c)
{
  LONG_PTR controlID   = 100;
  HWND     insertAfter = nullptr;

  uiWindowsControlAssignControlIDZOrder (c, &controlID, &insertAfter);
}

BOOL
uiWindowsControlTooSmall (uiWindowsControl *c)
{
  RECT r;
  uiWindowsControlLayoutRect (c, &r);

  int width;
  int height;
  uiWindowsControlMinimumSize (c, &width, &height);

  if (r.right - r.left < width)
    return TRUE;

  if (r.bottom - r.top < height)
    return TRUE;

  return FALSE;
}

void
uiWindowsControlContinueMinimumSizeChanged (uiWindowsControl *c)
{
  uiControl *parent = uiControlParent (reinterpret_cast<uiControl *> (c));

  if (parent != nullptr)
    uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (parent));
}

void
uiWindowsControlNotifyVisibilityChanged (uiWindowsControl *c)
{
  uiWindowsControlContinueMinimumSizeChanged (c);
}
