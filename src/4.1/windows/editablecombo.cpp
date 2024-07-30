#include <ui/editable_combobox.h>

#include "debug.h"
#include "editablecombo.h"

#include "init.h"
#include "utf16.h"
#include "winpublic.h"
#include "winutil.h"

#include <controlsigs.h>
#include <uipriv.h>

static BOOL
onWM_COMMAND (uiControl *cc, HWND hwnd, const WORD code, LRESULT *lResult)
{
  auto *c = reinterpret_cast<uiEditableCombobox *> (cc);

  if (code == CBN_SELCHANGE)
    {
      const auto result
          = PostMessage (parentOf (hwnd), WM_COMMAND, MAKEWPARAM (GetWindowLongPtrW (hwnd, GWLP_ID), CBN_EDITCHANGE),
                         reinterpret_cast<LPARAM> (hwnd));

      if (result == 0)
        (void)logLastError (L"error posting CBN_EDITCHANGE after CBN_SELCHANGE");

      *lResult = 0;
      return TRUE;
    }
  if (code != CBN_EDITCHANGE)
    return FALSE;

  (*c->onChanged) (c, c->onChangedData);
  *lResult = 0;

  return TRUE;
}

void
uiEditableComboboxDestroy (uiControl *cc)
{
  auto *c = reinterpret_cast<uiEditableCombobox *> (cc);

  uiWindowsUnregisterWM_COMMANDHandler (c->hwnd);
  uiWindowsEnsureDestroyWindow (c->hwnd);
  uiFreeControl (uiControl (c));
}

static uintptr_t
uiEditableComboboxHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiEditableCombobox *> (c)->hwnd);
}

static uiControl *
uiEditableComboboxParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiEditableComboboxSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiEditableComboboxToplevel (uiControl *)
{
  return 0;
}

static int
uiEditableComboboxVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiEditableComboboxShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiEditableCombobox *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiEditableComboboxHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiEditableCombobox *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiEditableComboboxEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiEditableComboboxEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiEditableComboboxDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiEditableComboboxSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;

  EnableWindow (reinterpret_cast<uiEditableCombobox *> (c)->hwnd, enabled);
}

static void
uiEditableComboboxSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiEditableCombobox *> (c)->hwnd, parent);
}

static void
uiEditableComboboxMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiEditableComboboxLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiEditableCombobox *> (c)->hwnd, r);
}

static void
uiEditableComboboxAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiEditableCombobox *> (c)->hwnd, controlID, insertAfter);
}

static void
uiEditableComboboxChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiEditableComboboxMinimumSize (uiWindowsControl *cc, int *width, int *height)
{
  const auto *c = reinterpret_cast<uiEditableCombobox *> (cc);

  uiWindowsSizing sizing;

  int x = comboboxWidth;
  int y = comboboxHeight;

  uiWindowsGetSizing (c->hwnd, &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x;
  *height = y;
}

static void
defaultOnChanged (uiEditableCombobox *, void *)
{
}

void
uiEditableComboboxAppend (const uiEditableCombobox *c, const char *text)
{
  WCHAR *wtext = toUTF16 (text);

  const LRESULT res = SendMessageW (c->hwnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (wtext));

  if (res == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error appending item to uiEditableCombobox");

  else if (res == static_cast<LRESULT> (CB_ERRSPACE))
    (void)logLastError (L"memory exhausted appending item to uiEditableCombobox");

  uiprivFree (wtext);
}

char *
uiEditableComboboxText (const uiEditableCombobox *c)
{
  return uiWindowsWindowText (c->hwnd);
}

void
uiEditableComboboxSetText (const uiEditableCombobox *c, const char *text)
{
  uiWindowsSetWindowText (c->hwnd, text);
}

void
uiEditableComboboxOnChanged (uiEditableCombobox *c, void (*f) (uiEditableCombobox *c, void *data), void *data)
{
  c->onChanged     = f;
  c->onChangedData = data;
}

uiEditableCombobox *
uiNewEditableCombobox ()
{
  auto *c = reinterpret_cast<uiEditableCombobox *> (
      uiWindowsAllocControl (sizeof (uiEditableCombobox), uiEditableComboboxSignature, "uiEditableCombobox"));

  auto *control      = reinterpret_cast<uiControl *> (c);
  control->Destroy   = uiEditableComboboxDestroy;
  control->Disable   = uiEditableComboboxDisable;
  control->Enable    = uiEditableComboboxEnable;
  control->Enabled   = uiEditableComboboxEnabled;
  control->Handle    = uiEditableComboboxHandle;
  control->Hide      = uiEditableComboboxHide;
  control->Parent    = uiEditableComboboxParent;
  control->SetParent = uiEditableComboboxSetParent;
  control->Show      = uiEditableComboboxShow;
  control->Toplevel  = uiEditableComboboxToplevel;
  control->Visible   = uiEditableComboboxVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (c);
  windows_control->AssignControlIDZOrder  = uiEditableComboboxAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiEditableComboboxChildVisibilityChanged;
  windows_control->LayoutRect             = uiEditableComboboxLayoutRect;
  windows_control->MinimumSize            = uiEditableComboboxMinimumSize;
  windows_control->MinimumSizeChanged     = uiEditableComboboxMinimumSizeChanged;
  windows_control->SetParentHWND          = uiEditableComboboxSetParentHWND;
  windows_control->SyncEnableState        = uiEditableComboboxSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  c->hwnd = uiWindowsEnsureCreateControlHWND (WS_EX_CLIENTEDGE, L"combobox", L"", CBS_DROPDOWN | WS_TABSTOP, hInstance,
                                              nullptr, TRUE);

  uiWindowsRegisterWM_COMMANDHandler (c->hwnd, onWM_COMMAND, uiControl (c));

  uiEditableComboboxOnChanged (c, defaultOnChanged, nullptr);

  return c;
}
