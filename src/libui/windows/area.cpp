#include "area.h"

#include "areadraw.h"
#include "areaevents.h"
#include "areascroll.h"
#include "debug.h"
#include "draw.h"
#include "init.h"
#include "winutil.h"

#include <controlsigs.h>

static LRESULT CALLBACK
areaWndProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  RECT    client;
  LRESULT lResult;

  const CREATESTRUCTW *cs = reinterpret_cast<CREATESTRUCTW *> (lParam);           // NOLINT(*-no-int-to-ptr)
  const WINDOWPOS     *wp = reinterpret_cast<WINDOWPOS *> (lParam);               // NOLINT(*-no-int-to-ptr)
  auto *a = reinterpret_cast<uiArea *> (GetWindowLongPtrW (hwnd, GWLP_USERDATA)); // NOLINT(*-no-int-to-ptr)

  if (a == nullptr)
    {
      if (uMsg == WM_CREATE)
        {
          a = static_cast<uiArea *> (cs->lpCreateParams);

          a->hwnd = hwnd;

          SetWindowLongPtrW (hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (a));
        }

      return DefWindowProcW (hwnd, uMsg, wParam, lParam);
    }

  // always recreate the render target if necessary
  if (a->rt == nullptr)
    a->rt = makeHWNDRenderTarget (a->hwnd);

  if (areaDoDraw (a, uMsg, wParam, lParam, &lResult) != FALSE)
    return lResult;

  if (uMsg == WM_WINDOWPOSCHANGED)
    {
      if ((wp->flags & SWP_NOSIZE) != 0)
        return DefWindowProcW (hwnd, uMsg, wParam, lParam);
      uiWindowsEnsureGetClientRect (a->hwnd, &client);
      areaDrawOnResize (a, &client);
      areaScrollOnResize (a, &client);
      return 0;
    }

  if (areaDoScroll (a, uMsg, wParam, lParam, &lResult) != FALSE)
    return lResult;

  if (areaDoEvents (a, uMsg, wParam, lParam, &lResult) != FALSE)
    return lResult;

  return DefWindowProc (hwnd, uMsg, wParam, lParam);
}

static void
uiAreaDestroy (uiControl *c)
{
  uiWindowsEnsureDestroyWindow (reinterpret_cast<uiArea *> (c)->hwnd);
  uiFreeControl (c);
}

static uintptr_t
uiAreaHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiArea *> (c)->hwnd);
}

static uiControl *
uiAreaParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiAreaSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiAreaToplevel (uiControl *c)
{
  return 0;
}

static int
uiAreaVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiAreaShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiArea *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiAreaHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiArea *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiAreaEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiAreaEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiAreaDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiAreaSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;

  EnableWindow (reinterpret_cast<uiArea *> (c)->hwnd, enabled);
}

static void
uiAreaSetParentHWND (uiWindowsControl *c, HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiArea *> (c)->hwnd, parent);
}

static void
uiAreaMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiAreaLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiArea *> (c)->hwnd, r);
}

static void
uiAreaAssignControlIDZOrder (uiWindowsControl *c, const LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiArea *> (c)->hwnd, controlID, insertAfter);
}

static void
uiAreaChildVisibilityChanged (uiWindowsControl *)
{
  // no-op
}

static void
uiAreaMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  *width  = 0;
  *height = 0;
}

ATOM
registerAreaClass (const HICON hDefaultIcon, const HCURSOR hDefaultCursor)
{
  WNDCLASSW wc;

  ZeroMemory (&wc, sizeof (WNDCLASSW));

  wc.lpszClassName = areaClass;
  wc.lpfnWndProc   = areaWndProc;
  wc.hInstance     = hInstance;
  wc.hIcon         = hDefaultIcon;
  wc.hCursor       = hDefaultCursor;
  wc.hbrBackground = reinterpret_cast<HBRUSH> ((COLOR_BTNFACE + 1)); // NOLINT(*-no-int-to-ptr)
  wc.style         = CS_HREDRAW | CS_VREDRAW;

  return RegisterClassW (&wc);
}

void
unregisterArea ()
{
  if (UnregisterClassW (areaClass, hInstance) == 0)
    (void)logLastError (L"error unregistering uiArea window class");
}

void
uiAreaSetSize (uiArea *a, const int width, const int height)
{
  a->scrollWidth  = width;
  a->scrollHeight = height;
  areaUpdateScroll (a);
}

