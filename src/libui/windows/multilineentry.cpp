#include "multilineentry.h"

#include "debug.h"
#include "init.h"
#include "utf16.h"
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

uiWindowsControlAllDefaultsExceptDestroy (uiMultilineEntry)

#define entryWidth  107
#define entryHeight 14

static void
uiMultilineEntryMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto *e = uiMultilineEntry (c);

  uiWindowsSizing sizing;

  int x = entryWidth;
  int y = entryHeight;

  uiWindowsGetSizing (e->hwnd, &sizing);
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
  // doing this raises an EN_CHANGED
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

  // doing this raises an EN_CHANGED
  e->inhibitChanged = TRUE;

  // Save current selection
  SendMessageW (e->hwnd, EM_GETSEL, (WPARAM)&selStart, (LPARAM)&selEnd);

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
uiMultilineEntrySetReadOnly (uiMultilineEntry *e, int readonly)
{
  if (Edit_SetReadOnly (e->hwnd, readonly) == 0)
    (void)logLastError (L"error setting uiMultilineEntry read-only state");
}

static uiMultilineEntry *
finishMultilineEntry (const DWORD style)
{
  uiMultilineEntry *e = NULL;

  uiWindowsNewControl (uiMultilineEntry, e);

  static constexpr auto flags
      = ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_WANTRETURN | WS_TABSTOP | WS_VSCROLL;

  e->hwnd = uiWindowsEnsureCreateControlHWND (WS_EX_CLIENTEDGE, L"edit", L"", flags | style, hInstance, NULL, TRUE);

  uiWindowsRegisterWM_COMMANDHandler (e->hwnd, onWM_COMMAND, uiControl (e));
  uiMultilineEntryOnChanged (e, defaultOnChanged, NULL);

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
