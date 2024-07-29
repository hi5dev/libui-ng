#include "group.h"

#include "debug.h"
#include "init.h"
#include "parent.h"
#include "utf16.h"
#include "winpublic.h"

#include <ui/group.h>

#include <algorithm>
#include <commctrl.h>
#include <controlsigs.h>
#include <uipriv.h>

static void
groupMargins (const uiGroup *g, int *mx, int *mtop, int *mbottom)
{
  uiWindowsSizing sizing;

  *mx      = groupUnmarginedXMargin;
  *mtop    = groupUnmarginedYMarginTop;
  *mbottom = groupUnmarginedYMarginBottom;

  if (g->margined != 0)
    {
      *mx      = groupXMargin;
      *mtop    = groupYMarginTop;
      *mbottom = groupYMarginBottom;
    }

  uiWindowsGetSizing (g->hwnd, &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, mx, mtop);
  uiWindowsSizingDlgUnitsToPixels (&sizing, nullptr, mbottom);
}

static void
groupRelayout (const uiGroup *g)
{
  if (g->child == nullptr)
    return;

  RECT r;
  uiWindowsEnsureGetClientRect (g->hwnd, &r);

  int mx;
  int mtop;
  int mbottom;
  groupMargins (g, &mx, &mtop, &mbottom);

  r.left += mx;
  r.top += mtop;
  r.right -= mx;
  r.bottom -= mbottom;

  uiWindowsEnsureMoveWindowDuringResize (reinterpret_cast<HWND> (uiControlHandle (g->child)), r.left, r.top,
                                         r.right - r.left, r.bottom - r.top);
}

static void
uiGroupDestroy (uiControl *c)
{
  auto *g = reinterpret_cast<uiGroup *> (c);

  if (g->child != nullptr)
    {
      uiControlSetParent (g->child, nullptr);
      uiControlDestroy (g->child);
    }

  uiWindowsEnsureDestroyWindow (g->hwnd);
  uiFreeControl (reinterpret_cast<uiControl *> (g));
}

static uintptr_t
uiGroupHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiGroup *> (c)->hwnd);
}

static uiControl *
uiGroupParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiGroupSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  uiWindowsControl (c)->parent = parent;
}

static int
uiGroupToplevel (uiControl *)
{
  return 0;
}

static int
uiGroupVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiGroupShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiGroup *> (c)->hwnd, SW_SHOW);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiGroupHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiGroup *> (c)->hwnd, SW_HIDE);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiGroupEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiGroupEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiGroupDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiGroupSyncEnableState (uiWindowsControl *c, const int enabled)
{
  auto *g = reinterpret_cast<uiGroup *> (c);

  if (uiWindowsShouldStopSyncEnableState (reinterpret_cast<uiWindowsControl *> (g), enabled) != 0)
    return;

  EnableWindow (g->hwnd, enabled);
  if (g->child != nullptr)
    uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (g->child), enabled);
}

static void
uiGroupSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiGroup *> (c)->hwnd, parent);
}

static void
uiGroupMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto *g = reinterpret_cast<uiGroup *> (c);

  int mx;
  int mtop;
  int mbottom;

  *width  = 0;
  *height = 0;
  if (g->child != nullptr)
    uiWindowsControlMinimumSize (uiWindowsControl (g->child), width, height);

  const int labelWidth = uiWindowsWindowTextWidth (g->hwnd);

  *width = std::max (*width, labelWidth);

  groupMargins (g, &mx, &mtop, &mbottom);
  *width += 2 * mx;
  *height += mtop + mbottom;
}

static void
uiGroupMinimumSizeChanged (uiWindowsControl *c)
{
  auto *g = reinterpret_cast<uiGroup *> (c);

  if (uiWindowsControlTooSmall (reinterpret_cast<uiWindowsControl *> (g)) != 0)
    {
      uiWindowsControlContinueMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (g));
      return;
    }

  groupRelayout (g);
}

static void
uiGroupLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiGroup *> (c)->hwnd, r);
}

static void
uiGroupAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiGroup *> (c)->hwnd, controlID, insertAfter);
}

static void
uiGroupChildVisibilityChanged (uiWindowsControl *c)
{
  uiWindowsControlMinimumSizeChanged (c);
}

