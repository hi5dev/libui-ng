#include <windows.h>

#include <commctrl.h>

#include "button.h"

#include "init.h"
#include "utf16.h"
#include "winpublic.h"

#include <controlsigs.h>
#include <ui/button.h>
#include <uipriv.h>

static BOOL
onWM_COMMAND (uiControl *c, HWND, const WORD code, LRESULT *lResult)
{
  auto *b = uiButton (c);

  if (code != BN_CLICKED)
    return FALSE;

  (*b->onClicked) (b, b->onClickedData);

  *lResult = 0;

  return TRUE;
}

static void
uiButtonDestroy (uiControl *c)
{
  auto *b = uiButton (c);

  uiWindowsUnregisterWM_COMMANDHandler (b->hwnd);
  uiWindowsEnsureDestroyWindow (b->hwnd);
  uiFreeControl (uiControl (b));
}

static uintptr_t
uiButtonHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiButton *> (c)->hwnd);
}

static uiControl *
uiButtonParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiButtonSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiButtonToplevel (uiControl *)
{
  return 0;
}

static int
uiButtonVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiButtonShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiButton *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiButtonHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiButton *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiButtonEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiButtonEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiButtonDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiButtonSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;
  EnableWindow (reinterpret_cast<uiButton *> (c)->hwnd, enabled);
}

static void
uiButtonSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiButton *> (c)->hwnd, parent);
}

static void
uiButtonMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiButtonLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiButton *> (c)->hwnd, r);
}

static void
uiButtonAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiButton *> (c)->hwnd, controlID, insertAfter);
}

static void
uiButtonChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiButtonMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto     *b = reinterpret_cast<uiButton *> (c);
  SIZE            size;
  uiWindowsSizing sizing;
  int             y;

  // try the comctl32 version 6 way
  size.cx = 0; // explicitly ask for ideal size
  size.cy = 0;
  if (SendMessageW (b->hwnd, BCM_GETIDEALSIZE, 0, reinterpret_cast<LPARAM> (&size)) != FALSE)
    {
      *width  = size.cx;
      *height = size.cy;
      return;
    }

  // that didn't work; fall back to using Microsoft's metrics
  // Microsoft says to use a fixed width for all buttons; this isn't good enough
  // use the text width instead, with some edge padding
  *width = uiWindowsWindowTextWidth (b->hwnd) + (2 * GetSystemMetrics (SM_CXEDGE));
  y      = buttonHeight;
  uiWindowsGetSizing (b->hwnd, &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, nullptr, &y);
  *height = y;
}

static void
defaultOnClicked (uiButton *b, void *data)
{
  // do nothing
}

char *
uiButtonText (const uiButton *b)
{
  return uiWindowsWindowText (b->hwnd);
}

void
uiButtonSetText (uiButton *b, const char *text)
{
  uiWindowsSetWindowText (b->hwnd, text);
  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (b));
}

void
uiButtonOnClicked (uiButton *b, void (*f) (uiButton *, void *), void *data)
{
  b->onClicked     = f;
  b->onClickedData = data;
}

uiButton *
uiNewButton (const char *text)
{

  auto *b = reinterpret_cast<uiButton *> (uiWindowsAllocControl (sizeof (uiButton), uiButtonSignature, "uiButton"));

  auto *control      = reinterpret_cast<uiControl *> (b);
  control->Destroy   = uiButtonDestroy;
  control->Disable   = uiButtonDisable;
  control->Enable    = uiButtonEnable;
  control->Enabled   = uiButtonEnabled;
  control->Handle    = uiButtonHandle;
  control->Hide      = uiButtonHide;
  control->Parent    = uiButtonParent;
  control->SetParent = uiButtonSetParent;
  control->Show      = uiButtonShow;
  control->Toplevel  = uiButtonToplevel;
  control->Visible   = uiButtonVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (b);
  windows_control->AssignControlIDZOrder  = uiButtonAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiButtonChildVisibilityChanged;
  windows_control->LayoutRect             = uiButtonLayoutRect;
  windows_control->MinimumSize            = uiButtonMinimumSize;
  windows_control->MinimumSizeChanged     = uiButtonMinimumSizeChanged;
  windows_control->SetParentHWND          = uiButtonSetParentHWND;
  windows_control->SyncEnableState        = uiButtonSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  WCHAR *wtext = toUTF16 (text);
  b->hwnd
      = uiWindowsEnsureCreateControlHWND (0, L"button", wtext, BS_PUSHBUTTON | WS_TABSTOP, hInstance, nullptr, TRUE);

  uiprivFree (wtext);

  uiWindowsRegisterWM_COMMANDHandler (b->hwnd, onWM_COMMAND, uiControl (b));
  uiButtonOnClicked (b, defaultOnClicked, nullptr);

  return b;
}
