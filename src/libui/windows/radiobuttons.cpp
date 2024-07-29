#include "radiobuttons.h"

#include "init.h"
#include "utf16.h"
#include "winpublic.h"

#include <ui/radio_buttons.h>
#include <windowsx.h>

#include <algorithm>
#include <controlsigs.h>
#include <uipriv.h>

static BOOL
onWM_COMMAND (uiControl *c, const HWND clicked, const WORD code, LRESULT *lResult)
{
  auto *r = reinterpret_cast<uiRadioButtons *> (c);

  if (code != BN_CLICKED)
    return FALSE;

  *lResult = 0;
  for (const HWND &hwnd : *r->hwnds)
    {
      WPARAM check = BST_UNCHECKED;

      if (clicked == hwnd)
        {
          if (Button_GetCheck (hwnd) == BST_CHECKED)
            return TRUE;
          check = BST_CHECKED;
        }

      SendMessage (hwnd, BM_SETCHECK, check, 0);
    }

  (*r->onSelected) (r, r->onSelectedData);

  return TRUE;
}

static void
defaultOnSelected (uiRadioButtons *, void *)
{
  // do nothing
}

static void
uiRadioButtonsDestroy (uiControl *c)
{
  auto *r = reinterpret_cast<uiRadioButtons *> (c);

  for (const HWND &hwnd : *r->hwnds)
    {
      uiWindowsUnregisterWM_COMMANDHandler (hwnd);
      uiWindowsEnsureDestroyWindow (hwnd);
    }

  delete r->hwnds;

  uiWindowsEnsureDestroyWindow (r->hwnd);

  uiFreeControl (reinterpret_cast<uiControl *> (r));
}

static uintptr_t
uiRadioButtonsHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiRadioButtons *> (c)->hwnd);
}

static uiControl *
uiRadioButtonsParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiRadioButtonsSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiRadioButtonsToplevel (uiControl *)
{
  return 0;
}

static int
uiRadioButtonsVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiRadioButtonsShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiRadioButtons *> (c)->hwnd, SW_SHOW);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiRadioButtonsHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiRadioButtons *> (c)->hwnd, SW_HIDE);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiRadioButtonsEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiRadioButtonsEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiRadioButtonsDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiRadioButtonsSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;
  EnableWindow (reinterpret_cast<uiRadioButtons *> (c)->hwnd, enabled);
}

static void
uiRadioButtonsSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiRadioButtons *> (c)->hwnd, parent);
}

static void
uiRadioButtonsMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiRadioButtonsLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiRadioButtons *> (c)->hwnd, r);
}

static void
uiRadioButtonsAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiRadioButtons *> (c)->hwnd, controlID, insertAfter);
}

static void
uiRadioButtonsChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiRadioButtonsMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto *r = reinterpret_cast<uiRadioButtons *> (c);
  if (r->hwnds->empty ())
    {
      *width  = 0;
      *height = 0;
      return;
    }

  int maxwid = 0;
  for (const HWND &hwnd : *r->hwnds)
    {
      const int wid = uiWindowsWindowTextWidth (hwnd);
      maxwid        = std::max (maxwid, wid);
    }

  uiWindowsSizing sizing;
  uiWindowsGetSizing ((*r->hwnds)[0], &sizing);

  int x = radiobuttonXFromLeftOfBoxToLeftOfLabel;
  int y = radiobuttonHeight;
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x + maxwid;
  *height = y * r->hwnds->size ();
}

static void
radiobuttonsRelayout (const uiRadioButtons *r)
{
  RECT            client;
  int             height1;
  uiWindowsSizing sizing;

  if (r->hwnds->size () == 0)
    return;
  uiWindowsEnsureGetClientRect (r->hwnd, &client);
  const int x     = client.left;
  int       y     = client.top;
  const int width = client.right - client.left;
  height1         = radiobuttonHeight;
  uiWindowsGetSizing ((*r->hwnds)[0], &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, nullptr, &height1);
  const int height = height1;
  for (const HWND &hwnd : *r->hwnds)
    {
      uiWindowsEnsureMoveWindowDuringResize (hwnd, x, y, width, height);
      y += height;
    }
}