char *
uiGroupTitle (const uiGroup *g)
{
  return uiWindowsWindowText (g->hwnd);
}

void
uiGroupSetTitle (uiGroup *g, const char *title)
{
  uiWindowsSetWindowText (g->hwnd, title);
  uiWindowsControlMinimumSizeChanged (uiWindowsControl (g));
}

void
uiGroupSetChild (uiGroup *g, uiControl *c)
{
  if (g->child != nullptr)
    {
      uiControlSetParent (g->child, nullptr);
      uiWindowsControlSetParentHWND (reinterpret_cast<uiWindowsControl *> (g->child), nullptr);
    }

  g->child = c;

  if (g->child != nullptr)
    {
      uiControlSetParent (g->child, reinterpret_cast<uiControl *> (g));
      uiWindowsControlSetParentHWND (reinterpret_cast<uiWindowsControl *> (g->child), g->hwnd);
      uiWindowsControlAssignSoleControlIDZOrder (reinterpret_cast<uiWindowsControl *> (g->child));
      uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (g));
    }
}

int
uiGroupMargined (const uiGroup *g)
{
  return g->margined;
}

void
uiGroupSetMargined (uiGroup *g, const int margined)
{
  g->margined = margined;
  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (g));
}

static LRESULT CALLBACK
groupSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, const UINT_PTR uIdSubclass,
              const DWORD_PTR dwRefData)
{
  auto       *g   = reinterpret_cast<uiGroup *> (dwRefData);
  const auto *wp  = reinterpret_cast<WINDOWPOS *> (lParam);
  auto       *mmi = reinterpret_cast<MINMAXINFO *> (lParam);

  int minwid;
  int minht;

  LRESULT lResult;

  if (handleParentMessages (hwnd, uMsg, wParam, lParam, &lResult) != FALSE)
    return lResult;

  switch (uMsg)
    {
    case WM_WINDOWPOSCHANGED:
      {
        if ((wp->flags & SWP_NOSIZE) != 0)
          break;
        groupRelayout (g);
        return 0;
      }

    case WM_GETMINMAXINFO:
      {
        lResult = DefWindowProcW (hwnd, uMsg, wParam, lParam);

        uiWindowsControlMinimumSize (uiWindowsControl (g), &minwid, &minht);

        mmi->ptMinTrackSize.x = minwid;
        mmi->ptMinTrackSize.y = minht;

        return lResult;
      }

    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, groupSubProc, uIdSubclass) == FALSE)
          (void)logLastError (L"error removing groupbox subclass");

        break;
      }

    default:;
    }
  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

uiGroup *
uiNewGroup (const char *title)
{
  auto *g = reinterpret_cast<uiGroup *> (uiWindowsAllocControl (sizeof (uiGroup), uiGroupSignature, "uiGroup"));

  auto *control      = reinterpret_cast<uiControl *> (g);
  control->Destroy   = uiGroupDestroy;
  control->Disable   = uiGroupDisable;
  control->Enable    = uiGroupEnable;
  control->Enabled   = uiGroupEnabled;
  control->Handle    = uiGroupHandle;
  control->Hide      = uiGroupHide;
  control->Parent    = uiGroupParent;
  control->SetParent = uiGroupSetParent;
  control->Show      = uiGroupShow;
  control->Toplevel  = uiGroupToplevel;
  control->Visible   = uiGroupVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (g);
  windows_control->AssignControlIDZOrder  = uiGroupAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiGroupChildVisibilityChanged;
  windows_control->LayoutRect             = uiGroupLayoutRect;
  windows_control->MinimumSize            = uiGroupMinimumSize;
  windows_control->MinimumSizeChanged     = uiGroupMinimumSizeChanged;
  windows_control->SetParentHWND          = uiGroupSetParentHWND;
  windows_control->SyncEnableState        = uiGroupSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  auto *wtext = toUTF16 (title);
  g->hwnd = uiWindowsEnsureCreateControlHWND (WS_EX_CONTROLPARENT, L"button", wtext, BS_GROUPBOX, hInstance, nullptr,
                                              TRUE);
  uiprivFree (wtext);

  if (SetWindowSubclass (g->hwnd, groupSubProc, 0, reinterpret_cast<DWORD_PTR> (g)) == FALSE)
    (void)logLastError (L"error subclassing groupbox to handle parent messages");

  return g;
}
