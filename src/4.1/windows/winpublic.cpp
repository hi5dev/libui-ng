#include <windows.h>

#include "debug.h"
#include "utilwin.h"
#include "winpublic.h"
#include "winutil.h"

void
uiWindowsEnsureAssignControlIDZOrder (const HWND hwnd, LONG_PTR *controlID, HWND *insertAfter)
{
  SetWindowLongPtrW (hwnd, GWLP_ID, *controlID);
  (*controlID)++;

  setWindowInsertAfter (hwnd, *insertAfter);
  *insertAfter = hwnd;
}

void
uiWindowsEnsureDestroyWindow (const HWND hwnd)
{
  if (DestroyWindow (hwnd) == 0)
    (void)logLastError (L"error destroying window");
}

void
uiWindowsEnsureGetClientRect (const HWND hwnd, RECT *r)
{
  if (GetClientRect (hwnd, r) != 0)
    return;

  (void)logLastError (L"error getting window client rect");

  r->left   = 0;
  r->top    = 0;
  r->right  = 0;
  r->bottom = 0;
}

void
uiWindowsEnsureGetWindowRect (const HWND hwnd, RECT *r)
{
  if (GetWindowRect (hwnd, r) != 0)
    return;

  (void)logLastError (L"error getting window rect");

  r->left   = 0;
  r->top    = 0;
  r->right  = 0;
  r->bottom = 0;
}

void
uiWindowsEnsureMoveWindowDuringResize (const HWND hwnd, const int x, const int y, const int width, const int height)
{
  RECT r;
  r.left   = x;
  r.top    = y;
  r.right  = x + width;
  r.bottom = y + height;

  static constexpr UINT flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER;

  if (SetWindowPos (hwnd, nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top, flags) == 0)
    (void)logLastError (L"error moving window");
}

void
uiWindowsEnsureSetParentHWND (const HWND hwnd, HWND parent)
{
  if (parent == nullptr)
    parent = utilWindow;

  if (SetParent (hwnd, parent) == nullptr)
    (void)logLastError (L"error setting window parent");
}
