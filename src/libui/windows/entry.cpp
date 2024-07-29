#include "entry.h"

#include "debug.h"
#include "init.h"
#include "winpublic.h"
#include "winutil.h"

#include <controlsigs.h>
#include <ui/entry.h>
#include <uxtheme.h>
#include <windowsx.h>

static BOOL
onWM_COMMAND (uiControl *c, HWND, const WORD code, LRESULT *lResult)
{
  auto *e = reinterpret_cast<uiEntry *> (c);

  if (code != EN_CHANGE)
    return FALSE;

  if (e->inhibitChanged != 0)
    return FALSE;

  (*e->onChanged) (e, e->onChangedData);

  *lResult = 0;

  return TRUE;
}

static void
uiEntryDestroy (uiControl *c)
{
  auto *e = reinterpret_cast<uiEntry *> (c);

  uiWindowsUnregisterWM_COMMANDHandler (e->hwnd);
  uiWindowsEnsureDestroyWindow (e->hwnd);
  uiFreeControl (reinterpret_cast<uiControl *> (e));
}

static uintptr_t
uiEntryHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiEntry *> (c)->hwnd);
}

static uiControl *
uiEntryParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiEntrySetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiEntryToplevel (uiControl *)
{
  return 0;
}

static int
uiEntryVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiEntryShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiEntry *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiEntryHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiEntry *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiEntryEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiEntryEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiEntryDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiEntrySyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;

  EnableWindow (reinterpret_cast<uiEntry *> (c)->hwnd, enabled);
}

static void
uiEntrySetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiEntry *> (c)->hwnd, parent);
}

static void
uiEntryMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiEntryLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiEntry *> (c)->hwnd, r);
}

static void
uiEntryAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiEntry *> (c)->hwnd, controlID, insertAfter);
}

static void
uiEntryChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiEntryMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto *e = reinterpret_cast<uiEntry *> (c);

  uiWindowsSizing sizing;

  int x = entryWidth;
  int y = entryHeight;

  uiWindowsGetSizing (e->hwnd, &sizing);

  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x;
  *height = y;
}

static void
defaultOnChanged (uiEntry *, void *)
{
  // do nothing
}

char *
uiEntryText (const uiEntry *e)
{
  return uiWindowsWindowText (e->hwnd);
}

void
uiEntrySetText (uiEntry *e, const char *text)
{
  e->inhibitChanged = TRUE;
  uiWindowsSetWindowText (e->hwnd, text);

  const int l = static_cast<int> (strlen (text));

  if (GetFocus () == e->hwnd)
    Edit_SetSel (e->hwnd, l, l);

  e->inhibitChanged = FALSE;
}

void
uiEntryOnChanged (uiEntry *e, void (*f) (uiEntry *, void *), void *data)
{
  e->onChanged     = f;
  e->onChangedData = data;
}

int
uiEntryReadOnly (const uiEntry *e)
{
  return (getStyle (e->hwnd) & ES_READONLY) != 0; // NOLINT(*-implicit-bool-conversion)
}

void
uiEntrySetReadOnly (const uiEntry *e, const int readonly)
{
  if (Edit_SetReadOnly (e->hwnd, readonly) == 0)
    (void)logLastError (L"error setting uiEntry read-only state");
}

static uiEntry *
finishNewEntry (const DWORD style)
{
  auto *e = reinterpret_cast<uiEntry *> (uiWindowsAllocControl (sizeof (uiEntry), uiEntrySignature, "uiEntry"));

  auto *control      = reinterpret_cast<uiControl *> (e);
  control->Destroy   = uiEntryDestroy;
  control->Disable   = uiEntryDisable;
  control->Enable    = uiEntryEnable;
  control->Enabled   = uiEntryEnabled;
  control->Handle    = uiEntryHandle;
  control->Hide      = uiEntryHide;
  control->Parent    = uiEntryParent;
  control->SetParent = uiEntrySetParent;
  control->Show      = uiEntryShow;
  control->Toplevel  = uiEntryToplevel;
  control->Visible   = uiEntryVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (e);
  windows_control->AssignControlIDZOrder  = uiEntryAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiEntryChildVisibilityChanged;
  windows_control->LayoutRect             = uiEntryLayoutRect;
  windows_control->MinimumSize            = uiEntryMinimumSize;
  windows_control->MinimumSizeChanged     = uiEntryMinimumSizeChanged;
  windows_control->SetParentHWND          = uiEntrySetParentHWND;
  windows_control->SyncEnableState        = uiEntrySyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  e->hwnd = uiWindowsEnsureCreateControlHWND (WS_EX_CLIENTEDGE, L"edit", L"",
                                              style | ES_AUTOHSCROLL | ES_LEFT | ES_NOHIDESEL | WS_TABSTOP, hInstance,
                                              nullptr, TRUE);

  uiWindowsRegisterWM_COMMANDHandler (e->hwnd, onWM_COMMAND, uiControl (e));

  uiEntryOnChanged (e, defaultOnChanged, nullptr);

  return e;
}

uiEntry *
uiNewEntry ()
{
  return finishNewEntry (0);
}

uiEntry *
uiNewPasswordEntry ()
{
  return finishNewEntry (ES_PASSWORD);
}

uiEntry *
uiNewSearchEntry ()
{
  auto *e = finishNewEntry (0);

  const auto hr = SetWindowTheme (e->hwnd, L"SearchBoxEditComposited", nullptr);
  if (hr != S_OK || IsAppThemed () == 0)
    (void)logHRESULT (L"SetWindowTheme", hr);

  return e;
}
