#include "fontbutton.h"

#include "fontmatch.h"
#include "init.h"
#include "text.h"
#include "utf16.h"
#include "winpublic.h"
#include "winutil.h"

#include <commctrl.h>
#include <controlsigs.h>
#include <ui/font_button.h>
#include <uipriv.h>

static void
uiFontButtonDestroy (uiControl *c)
{
  auto *b = reinterpret_cast<uiFontButton *> (c);

  uiWindowsUnregisterWM_COMMANDHandler (b->hwnd);
  uiprivDestroyFontDialogParams (&b->params);
  uiWindowsEnsureDestroyWindow (b->hwnd);
  uiFreeControl (uiControl (b));
}

static void
updateFontButtonLabel (uiFontButton *b)
{
  WCHAR *text = uiprivFontDialogParamsToString (&b->params);

  setWindowText (b->hwnd, text);

  uiprivFree (text);

  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (b));
}

static BOOL
onWM_COMMAND (uiControl *c, HWND hwnd, const WORD code, LRESULT *lResult)
{
  auto *b = reinterpret_cast<uiFontButton *> (c);

  if (code != BN_CLICKED)
    return FALSE;

  const HWND parent = parentToplevel (b->hwnd);
  if (uiprivShowFontDialog (parent, &b->params) != 0)
    {
      updateFontButtonLabel (b);
      (*b->onChanged) (b, b->onChangedData);
    }

  *lResult = 0;
  return TRUE;
}

static uintptr_t
uiFontButtonHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiFontButton *> (c)->hwnd);
}

static uiControl *
uiFontButtonParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiFontButtonSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiFontButtonToplevel (uiControl *)
{
  return 0;
}

static int
uiFontButtonVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiFontButtonShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiFontButton *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiFontButtonHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiFontButton *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiFontButtonEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiFontButtonEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiFontButtonDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiFontButtonSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;

  EnableWindow (reinterpret_cast<uiFontButton *> (c)->hwnd, enabled);
}

static void
uiFontButtonSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiFontButton *> (c)->hwnd, parent);
}

static void
uiFontButtonMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiFontButtonLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiFontButton *> (c)->hwnd, r);
}

static void
uiFontButtonAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiFontButton *> (c)->hwnd, controlID, insertAfter);
}

static void
uiFontButtonChildVisibilityChanged (uiWindowsControl *)
{
}
static void
uiFontButtonMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto *b = reinterpret_cast<uiFontButton *> (c);

  SIZE size;

  uiWindowsSizing sizing;

  int y;

  size.cx = 0;
  size.cy = 0;
  if (SendMessageW (b->hwnd, BCM_GETIDEALSIZE, 0, reinterpret_cast<LPARAM> (&size)) != FALSE)
    {
      *width  = size.cx;
      *height = size.cy;
      return;
    }

  *width = uiWindowsWindowTextWidth (b->hwnd) + 2 * GetSystemMetrics (SM_CXEDGE);

  y = buttonHeight;

  uiWindowsGetSizing (b->hwnd, &sizing);

  uiWindowsSizingDlgUnitsToPixels (&sizing, nullptr, &y);

  *height = y;
}

static void
defaultOnChanged (uiFontButton *, void *)
{
  // do nothing
}

void
uiFontButtonFont (const uiFontButton *b, uiFontDescriptor *desc)
{
  uiprivFontDescriptorFromIDWriteFont (b->params.font, desc);
  desc->Family = toUTF8 (b->params.familyName);
  desc->Size   = b->params.size;
}

void
uiFontButtonOnChanged (uiFontButton *b, void (*f) (uiFontButton *, void *), void *data)
{
  b->onChanged     = f;
  b->onChangedData = data;
}

uiFontButton *
uiNewFontButton ()
{
  auto *const b = reinterpret_cast<uiFontButton *> (
      uiWindowsAllocControl (sizeof (uiFontButton), uiFontButtonSignature, "uiFontButton"));

  auto *control      = reinterpret_cast<uiControl *> (b);
  control->Destroy   = uiFontButtonDestroy;
  control->Disable   = uiFontButtonDisable;
  control->Enable    = uiFontButtonEnable;
  control->Enabled   = uiFontButtonEnabled;
  control->Handle    = uiFontButtonHandle;
  control->Hide      = uiFontButtonHide;
  control->Parent    = uiFontButtonParent;
  control->SetParent = uiFontButtonSetParent;
  control->Show      = uiFontButtonShow;
  control->Toplevel  = uiFontButtonToplevel;
  control->Visible   = uiFontButtonVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (b);
  windows_control->AssignControlIDZOrder  = uiFontButtonAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiFontButtonChildVisibilityChanged;
  windows_control->LayoutRect             = uiFontButtonLayoutRect;
  windows_control->MinimumSize            = uiFontButtonMinimumSize;
  windows_control->MinimumSizeChanged     = uiFontButtonMinimumSizeChanged;
  windows_control->SetParentHWND          = uiFontButtonSetParentHWND;
  windows_control->SyncEnableState        = uiFontButtonSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  b->hwnd = uiWindowsEnsureCreateControlHWND (0, L"button", L"you should not be seeing this",
                                              BS_PUSHBUTTON | WS_TABSTOP, hInstance, nullptr, TRUE);

  uiprivLoadInitialFontDialogParams (&b->params);

  uiWindowsRegisterWM_COMMANDHandler (b->hwnd, onWM_COMMAND, uiControl (b));

  uiFontButtonOnChanged (b, defaultOnChanged, nullptr);

  updateFontButtonLabel (b);

  return b;
}

void
uiFreeFontButtonFont (uiFontDescriptor *desc)
{
  uiFreeFontDescriptor (desc);
}