static void
radiobuttonsArrangeChildren (const uiRadioButtons *r)
{
  LONG_PTR controlID;
  HWND     insertAfter;

  controlID   = 100;
  insertAfter = nullptr;
  for (const HWND &hwnd : *r->hwnds)
    uiWindowsEnsureAssignControlIDZOrder (hwnd, &controlID, &insertAfter);
}

void
uiRadioButtonsAppend (uiRadioButtons *r, const char *text)
{
  DWORD groupTabStop = 0;
  if (r->hwnds->size () == 0)
    groupTabStop = WS_GROUP | WS_TABSTOP;

  WCHAR     *wtext = toUTF16 (text);
  const HWND hwnd  = uiWindowsEnsureCreateControlHWND (0, L"button", wtext, BS_RADIOBUTTON | groupTabStop, hInstance,
                                                       nullptr, TRUE);
  uiprivFree (wtext);
  uiWindowsEnsureSetParentHWND (hwnd, r->hwnd);
  uiWindowsRegisterWM_COMMANDHandler (hwnd, onWM_COMMAND, reinterpret_cast<uiControl *> (r));

  r->hwnds->push_back (hwnd);
  radiobuttonsArrangeChildren (r);
  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (r));
}

int
uiRadioButtonsSelected (const uiRadioButtons *r)
{

  for (size_t i = 0; i < r->hwnds->size (); i++)
    if (SendMessage ((*r->hwnds)[i], BM_GETCHECK, 0, 0) == BST_CHECKED)
      return i;

  return -1;
}

void
uiRadioButtonsSetSelected (const uiRadioButtons *r, const int index)
{
  const int m = uiRadioButtonsSelected (r);
  if (m != -1)
    SendMessage ((*r->hwnds)[m], BM_SETCHECK, BST_UNCHECKED, 0);

  if (index != -1)
    SendMessage ((*r->hwnds)[index], BM_SETCHECK, BST_CHECKED, 0);
}

void
uiRadioButtonsOnSelected (uiRadioButtons *r, void (*f) (uiRadioButtons *, void *), void *data)
{
  r->onSelected     = f;
  r->onSelectedData = data;
}

static void
onResize (uiWindowsControl *c)
{
  radiobuttonsRelayout (reinterpret_cast<uiRadioButtons *> (c));
}

uiRadioButtons *
uiNewRadioButtons ()
{
  const auto r = reinterpret_cast<uiRadioButtons *> (
      uiWindowsAllocControl (sizeof (uiRadioButtons), uiRadioButtonsSignature, "uiRadioButtons"));

  auto *control      = reinterpret_cast<uiControl *> (r);
  control->Destroy   = uiRadioButtonsDestroy;
  control->Disable   = uiRadioButtonsDisable;
  control->Enable    = uiRadioButtonsEnable;
  control->Enabled   = uiRadioButtonsEnabled;
  control->Handle    = uiRadioButtonsHandle;
  control->Hide      = uiRadioButtonsHide;
  control->Parent    = uiRadioButtonsParent;
  control->SetParent = uiRadioButtonsSetParent;
  control->Show      = uiRadioButtonsShow;
  control->Toplevel  = uiRadioButtonsToplevel;
  control->Visible   = uiRadioButtonsVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (r);
  windows_control->AssignControlIDZOrder  = uiRadioButtonsAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiRadioButtonsChildVisibilityChanged;
  windows_control->LayoutRect             = uiRadioButtonsLayoutRect;
  windows_control->MinimumSize            = uiRadioButtonsMinimumSize;
  windows_control->MinimumSizeChanged     = uiRadioButtonsMinimumSizeChanged;
  windows_control->SetParentHWND          = uiRadioButtonsSetParentHWND;
  windows_control->SyncEnableState        = uiRadioButtonsSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  r->hwnd = uiWindowsMakeContainer (reinterpret_cast<uiWindowsControl *> (r), onResize);

  r->hwnds = new std::vector<HWND>;

  uiRadioButtonsOnSelected (r, defaultOnSelected, nullptr);

  return r;
}
