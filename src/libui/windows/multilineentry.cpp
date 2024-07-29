#include "multilineentry.h"

#include "debug.h"
#include "init.h"
#include "utf16.h"
#include "winpublic.h"
#include "winutil.h"

#include <controlsigs.h>
#include <uipriv.h>
#include <windowsx.h>

static BOOL
onWM_COMMAND (uiControl *c, HWND, const WORD code, LRESULT *lResult)
{
  auto *e = uiMultilineEntry (c);

  if (code != EN_CHANGE)
    return FALSE;

  if (e->inhibitChanged != 0)
    return FALSE;

  (*e->onChanged) (e, e->onChangedData);

  *lResult = 0;

  return TRUE;
}

static void
uiMultilineEntryDestroy (uiControl *c)
{
  auto *e = uiMultilineEntry (c);

  uiWindowsUnregisterWM_COMMANDHandler (e->hwnd);
  uiWindowsEnsureDestroyWindow (e->hwnd);
  uiFreeControl (uiControl (e));
}

static uintptr_t
uiMultilineEntryHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiMultilineEntry *> (c)->hwnd);
}

static uiControl *
uiMultilineEntryParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiMultilineEntrySetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiMultilineEntryToplevel (uiControl *c)
{
  return 0;
}

static int
uiMultilineEntryVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiMultilineEntryShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiMultilineEntry *> (c)->hwnd, SW_SHOW);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiMultilineEntryHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiMultilineEntry *> (c)->hwnd, SW_HIDE);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiMultilineEntryEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiMultilineEntryEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiMultilineEntryDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiMultilineEntrySyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;
  EnableWindow (reinterpret_cast<uiMultilineEntry *> (c)->hwnd, enabled);
}

static void
uiMultilineEntrySetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiMultilineEntry *> (c)->hwnd, parent);
}

static void
uiMultilineEntryMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiMultilineEntryLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiMultilineEntry *> (c)->hwnd, r);
}

static void
uiMultilineEntryAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiMultilineEntry *> (c)->hwnd, controlID, insertAfter);
}

static void
uiMultilineEntryChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiMultilineEntryMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto *e = reinterpret_cast<uiMultilineEntry *> (c);

  uiWindowsSizing sizing;
  uiWindowsGetSizing (e->hwnd, &sizing);

  int x = entryWidth;
  int y = entryHeight;
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x;
  *height = y;
}

static void
defaultOnChanged (uiMultilineEntry *, void *)
{
  // do nothing
}

char *
uiMultilineEntryText (const uiMultilineEntry *e)
{
  char *out = uiWindowsWindowText (e->hwnd);

  CRLFtoLF (out);

  return out;
}

void
uiMultilineEntrySetText (uiMultilineEntry *e, const char *text)
{
  e->inhibitChanged = TRUE;

  char *crlf = LFtoCRLF (text);

  uiWindowsSetWindowText (e->hwnd, crlf);

  uiprivFree (crlf);

  e->inhibitChanged = FALSE;
}

void
uiMultilineEntryAppend (uiMultilineEntry *e, const char *text)
{
  DWORD selStart;
  DWORD selEnd;

  e->inhibitChanged = TRUE;

  // Save current selection
  SendMessageW (e->hwnd, EM_GETSEL, reinterpret_cast<WPARAM> (&selStart), reinterpret_cast<LPARAM> (&selEnd));

  // Append by replacing an empty selection at the end of the input
  const LRESULT l = SendMessageW (e->hwnd, WM_GETTEXTLENGTH, 0, 0);
  Edit_SetSel (e->hwnd, l, l);

  char  *crlf  = LFtoCRLF (text);
  WCHAR *wtext = toUTF16 (crlf);
  uiprivFree (crlf);

  Edit_ReplaceSel (e->hwnd, wtext);
  uiprivFree (wtext);

  // Restore selection
  Edit_SetSel (e->hwnd, selStart, selEnd);

  e->inhibitChanged = FALSE;
}

void
uiMultilineEntryOnChanged (uiMultilineEntry *e, void (*f) (uiMultilineEntry *, void *), void *data)
{
  e->onChanged     = f;
  e->onChangedData = data;
}

int
uiMultilineEntryReadOnly (const uiMultilineEntry *e)
{
  return (getStyle (e->hwnd) & ES_READONLY) != 0; // NOLINT(*-implicit-bool-conversion)
}

void
uiMultilineEntrySetReadOnly (const uiMultilineEntry *e, const int readonly)
{
  if (Edit_SetReadOnly (e->hwnd, readonly) == 0)
    (void)logLastError (L"error setting uiMultilineEntry read-only state");
}

static uiMultilineEntry *
finishMultilineEntry (const DWORD style)
{
  uiMultilineEntry *e;

  uiWindowsNewControl (uiMultilineEntry, e);

  static constexpr auto flags
      = ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_WANTRETURN | WS_TABSTOP | WS_VSCROLL;

  e->hwnd = uiWindowsEnsureCreateControlHWND (WS_EX_CLIENTEDGE, L"edit", L"", flags | style, hInstance, nullptr, TRUE);

  uiWindowsRegisterWM_COMMANDHandler (e->hwnd, onWM_COMMAND, uiControl (e));
  uiMultilineEntryOnChanged (e, defaultOnChanged, nullptr);

  return e;
}

uiMultilineEntry *
uiNewMultilineEntry ()
{
  return finishMultilineEntry (0);
}

uiMultilineEntry *
uiNewNonWrappingMultilineEntry ()
{
  return finishMultilineEntry (WS_HSCROLL | ES_AUTOHSCROLL);
}
