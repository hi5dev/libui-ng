#include "spinbox.h"
#include "controlsigs.h"
#include "debug.h"
#include "init.h"
#include "text.h"
#include "winpublic.h"

#include <commctrl.h>
#include <uipriv.h>

static int
value (uiSpinbox *s)
{
  BOOL neededCap = FALSE;

  // This verifies the value put in, capping it automatically.
  // We don't need to worry about checking for an error; that flag should really be called "did we have to cap?".
  // We DO need to set the value in case of a cap though.
  const LRESULT val = SendMessageW (s->updown, UDM_GETPOS32, 0, reinterpret_cast<LPARAM> (&neededCap));
  if (neededCap)
    {
      s->inhibitChanged = TRUE;
      SendMessageW (s->updown, UDM_SETPOS32, 0, val);
      s->inhibitChanged = FALSE;
    }

  return val;
}

static BOOL
onWM_COMMAND (uiControl *c, HWND, const WORD code, LRESULT *)
{
  const auto s = reinterpret_cast<uiSpinbox *> (c);

  if (code != EN_CHANGE)
    return FALSE;

  if (s->inhibitChanged)
    return FALSE;

  WCHAR *wtext = windowText (s->edit);
  if (wcscmp (wtext, L"-") == 0)
    {
      uiprivFree (wtext);
      return TRUE;
    }

  uiprivFree (wtext);

  value (s);

  (*s->onChanged) (s, s->onChangedData);

  return TRUE;
}

static void
uiSpinboxDestroy (uiControl *c)
{
  const auto s = reinterpret_cast<uiSpinbox *> (c);

  uiWindowsUnregisterWM_COMMANDHandler (s->edit);
  uiWindowsEnsureDestroyWindow (s->updown);
  uiWindowsEnsureDestroyWindow (s->edit);
  uiWindowsEnsureDestroyWindow (s->hwnd);

  uiFreeControl (reinterpret_cast<uiControl *> (s));
}

static uintptr_t
uiSpinboxHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiSpinbox *> (c)->hwnd);
}

static uiControl *
uiSpinboxParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiSpinboxSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiSpinboxToplevel (uiControl *)
{
  return 0;
}

static int
uiSpinboxVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiSpinboxShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiSpinbox *> (c)->hwnd, SW_SHOW);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiSpinboxHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiSpinbox *> (c)->hwnd, SW_HIDE);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiSpinboxEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiSpinboxEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiSpinboxDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiSpinboxSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled))
    return;
  EnableWindow (reinterpret_cast<uiSpinbox *> (c)->hwnd, enabled);
}

static void
uiSpinboxSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiSpinbox *> (c)->hwnd, parent);
}

static void
uiSpinboxMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c))
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiSpinboxLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiSpinbox *> (c)->hwnd, r);
}

static void
uiSpinboxAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiSpinbox *> (c)->hwnd, controlID, insertAfter);
}

static void
uiSpinboxChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiSpinboxMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto s = reinterpret_cast<uiSpinbox *> (c);

  uiWindowsSizing sizing;
  uiWindowsGetSizing (s->edit, &sizing);

  int x = entryWidth;
  int y = entryHeight;
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x;
  *height = y;
}

static void
spinboxArrangeChildren (const uiSpinbox *s)
{
  LONG_PTR controlID   = 100;
  HWND     insertAfter = nullptr;

  uiWindowsEnsureAssignControlIDZOrder (s->edit, &controlID, &insertAfter);
  uiWindowsEnsureAssignControlIDZOrder (s->updown, &controlID, &insertAfter);
}

