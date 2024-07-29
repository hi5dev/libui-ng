#include "label.h"

#include "init.h"
#include "utf16.h"
#include "winpublic.h"

#include <controlsigs.h>
#include <ui/label.h>
#include <uipriv.h>

static void
uiLabelDestroy (uiControl *c)
{
  uiWindowsEnsureDestroyWindow (reinterpret_cast<uiLabel *> (c)->hwnd);
  uiFreeControl (c);
}

static uintptr_t
uiLabelHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiLabel *> (c)->hwnd);
}

static uiControl *
uiLabelParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiLabelSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiLabelToplevel (uiControl *)
{
  return 0;
}

static int
uiLabelVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiLabelShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiLabel *> (c)->hwnd, SW_SHOW);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiLabelHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiLabel *> (c)->hwnd, SW_HIDE);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiLabelEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiLabelEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiLabelDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiLabelSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;
  EnableWindow (reinterpret_cast<uiLabel *> (c)->hwnd, enabled);
}

static void
uiLabelSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiLabel *> (c)->hwnd, parent);
}

static void
uiLabelMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiLabelLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiLabel *> (c)->hwnd, r);
}

static void
uiLabelAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiLabel *> (c)->hwnd, controlID, insertAfter);
}

static void
uiLabelChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiLabelMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const uiLabel *l = reinterpret_cast<uiLabel *> (c);

  uiWindowsSizing sizing;

  int y;

  *width = uiWindowsWindowTextWidth (l->hwnd);

  y = uiWindowsWindowTextHeight (l->hwnd);

  uiWindowsGetSizing (l->hwnd, &sizing);

  uiWindowsSizingDlgUnitsToPixels (&sizing, nullptr, &y);

  *height = y;
}

char *
uiLabelText (const uiLabel *l)
{
  return uiWindowsWindowText (l->hwnd);
}

void
uiLabelSetText (uiLabel *l, const char *text)
{
  uiWindowsSetWindowText (l->hwnd, text);
  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (l));
}

uiLabel *
uiNewLabel (const char *text)
{
  auto *l = reinterpret_cast<uiLabel *> (uiWindowsAllocControl (sizeof (uiLabel), uiLabelSignature, "uiLabel"));

  auto *control      = reinterpret_cast<uiControl *> (l);
  control->Destroy   = uiLabelDestroy;
  control->Disable   = uiLabelDisable;
  control->Enable    = uiLabelEnable;
  control->Enabled   = uiLabelEnabled;
  control->Handle    = uiLabelHandle;
  control->Hide      = uiLabelHide;
  control->Parent    = uiLabelParent;
  control->SetParent = uiLabelSetParent;
  control->Show      = uiLabelShow;
  control->Toplevel  = uiLabelToplevel;
  control->Visible   = uiLabelVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (l);
  windows_control->AssignControlIDZOrder  = uiLabelAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiLabelChildVisibilityChanged;
  windows_control->LayoutRect             = uiLabelLayoutRect;
  windows_control->MinimumSize            = uiLabelMinimumSize;
  windows_control->MinimumSizeChanged     = uiLabelMinimumSizeChanged;
  windows_control->SetParentHWND          = uiLabelSetParentHWND;
  windows_control->SyncEnableState        = uiLabelSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  WCHAR *wtext = toUTF16 (text);

  static constexpr auto style = SS_LEFTNOWORDWRAP | SS_NOPREFIX;
  l->hwnd = uiWindowsEnsureCreateControlHWND (0, L"static", wtext, style, hInstance, nullptr, TRUE);

  uiprivFree (wtext);

  return l;
}
