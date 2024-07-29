#include "tabpage.h"

#include "debug.h"
#include "init.h"
#include "parent.h"
#include "utilwin.h"

#include <ui_win32.h>
#include <uipriv.h>
#include <uxtheme.h>

static void
tabPageMargins (const tabPage *tp, int *mx, int *my)
{
  uiWindowsSizing sizing;

  *mx = 0;
  *my = 0;

  if (tp->margined == 0)
    return;

  uiWindowsGetSizing (tp->hwnd, &sizing);

  *mx = tabMargin;
  *my = tabMargin;

  uiWindowsSizingDlgUnitsToPixels (&sizing, mx, my);
}

static void
tabPageRelayout (const tabPage *tp)
{
  RECT r;
  int  mx;
  int  my;

  if (tp->child == nullptr)
    return;

  uiWindowsEnsureGetClientRect (tp->hwnd, &r);
  tabPageMargins (tp, &mx, &my);

  r.left += mx;
  r.top += my;
  r.right -= mx;
  r.bottom -= my;

  const auto child = reinterpret_cast<HWND> (uiControlHandle (tp->child));

  uiWindowsEnsureMoveWindowDuringResize (child, r.left, r.top, r.right - r.left, r.bottom - r.top);
}

static INT_PTR CALLBACK
dlgproc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  tabPage *tp;
  LRESULT  lResult;

  if (uMsg == WM_INITDIALOG)
    {
      tp       = reinterpret_cast<tabPage *> (lParam); // NOLINT(*-no-int-to-ptr)
      tp->hwnd = hwnd;
      SetWindowLongPtrW (hwnd, DWLP_USER, reinterpret_cast<LONG_PTR> (tp));
      return TRUE;
    }

  if (handleParentMessages (hwnd, uMsg, wParam, lParam, &lResult) != FALSE)
    {
      SetWindowLongPtrW (hwnd, DWLP_MSGRESULT, lResult);
      return TRUE;
    }

  if (uMsg == WM_WINDOWPOSCHANGED)
    {
      tp = reinterpret_cast<tabPage *> (GetWindowLongPtrW (hwnd, DWLP_USER)); // NOLINT(*-no-int-to-ptr)

      tabPageRelayout (tp);

      return FALSE;
    }

  return FALSE;
}

static const uint8_t data_rcTabPageDialog[] = {
  0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x50,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static_assert (ARRAYSIZE (data_rcTabPageDialog) == 32 /* NOLINT(*-magic-numbers) */,
               "wrong size for resource rcTabPageDialog");

tabPage *
newTabPage (uiControl *child)
{
  auto *tp = uiprivNew (struct tabPage);

  if (CreateDialogIndirectParamW (hInstance, reinterpret_cast<const DLGTEMPLATE *> (data_rcTabPageDialog), utilWindow,
                                  dlgproc, reinterpret_cast<LPARAM> (tp))
      == nullptr)
    (void)logLastError (L"error creating tab page");

  tp->child = child;
  if (tp->child != nullptr)
    {
      uiWindowsEnsureSetParentHWND (reinterpret_cast<HWND> (uiControlHandle (tp->child)), tp->hwnd);
      uiWindowsControlAssignSoleControlIDZOrder (uiWindowsControl (tp->child));
    }

  static constexpr auto flags = ETDT_ENABLE | ETDT_USETABTEXTURE | ETDT_ENABLETAB;
  const auto            hr    = EnableThemeDialogTexture (tp->hwnd, flags);
  if (hr != S_OK)
    (void)logHRESULT (L"error setting tab page background", hr);

  ShowWindow (tp->hwnd, SW_HIDE);

  return tp;
}

void
tabPageDestroy (tabPage *tp)
{
  if (tp->child != nullptr)
    uiWindowsControlSetParentHWND (uiWindowsControl (tp->child), nullptr);

  uiWindowsEnsureDestroyWindow (tp->hwnd);
  uiprivFree (tp);
}

void
tabPageMinimumSize (const tabPage *tp, int *width, int *height)
{
  int mx;
  int my;

  *width  = 0;
  *height = 0;

  if (tp->child != nullptr)
    uiWindowsControlMinimumSize (uiWindowsControl (tp->child), width, height);

  tabPageMargins (tp, &mx, &my);

  *width += 2 * mx;
  *height += 2 * my;
}