static void
recreateUpDown (uiSpinbox *s)
{
  // an up-down control will only properly position itself the first time
  // stupidly, there are no messages to force a size calculation, nor can I seem to reset the buddy window to force a
  // new position alas, we have to make a new up/down control each time :(

  BOOL preserve = FALSE;

  INT min;
  INT max;

  // ReSharper disable once CppDFAUnusedValue
  int current = 0;

  if (s->updown != nullptr)
    {
      preserve = TRUE;
      current  = value (s);
      SendMessageW (s->updown, UDM_GETRANGE32, reinterpret_cast<WPARAM> (&min), reinterpret_cast<LPARAM> (&max));
      uiWindowsEnsureDestroyWindow (s->updown);
    }

  s->inhibitChanged = TRUE;

  static constexpr auto style = //
      WS_CHILD |                //
      UDS_ALIGNRIGHT |          //
      UDS_ARROWKEYS |           //
      UDS_HOTTRACK |            //
      UDS_NOTHOUSANDS |         //
      UDS_SETBUDDYINT |         //
      0;

  s->updown = CreateWindowExW (0, UPDOWN_CLASSW, L"", style, 0, 0, 0, 0, s->hwnd, nullptr, hInstance, nullptr);

  if (s->updown == nullptr)
    (void)logLastError (L"error creating updown");

  SendMessageW (s->updown, UDM_SETBUDDY, reinterpret_cast<WPARAM> (s->edit), 0);

  if (preserve)
    {
      SendMessageW (s->updown, UDM_SETRANGE32, static_cast<WPARAM> (min), max);
      SendMessageW (s->updown, UDM_SETPOS32, 0, current);
    }

  spinboxArrangeChildren (s);
  ShowWindow (s->updown, SW_SHOW);

  s->inhibitChanged = FALSE;
}

static void
spinboxRelayout (uiSpinbox *s)
{
  RECT r;

  uiWindowsEnsureGetClientRect (s->hwnd, &r);

  uiWindowsEnsureMoveWindowDuringResize (s->edit, r.left, r.top, r.right - r.left, r.bottom - r.top);

  recreateUpDown (s);
}

static void
defaultOnChanged (uiSpinbox *, void *)
{
  // do nothing
}

int
uiSpinboxValue (uiSpinbox *s)
{
  return value (s);
}

void
uiSpinboxSetValue (uiSpinbox *s, const int value)
{
  s->inhibitChanged = TRUE;
  SendMessageW (s->updown, UDM_SETPOS32, 0, value);
  s->inhibitChanged = FALSE;
}

void
uiSpinboxOnChanged (uiSpinbox *s, void (*f) (uiSpinbox *, void *), void *data)
{
  s->onChanged     = f;
  s->onChangedData = data;
}

static void
onResize (uiWindowsControl *c)
{
  spinboxRelayout (reinterpret_cast<uiSpinbox *> (c));
}

uiSpinbox *
uiNewSpinbox (int min, int max)
{
  if (min >= max)
    {
      const int temp = min;

      min = max;
      max = temp;
    }

  const auto s
      = reinterpret_cast<uiSpinbox *> (uiWindowsAllocControl (sizeof (uiSpinbox), uiSpinboxSignature, "uiSpinbox"));

  auto *control      = uiControl (s);
  control->Destroy   = uiSpinboxDestroy;
  control->Disable   = uiSpinboxDisable;
  control->Enable    = uiSpinboxEnable;
  control->Enabled   = uiSpinboxEnabled;
  control->Handle    = uiSpinboxHandle;
  control->Hide      = uiSpinboxHide;
  control->Parent    = uiSpinboxParent;
  control->SetParent = uiSpinboxSetParent;
  control->Show      = uiSpinboxShow;
  control->Toplevel  = uiSpinboxToplevel;
  control->Visible   = uiSpinboxVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (s);
  windows_control->AssignControlIDZOrder  = uiSpinboxAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiSpinboxChildVisibilityChanged;
  windows_control->LayoutRect             = uiSpinboxLayoutRect;
  windows_control->MinimumSize            = uiSpinboxMinimumSize;
  windows_control->MinimumSizeChanged     = uiSpinboxMinimumSizeChanged;
  windows_control->SetParentHWND          = uiSpinboxSetParentHWND;
  windows_control->SyncEnableState        = uiSpinboxSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  s->hwnd = uiWindowsMakeContainer (reinterpret_cast<uiWindowsControl *> (s), onResize);

  static constexpr auto style = ES_AUTOHSCROLL | ES_LEFT | ES_NOHIDESEL | WS_TABSTOP;
  s->edit = uiWindowsEnsureCreateControlHWND (WS_EX_CLIENTEDGE, L"edit", L"", style, hInstance, nullptr, TRUE);

  uiWindowsEnsureSetParentHWND (s->edit, s->hwnd);

  uiWindowsRegisterWM_COMMANDHandler (s->edit, onWM_COMMAND, uiControl (s));

  uiSpinboxOnChanged (s, defaultOnChanged, nullptr);

  recreateUpDown (s);

  s->inhibitChanged = TRUE;
  SendMessageW (s->updown, UDM_SETRANGE32, static_cast<WPARAM> (min), max);
  SendMessageW (s->updown, UDM_SETPOS32, 0, min);
  s->inhibitChanged = FALSE;

  return s;
}