void
uiAreaQueueRedrawAll (const uiArea *a)
{
  invalidateRect (a->hwnd, nullptr, FALSE);
}

void
uiAreaScrollTo (uiArea *, double, double, double, double)
{
}

void
uiAreaBeginUserWindowMove (const uiArea *a)
{
  ReleaseCapture ();

  const HWND toplevel = parentToplevel (a->hwnd);

  if (toplevel == nullptr)
    return;

  SendMessageW (toplevel, WM_SYSCOMMAND, SC_MOVE | 2, 0);
}

void
uiAreaBeginUserWindowResize (const uiArea *a, const uiWindowResizeEdge edge)
{
  ReleaseCapture ();

  const HWND toplevel = parentToplevel (a->hwnd);
  if (toplevel == nullptr)
    return;

  WPARAM wParam = SC_SIZE;
  switch (edge)
    {
    case uiWindowResizeEdgeLeft:
      wParam |= 1;
      break;

    case uiWindowResizeEdgeTop:
      wParam |= 3;
      break;

    case uiWindowResizeEdgeRight:
      wParam |= 2;
      break;

    case uiWindowResizeEdgeBottom:
      wParam |= 6; // NOLINT(*-magic-numbers)
      break;

    case uiWindowResizeEdgeTopLeft:
      wParam |= 4;
      break;

    case uiWindowResizeEdgeTopRight:
      wParam |= 5; // NOLINT(*-magic-numbers)
      break;

    case uiWindowResizeEdgeBottomLeft:
      wParam |= 7; // NOLINT(*-magic-numbers)
      break;

    case uiWindowResizeEdgeBottomRight:
      wParam |= 8; // NOLINT(*-magic-numbers)
      break;

    default:
      break;
    }
  SendMessageW (toplevel, WM_SYSCOMMAND, wParam, 0);
}

uiArea *
uiNewArea (uiAreaHandler *ah)
{
  uiArea *a;

  uiWindowsNewControl (uiArea, a);

  a->ah        = ah;
  a->scrolling = FALSE;
  uiprivClickCounterReset (&a->cc);

  // a->hwnd is assigned in areaWndProc()
  uiWindowsEnsureCreateControlHWND (0, areaClass, L"", 0, hInstance, a, FALSE);

  return a;
}

uiArea *
uiNewScrollingArea (uiAreaHandler *ah, const int width, const int height)
{
  auto *a         = reinterpret_cast<uiArea *> (uiWindowsAllocControl (sizeof (uiArea), uiAreaSignature, "uiArea"));
  a->ah           = ah;
  a->scrolling    = TRUE;
  a->scrollWidth  = width;
  a->scrollHeight = height;

  auto *control      = reinterpret_cast<uiControl *> (a);
  control->Destroy   = uiAreaDestroy;
  control->Handle    = uiAreaHandle;
  control->Parent    = uiAreaParent;
  control->SetParent = uiAreaSetParent;
  control->Toplevel  = uiAreaToplevel;
  control->Visible   = uiAreaVisible;
  control->Show      = uiAreaShow;
  control->Hide      = uiAreaHide;
  control->Enabled   = uiAreaEnabled;
  control->Enable    = uiAreaEnable;
  control->Disable   = uiAreaDisable;

  auto *windowsControl                   = reinterpret_cast<uiWindowsControl *> (a);
  windowsControl->SyncEnableState        = uiAreaSyncEnableState;
  windowsControl->SetParentHWND          = uiAreaSetParentHWND;
  windowsControl->MinimumSize            = uiAreaMinimumSize;
  windowsControl->MinimumSizeChanged     = uiAreaMinimumSizeChanged;
  windowsControl->LayoutRect             = uiAreaLayoutRect;
  windowsControl->AssignControlIDZOrder  = uiAreaAssignControlIDZOrder;
  windowsControl->ChildVisibilityChanged = uiAreaChildVisibilityChanged;
  windowsControl->visible                = 1;
  windowsControl->enabled                = 1;

  uiprivClickCounterReset (&a->cc);

  uiWindowsEnsureCreateControlHWND (0, areaClass, L"", WS_HSCROLL | WS_VSCROLL, hInstance, a, FALSE);

  areaUpdateScroll (a);

  return a;
}
