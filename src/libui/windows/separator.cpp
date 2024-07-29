#include "separator.h"

#include "init.h"
#include "winpublic.h"

#include <controlsigs.h>
#include <ui/separator.h>

static void
uiSeparatorDestroy (uiControl *c)
{
  uiWindowsEnsureDestroyWindow (reinterpret_cast<uiSeparator *> (c)->hwnd);
  uiFreeControl (c);
}

static uintptr_t
uiSeparatorHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiSeparator *> (c)->hwnd);
}

static uiControl *
uiSeparatorParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiSeparatorSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiSeparatorToplevel (uiControl *)
{
  return 0;
}

static int
uiSeparatorVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiSeparatorShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiSeparator *> (c)->hwnd, SW_SHOW);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiSeparatorHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiSeparator *> (c)->hwnd, SW_HIDE);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiSeparatorEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiSeparatorEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiSeparatorDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiSeparatorSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled))
    return;
  EnableWindow (reinterpret_cast<uiSeparator *> (c)->hwnd, enabled);
}

static void
uiSeparatorSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiSeparator *> (c)->hwnd, parent);
}

static void
uiSeparatorMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c))
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiSeparatorLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiSeparator *> (c)->hwnd, r);
}

static void
uiSeparatorAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiSeparator *> (c)->hwnd, controlID, insertAfter);
}

static void
uiSeparatorChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiSeparatorMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto s = reinterpret_cast<uiSeparator *> (c);

  *width  = 1;
  *height = 1;

  uiWindowsSizing sizing;
  uiWindowsGetSizing (s->hwnd, &sizing);

  int x = separatorWidth;
  int y = separatorHeight;
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  if (s->vertical)
    *width = x;

  else
    *height = y;
}

uiSeparator *
uiNewHorizontalSeparator ()
{
  const auto s = reinterpret_cast<uiSeparator *> (
      uiWindowsAllocControl (sizeof (uiSeparator), uiSeparatorSignature, "uiSeparator"));

  auto *control      = reinterpret_cast<uiControl *> (s);
  control->Destroy   = uiSeparatorDestroy;
  control->Disable   = uiSeparatorDisable;
  control->Enable    = uiSeparatorEnable;
  control->Enabled   = uiSeparatorEnabled;
  control->Handle    = uiSeparatorHandle;
  control->Hide      = uiSeparatorHide;
  control->Parent    = uiSeparatorParent;
  control->SetParent = uiSeparatorSetParent;
  control->Show      = uiSeparatorShow;
  control->Toplevel  = uiSeparatorToplevel;
  control->Visible   = uiSeparatorVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (s);
  windows_control->AssignControlIDZOrder  = uiSeparatorAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiSeparatorChildVisibilityChanged;
  windows_control->LayoutRect             = uiSeparatorLayoutRect;
  windows_control->MinimumSize            = uiSeparatorMinimumSize;
  windows_control->MinimumSizeChanged     = uiSeparatorMinimumSizeChanged;
  windows_control->SetParentHWND          = uiSeparatorSetParentHWND;
  windows_control->SyncEnableState        = uiSeparatorSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  s->hwnd = uiWindowsEnsureCreateControlHWND (0, L"static", L"", SS_ETCHEDHORZ, hInstance, nullptr, TRUE);

  return s;
}

uiSeparator *
uiNewVerticalSeparator ()
{
  uiSeparator *s;

  uiWindowsNewControl (uiSeparator, s);

  s->hwnd     = uiWindowsEnsureCreateControlHWND (0, L"static", L"", SS_ETCHEDVERT, hInstance, nullptr, TRUE);
  s->vertical = TRUE;

  return s;
}
