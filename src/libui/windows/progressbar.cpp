#include <commctrl.h>

#include <controlsigs.h>
#include <ui/progressbar.h>
#include <ui/userbugs.h>

#include "init.h"
#include "progressbar.h"
#include "winpublic.h"
#include "winutil.h"

static void
uiProgressBarDestroy (uiControl *c)
{
  uiWindowsEnsureDestroyWindow (reinterpret_cast<uiProgressBar *> (c)->hwnd);
  uiFreeControl (c);
}

static uintptr_t
uiProgressBarHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiProgressBar *> (c)->hwnd);
}

static uiControl *
uiProgressBarParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiProgressBarSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiProgressBarToplevel (uiControl *)
{
  return 0;
}

static int
uiProgressBarVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiProgressBarShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiProgressBar *> (c)->hwnd, SW_SHOW);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiProgressBarHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiProgressBar *> (c)->hwnd, SW_HIDE);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiProgressBarEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiProgressBarEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiProgressBarDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiProgressBarSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;
  EnableWindow (reinterpret_cast<uiProgressBar *> (c)->hwnd, enabled);
}

static void
uiProgressBarSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiProgressBar *> (c)->hwnd, parent);
}

static void
uiProgressBarMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiProgressBarLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiProgressBar *> (c)->hwnd, r);
}

static void
uiProgressBarAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiProgressBar *> (c)->hwnd, controlID, insertAfter);
}

static void
uiProgressBarChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiProgressBarMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto *p = reinterpret_cast<uiProgressBar *> (c);

  uiWindowsSizing sizing;
  uiWindowsGetSizing (p->hwnd, &sizing);

  int x = pbarWidth;
  int y = pbarHeight;
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x;
  *height = y;
}

int
uiProgressBarValue (const uiProgressBar *p)
{
  if ((getStyle (p->hwnd) & PBS_MARQUEE) != 0)
    return -1;

  return SendMessage (p->hwnd, PBM_GETPOS, 0, 0);
}

void
uiProgressBarSetValue (const uiProgressBar *p, const int n)
{
  if (n == -1)
    {
      if ((getStyle (p->hwnd) & PBS_MARQUEE) != 0)
        return;

      setStyle (p->hwnd, getStyle (p->hwnd) | PBS_MARQUEE);
      SendMessageW (p->hwnd, PBM_SETMARQUEE, TRUE, 0);
    }

  if ((getStyle (p->hwnd) & PBS_MARQUEE) != 0)
    {
      SendMessageW (p->hwnd, PBM_SETMARQUEE, FALSE, 0);
      setStyle (p->hwnd, getStyle (p->hwnd) & ~PBS_MARQUEE);
    }

  if (n < 0 || n > 100)
    uiprivUserBug ("Value %d is out of range for uiProgressBars.", n);

  if (n == 100)
    {
      SendMessageW (p->hwnd, PBM_SETRANGE32, 0, 101);
      SendMessageW (p->hwnd, PBM_SETPOS, 101, 0);
      SendMessageW (p->hwnd, PBM_SETPOS, 100, 0);
      SendMessageW (p->hwnd, PBM_SETRANGE32, 0, 100);
      return;
    }

  SendMessageW (p->hwnd, PBM_SETPOS, static_cast<WPARAM> (n + 1), 0);
  SendMessageW (p->hwnd, PBM_SETPOS, static_cast<WPARAM> (n), 0);
}

uiProgressBar *
uiNewProgressBar ()
{
  auto *p = reinterpret_cast<uiProgressBar *> (
      uiWindowsAllocControl (sizeof (uiProgressBar), uiProgressBarSignature, "uiProgressBar"));

  auto *control      = reinterpret_cast<uiControl *> (p);
  control->Destroy   = uiProgressBarDestroy;
  control->Disable   = uiProgressBarDisable;
  control->Enable    = uiProgressBarEnable;
  control->Enabled   = uiProgressBarEnabled;
  control->Handle    = uiProgressBarHandle;
  control->Hide      = uiProgressBarHide;
  control->Parent    = uiProgressBarParent;
  control->SetParent = uiProgressBarSetParent;
  control->Show      = uiProgressBarShow;
  control->Toplevel  = uiProgressBarToplevel;
  control->Visible   = uiProgressBarVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (p);
  windows_control->AssignControlIDZOrder  = uiProgressBarAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiProgressBarChildVisibilityChanged;
  windows_control->LayoutRect             = uiProgressBarLayoutRect;
  windows_control->MinimumSize            = uiProgressBarMinimumSize;
  windows_control->MinimumSizeChanged     = uiProgressBarMinimumSizeChanged;
  windows_control->SetParentHWND          = uiProgressBarSetParentHWND;
  windows_control->SyncEnableState        = uiProgressBarSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  p->hwnd = uiWindowsEnsureCreateControlHWND (0, PROGRESS_CLASSW, L"", PBS_SMOOTH, hInstance, nullptr, FALSE);

  return p;
}
