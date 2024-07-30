#include <windows.h>

#include "checkbox.h"
#include "controlsigs.h"
#include "init.h"
#include "utf16.h"
#include "winpublic.h"

#include <ui/checkbox.h>
#include <uipriv.h>

static BOOL
onWM_COMMAND (uiControl *cc, HWND, const WORD code, LRESULT *lResult)
{
  auto *c = reinterpret_cast<uiCheckbox *> (cc);

  if (code != BN_CLICKED)
    return FALSE;

  WPARAM check = BST_CHECKED;
  if (SendMessage (c->hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED)
    check = BST_UNCHECKED;

  SendMessage (c->hwnd, BM_SETCHECK, check, 0);

  (*c->onToggled) (c, c->onToggledData);
  *lResult = 0;
  return TRUE;
}

static void
uiCheckboxDestroy (uiControl *cc)
{
  auto *c = reinterpret_cast<uiCheckbox *> (cc);

  uiWindowsUnregisterWM_COMMANDHandler (c->hwnd);
  uiWindowsEnsureDestroyWindow (c->hwnd);
  uiFreeControl (reinterpret_cast<uiControl *> (c));
}

static uintptr_t
uiCheckboxHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiCheckbox *> (c)->hwnd);
}

static uiControl *
uiCheckboxParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiCheckboxSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiCheckboxToplevel (uiControl *)
{
  return 0;
}

static int
uiCheckboxVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiCheckboxShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiCheckbox *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiCheckboxHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiCheckbox *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiCheckboxEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiCheckboxEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiCheckboxDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiCheckboxSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;

  EnableWindow (reinterpret_cast<uiCheckbox *> (c)->hwnd, enabled);
}

static void
uiCheckboxSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiCheckbox *> (c)->hwnd, parent);
}

static void
uiCheckboxMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiCheckboxLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiCheckbox *> (c)->hwnd, r);
}

static void
uiCheckboxAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiCheckbox *> (c)->hwnd, controlID, insertAfter);
}

static void
uiCheckboxChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiCheckboxMinimumSize (uiWindowsControl *cc, int *width, int *height)
{
  const auto *c = reinterpret_cast<uiCheckbox *> (cc);

  uiWindowsSizing sizing;

  int x = checkboxXFromLeftOfBoxToLeftOfLabel;
  int y = checkboxHeight;

  uiWindowsGetSizing (c->hwnd, &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x + uiWindowsWindowTextWidth (c->hwnd);
  *height = y;
}

static void
defaultOnToggled (uiCheckbox *, void *)
{
  // do nothing
}

char *
uiCheckboxText (const uiCheckbox *c)
{
  return uiWindowsWindowText (c->hwnd);
}

void
uiCheckboxSetText (uiCheckbox *c, const char *text)
{
  uiWindowsSetWindowText (c->hwnd, text);

  // changing the text might necessitate a change in the checkbox's size
  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (c));
}

void
uiCheckboxOnToggled (uiCheckbox *c, void (*f) (uiCheckbox *, void *), void *data)
{
  c->onToggled     = f;
  c->onToggledData = data;
}

int
uiCheckboxChecked (const uiCheckbox *c)
{
  return SendMessage (c->hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

void
uiCheckboxSetChecked (const uiCheckbox *c, const int checked)
{
  WPARAM check = BST_CHECKED;

  if (checked == 0)
    check = BST_UNCHECKED;

  SendMessage (c->hwnd, BM_SETCHECK, check, 0);
}

uiCheckbox *
uiNewCheckbox (const char *text)
{
  auto *const c = reinterpret_cast<uiCheckbox *> (
      uiWindowsAllocControl (sizeof (uiCheckbox), uiCheckboxSignature, "uiCheckbox"));

  auto *control      = reinterpret_cast<uiControl *> (c);
  control->Destroy   = uiCheckboxDestroy;
  control->Disable   = uiCheckboxDisable;
  control->Enable    = uiCheckboxEnable;
  control->Enabled   = uiCheckboxEnabled;
  control->Handle    = uiCheckboxHandle;
  control->Hide      = uiCheckboxHide;
  control->Parent    = uiCheckboxParent;
  control->SetParent = uiCheckboxSetParent;
  control->Show      = uiCheckboxShow;
  control->Toplevel  = uiCheckboxToplevel;
  control->Visible   = uiCheckboxVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (c);
  windows_control->AssignControlIDZOrder  = uiCheckboxAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiCheckboxChildVisibilityChanged;
  windows_control->LayoutRect             = uiCheckboxLayoutRect;
  windows_control->MinimumSize            = uiCheckboxMinimumSize;
  windows_control->MinimumSizeChanged     = uiCheckboxMinimumSizeChanged;
  windows_control->SetParentHWND          = uiCheckboxSetParentHWND;
  windows_control->SyncEnableState        = uiCheckboxSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  WCHAR *wtext = toUTF16 (text);
  c->hwnd = uiWindowsEnsureCreateControlHWND (0, L"button", wtext, BS_CHECKBOX | WS_TABSTOP, hInstance, nullptr, TRUE);
  uiprivFree (wtext);

  uiWindowsRegisterWM_COMMANDHandler (c->hwnd, onWM_COMMAND, uiControl (c));
  uiCheckboxOnToggled (c, defaultOnToggled, nullptr);

  return c;
}
