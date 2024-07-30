#include <windows.h>

#include <commctrl.h>

#include <ui/color_button.h>

#include <controlsigs.h>

#include "colorbutton.h"
#include "colordialog.h"
#include "debug.h"
#include "draw.h"
#include "init.h"
#include "winpublic.h"
#include "winutil.h"

static void
uiColorButtonDestroy (uiControl *c)
{
  auto *b = reinterpret_cast<uiColorButton *> (c);

  uiWindowsUnregisterWM_COMMANDHandler (b->hwnd);
  uiWindowsUnregisterWM_NOTIFYHandler (b->hwnd);
  uiWindowsEnsureDestroyWindow (b->hwnd);

  uiFreeControl (reinterpret_cast<uiControl *> (b));
}

static BOOL
onWM_COMMAND (uiControl *c, HWND, const WORD code, LRESULT *lResult)
{
  auto *b = reinterpret_cast<uiColorButton *> (c);

  colorDialogRGBA rgba;

  if (code != BN_CLICKED)
    return FALSE;

  const HWND parent = parentToplevel (b->hwnd);

  rgba.r = b->r;
  rgba.g = b->g;
  rgba.b = b->b;
  rgba.a = b->a;

  if (showColorDialog (parent, &rgba) != 0)
    {
      b->r = rgba.r;
      b->g = rgba.g;
      b->b = rgba.b;
      b->a = rgba.a;

      invalidateRect (b->hwnd, nullptr, TRUE);

      (*b->onChanged) (b, b->onChangedData);
    }

  *lResult = 0;

  return TRUE;
}

static BOOL
onWM_NOTIFY (uiControl *c, HWND, NMHDR *nmhdr, LRESULT *lResult)
{
  const uiColorButton *b  = reinterpret_cast<uiColorButton *> (c);
  const auto          *nm = reinterpret_cast<NMCUSTOMDRAW *> (nmhdr);

  RECT client;

  D2D1_RECT_F r;

  D2D1_COLOR_F color;

  D2D1_BRUSH_PROPERTIES bprop;

  ID2D1SolidColorBrush *brush;

  uiWindowsSizing sizing;

  int x;
  int y;

  if (nmhdr->code != NM_CUSTOMDRAW)
    return FALSE;

  // and allow the button to draw its background
  if (nm->dwDrawStage != CDDS_PREPAINT)
    return FALSE;

  uiWindowsEnsureGetClientRect (b->hwnd, &client);
  ID2D1DCRenderTarget *rt = makeHDCRenderTarget (nm->hdc, &client);
  rt->BeginDraw ();

  uiWindowsGetSizing (b->hwnd, &sizing);
  x = 3;
  y = 3;

  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);
  r.left   = client.left + x;   // NOLINT(*-narrowing-conversions)
  r.top    = client.top + y;    // NOLINT(*-narrowing-conversions)
  r.right  = client.right - x;  // NOLINT(*-narrowing-conversions)
  r.bottom = client.bottom - y; // NOLINT(*-narrowing-conversions)

  color.r = b->r; // NOLINT(*-narrowing-conversions)
  color.g = b->g; // NOLINT(*-narrowing-conversions)
  color.b = b->b; // NOLINT(*-narrowing-conversions)
  color.a = b->a; // NOLINT(*-narrowing-conversions)

  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = 1.0;
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  HRESULT hr = rt->CreateSolidColorBrush (&color, &bprop, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating brush for color button", hr);

  rt->FillRectangle (&r, brush);
  brush->Release ();

  hr = rt->EndDraw (nullptr, nullptr);
  if (hr != S_OK)
    (void)logHRESULT (L"error drawing color on color button", hr);

  rt->Release ();

  // skip default processing (don't draw text)
  *lResult = CDRF_SKIPDEFAULT;
  return TRUE;
}

static uintptr_t
uiColorButtonHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiColorButton *> (c)->hwnd);
}

static uiControl *
uiColorButtonParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiColorButtonSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiColorButtonToplevel (uiControl *c)
{
  return 0;
}

static int
uiColorButtonVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiColorButtonShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiColorButton *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiColorButtonHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiColorButton *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiColorButtonEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiColorButtonEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiColorButtonDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiColorButtonSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;

  EnableWindow (reinterpret_cast<uiColorButton *> (c)->hwnd, enabled);
}

static void
uiColorButtonSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiColorButton *> (c)->hwnd, parent);
}

static void
uiColorButtonMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)

    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiColorButtonLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiColorButton *> (c)->hwnd, r);
}

static void
uiColorButtonAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiColorButton *> (c)->hwnd, controlID, insertAfter);
}

static void
uiColorButtonChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiColorButtonMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const uiColorButton *b = uiColorButton (c);
  SIZE                 size;
  uiWindowsSizing      sizing;
  int                  y;

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
defaultOnChanged (uiColorButton *, void *)
{
  // do nothing
}

void
uiColorButtonColor (const uiColorButton *b, double *r, double *g, double *bl, double *a)
{
  *r  = b->r;
  *g  = b->g;
  *bl = b->b;
  *a  = b->a;
}

void
uiColorButtonSetColor (uiColorButton *b, const double r, const double g, const double bl, const double a)
{
  b->r = r;
  b->g = g;
  b->b = bl;
  b->a = a;

  invalidateRect (b->hwnd, nullptr, TRUE);
}

void
uiColorButtonOnChanged (uiColorButton *b, void (*f) (uiColorButton *, void *), void *data)
{
  b->onChanged     = f;
  b->onChangedData = data;
}

uiColorButton *
uiNewColorButton ()
{
  auto *b = reinterpret_cast<uiColorButton *> (
      uiWindowsAllocControl (sizeof (uiColorButton), uiColorButtonSignature, "uiColorButton"));

  auto *control      = reinterpret_cast<uiControl *> (b);
  control->Destroy   = uiColorButtonDestroy;
  control->Disable   = uiColorButtonDisable;
  control->Enable    = uiColorButtonEnable;
  control->Enabled   = uiColorButtonEnabled;
  control->Handle    = uiColorButtonHandle;
  control->Hide      = uiColorButtonHide;
  control->Parent    = uiColorButtonParent;
  control->SetParent = uiColorButtonSetParent;
  control->Show      = uiColorButtonShow;
  control->Toplevel  = uiColorButtonToplevel;
  control->Visible   = uiColorButtonVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (b);
  windows_control->AssignControlIDZOrder  = uiColorButtonAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiColorButtonChildVisibilityChanged;
  windows_control->LayoutRect             = uiColorButtonLayoutRect;
  windows_control->MinimumSize            = uiColorButtonMinimumSize;
  windows_control->MinimumSizeChanged     = uiColorButtonMinimumSizeChanged;
  windows_control->SetParentHWND          = uiColorButtonSetParentHWND;
  windows_control->SyncEnableState        = uiColorButtonSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  // initial color is black
  b->r = 0.0;
  b->g = 0.0;
  b->b = 0.0;
  b->a = 1.0;

  b->hwnd
      = uiWindowsEnsureCreateControlHWND (0, L"button", L" ", BS_PUSHBUTTON | WS_TABSTOP, hInstance, nullptr, TRUE);

  uiWindowsRegisterWM_COMMANDHandler (b->hwnd, onWM_COMMAND, uiControl (b));
  uiWindowsRegisterWM_NOTIFYHandler (b->hwnd, onWM_NOTIFY, uiControl (b));
  uiColorButtonOnChanged (b, defaultOnChanged, nullptr);

  return b;
}
