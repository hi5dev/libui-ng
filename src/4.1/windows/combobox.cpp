#include <controlsigs.h>
#include <uipriv.h>

#include <ui/combobox.h>

#include "combobox.h"
#include "debug.h"
#include "init.h"
#include "utf16.h"
#include "winpublic.h"

static BOOL
onWM_COMMAND (uiControl *cc, HWND, const WORD code, LRESULT *lResult)
{
  auto *c = reinterpret_cast<uiCombobox *> (cc);

  if (code != CBN_SELCHANGE)
    return FALSE;

  (*c->onSelected) (c, c->onSelectedData);

  *lResult = 0;

  return TRUE;
}

void
uiComboboxDestroy (uiControl *cc)
{
  auto *c = reinterpret_cast<uiCombobox *> (cc);

  uiWindowsUnregisterWM_COMMANDHandler (c->hwnd);
  uiWindowsEnsureDestroyWindow (c->hwnd);
  uiFreeControl (uiControl (c));
}

static uintptr_t
uiComboboxHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiCombobox *> (c)->hwnd);
}

static uiControl *
uiComboboxParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiComboboxSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiComboboxToplevel (uiControl *)
{
  return 0;
}

static int
uiComboboxVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiComboboxShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiCombobox *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiComboboxHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiCombobox *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiComboboxEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiComboboxEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiComboboxDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiComboboxSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;

  EnableWindow (reinterpret_cast<uiCombobox *> (c)->hwnd, enabled);
}

static void
uiComboboxSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiCombobox *> (c)->hwnd, parent);
}

static void
uiComboboxMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiComboboxLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiCombobox *> (c)->hwnd, r);
}

static void
uiComboboxAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiCombobox *> (c)->hwnd, controlID, insertAfter);
}

static void
uiComboboxChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiComboboxMinimumSize (uiWindowsControl *cc, int *width, int *height)
{
  const auto *c = reinterpret_cast<uiCombobox *> (cc);

  uiWindowsSizing sizing;

  int x = comboboxWidth;
  int y = comboboxHeight;

  uiWindowsGetSizing (c->hwnd, &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x;
  *height = y;
}

static void
defaultOnSelected (uiCombobox *, void *)
{
  // do nothing
}

void
uiComboboxAppend (const uiCombobox *c, const char *text)
{
  WCHAR *wtext = toUTF16 (text);

  const LRESULT res = SendMessageW (c->hwnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (wtext));

  if (res == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error appending item to uiCombobox");

  else if (res == static_cast<LRESULT> (CB_ERRSPACE))
    (void)logLastError (L"memory exhausted appending item to uiCombobox");

  uiprivFree (wtext);
}

void
uiComboboxInsertAt (const uiCombobox *c, const int index, const char *text)
{
  WCHAR *wtext = toUTF16 (text);

  const LRESULT res
      = SendMessageW (c->hwnd, CB_INSERTSTRING, static_cast<WPARAM> (index), reinterpret_cast<LPARAM> (wtext));

  if (res == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error inserting item to uiCombobox");

  else if (res == static_cast<LRESULT> (CB_ERRSPACE))
    (void)logLastError (L"memory exhausted inserting item to uiCombobox");

  uiprivFree (wtext);
}

void
uiComboboxDelete (const uiCombobox *c, const int index)
{
  const LRESULT res = SendMessage (c->hwnd, CB_DELETESTRING, static_cast<WPARAM> (index), 0);

  if (res == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error removing item from uiCombobox");
}

void
uiComboboxClear (const uiCombobox *c)
{
  const LRESULT res = SendMessage (c->hwnd, CB_RESETCONTENT, 0, 0);

  if (res == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error clearing items from uiCombobox");
}

int
uiComboboxNumItems (const uiCombobox *c)
{
  const LRESULT n = SendMessage (c->hwnd, CB_GETCOUNT, 0, 0);

  if (n == static_cast<LRESULT> (CB_ERR))
    return -1;

  return n; // NOLINT(*-narrowing-conversions)
}

int
uiComboboxSelected (const uiCombobox *c)
{
  const LRESULT n = SendMessage (c->hwnd, CB_GETCURSEL, 0, 0);

  if (n == static_cast<LRESULT> (CB_ERR))
    return -1;

  return n; // NOLINT(*-narrowing-conversions)
}

void
uiComboboxSetSelected (const uiCombobox *c, const int index)
{
  (void)SendMessageW (c->hwnd, CB_SETCURSEL, static_cast<WPARAM> (index), 0);
}

void
uiComboboxOnSelected (uiCombobox *c, void (*f) (uiCombobox *c, void *data), void *data)
{
  c->onSelected     = f;
  c->onSelectedData = data;
}

uiCombobox *
uiNewCombobox ()
{
  uiCombobox *c;

  uiWindowsNewControl (uiCombobox, c);

  c->hwnd = uiWindowsEnsureCreateControlHWND (WS_EX_CLIENTEDGE, L"combobox", L"", CBS_DROPDOWNLIST | WS_TABSTOP,
                                              hInstance, nullptr, TRUE);

  uiWindowsRegisterWM_COMMANDHandler (c->hwnd, onWM_COMMAND, uiControl (c));

  uiComboboxOnSelected (c, defaultOnSelected, nullptr);

  return c;
}
