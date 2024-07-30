#include <windows.h>

#include <commctrl.h>

#include "slider.h"

#include "init.h"
#include "winpublic.h"

#include <controlsigs.h>

static BOOL
onWM_HSCROLL (uiControl *c, HWND, const WORD code, LRESULT *lResult)
{
  const auto s = reinterpret_cast<uiSlider *> (c);

  if (code == TB_ENDTRACK)
    (*s->onReleased) (s, s->onReleasedData);

  else
    (*s->onChanged) (s, s->onChangedData);

  *lResult = 0;

  return TRUE;
}

static void
uiSliderDestroy (uiControl *c)
{
  const auto s = reinterpret_cast<uiSlider *> (c);

  uiSliderSetHasToolTip (s, 1);
  uiWindowsUnregisterWM_HSCROLLHandler (s->hwnd);
  uiWindowsEnsureDestroyWindow (s->hwnd);
  uiFreeControl (reinterpret_cast<uiControl *> (s));
}

uiWindowsControlAllDefaultsExceptDestroy (uiSlider);

static void
uiSliderMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto s = reinterpret_cast<uiSlider *> (c);

  uiWindowsSizing sizing;
  uiWindowsGetSizing (s->hwnd, &sizing);

  int x = sliderWidth;
  int y = sliderHeight;
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);

  *width  = x;
  *height = y;
}

int
uiSliderHasToolTip (const uiSlider *s)
{
  return reinterpret_cast<HWND> (SendMessage (s->hwnd, TBM_GETTOOLTIPS, 0, 0)) == s->hwndToolTip;
}

void
uiSliderSetHasToolTip (uiSlider *s, const int hasToolTip)
{
  if (hasToolTip)
    SendMessage (s->hwnd, TBM_SETTOOLTIPS, reinterpret_cast<WPARAM> (s->hwndToolTip), 0);

  else
    SendMessage (s->hwnd, TBM_SETTOOLTIPS, 0, 0);
}

static void
defaultOnChanged (uiSlider *, void *)
{
  // do nothing
}

static void
defaultOnReleased (uiSlider *, void *)
{
  // do nothing
}

int
uiSliderValue (const uiSlider *s)
{
  return SendMessageW (s->hwnd, TBM_GETPOS, 0, 0);
}

void
uiSliderSetValue (const uiSlider *s, const int value)
{
  SendMessageW (s->hwnd, TBM_SETPOS, TRUE, value);
}

void
uiSliderOnChanged (uiSlider *s, void (*f) (uiSlider *, void *), void *data)
{
  s->onChanged     = f;
  s->onChangedData = data;
}

void
uiSliderOnReleased (uiSlider *s, void (*f) (uiSlider *, void *), void *data)
{
  s->onReleased     = f;
  s->onReleasedData = data;
}

void
uiSliderSetRange (const uiSlider *s, int min, int max)
{
  if (min >= max)
    {
      const int temp = min;

      min = max;
      max = temp;
    }

  SendMessageW (s->hwnd, TBM_SETRANGEMIN, TRUE, min);
  SendMessageW (s->hwnd, TBM_SETRANGEMAX, TRUE, max);
}

uiSlider *
uiNewSlider (int min, int max)
{
  if (min >= max)
    {
      const int temp = min;

      min = max;
      max = temp;
    }

  const auto s
      = reinterpret_cast<uiSlider *> (uiWindowsAllocControl (sizeof (uiSlider), uiSliderSignature, "uiSlider"));

  auto *control      = reinterpret_cast<uiControl *> (s);
  control->Destroy   = uiSliderDestroy;
  control->Disable   = uiSliderDisable;
  control->Enable    = uiSliderEnable;
  control->Enabled   = uiSliderEnabled;
  control->Handle    = uiSliderHandle;
  control->Hide      = uiSliderHide;
  control->Parent    = uiSliderParent;
  control->SetParent = uiSliderSetParent;
  control->Show      = uiSliderShow;
  control->Toplevel  = uiSliderToplevel;
  control->Visible   = uiSliderVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (s);
  windows_control->AssignControlIDZOrder  = uiSliderAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiSliderChildVisibilityChanged;
  windows_control->LayoutRect             = uiSliderLayoutRect;
  windows_control->MinimumSize            = uiSliderMinimumSize;
  windows_control->MinimumSizeChanged     = uiSliderMinimumSizeChanged;
  windows_control->SetParentHWND          = uiSliderSetParentHWND;
  windows_control->SyncEnableState        = uiSliderSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  static constexpr auto style = TBS_HORZ | TBS_TOOLTIPS | TBS_TRANSPARENTBKGND | WS_TABSTOP;
  s->hwnd = uiWindowsEnsureCreateControlHWND (0, TRACKBAR_CLASSW, L"", style, hInstance, nullptr, TRUE);

  uiWindowsRegisterWM_HSCROLLHandler (s->hwnd, onWM_HSCROLL, reinterpret_cast<uiControl *> (s));
  uiSliderOnChanged (s, defaultOnChanged, nullptr);
  uiSliderOnReleased (s, defaultOnReleased, nullptr);

  SendMessageW (s->hwnd, TBM_SETRANGEMIN, TRUE, min);
  SendMessageW (s->hwnd, TBM_SETRANGEMAX, TRUE, max);
  SendMessageW (s->hwnd, TBM_SETPOS, TRUE, min);

  s->hwndToolTip = reinterpret_cast<HWND> (SendMessage (s->hwnd, TBM_GETTOOLTIPS, 0, 0));

  return s;
}
