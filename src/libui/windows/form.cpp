#include <ui/form.h>

#include "form.h"

#include "init.h"
#include "utf16.h"
#include "winpublic.h"

#include <algorithm>
#include <controlsigs.h>
#include <uipriv.h>

static void
formPadding (const uiForm *f, int *xpadding, int *ypadding)
{
  uiWindowsSizing sizing;

  *xpadding = 0;
  *ypadding = 0;

  if (f->padded != 0)
    {
      uiWindowsGetSizing (f->hwnd, &sizing);
      uiWindowsSizingStandardPadding (&sizing, xpadding, ypadding);
    }
}

static void
formRelayout (const uiForm *f)
{
  RECT r;

  int xpadding;
  int ypadding;

  int minimumWidth;
  int minimumHeight;

  uiWindowsSizing sizing;

  int labelht;
  int labelyoff;

  if (f->controls->empty ())
    return;

  uiWindowsEnsureGetClientRect (f->hwnd, &r);

  int width = r.right - r.left;

  const int height = r.bottom - r.top;

  // 0) get this Form's padding
  formPadding (f, &xpadding, &ypadding);

  // 1) get width of labels and height of non-stretchy controls
  // this will tell us how much space will be left for controls
  int labelwid   = 0;
  int stretchyht = height;
  int nStretchy  = 0;
  int nVisible   = 0;
  for (formChild &fc : *f->controls)
    {
      if (uiControlVisible (fc.c) == 0)
        {
          ShowWindow (fc.label, SW_HIDE);
          continue;
        }

      ShowWindow (fc.label, SW_SHOW);

      nVisible++;

      const int thiswid = uiWindowsWindowTextWidth (fc.label);

      labelwid = std::max (labelwid, thiswid);

      if (fc.stretchy != 0)
        {
          nStretchy++;
          continue;
        }
      uiWindowsControlMinimumSize (uiWindowsControl (fc.c), &minimumWidth, &minimumHeight);
      fc.height = minimumHeight;
      stretchyht -= minimumHeight;
    }

  if (nVisible == 0) // nothing to do
    return;

  // 2) inset the available rect by the needed padding
  width -= xpadding;
  stretchyht -= (nVisible - 1) * ypadding;

  // 3) now get the width of controls and the height of stretchy controls
  width -= labelwid;
  if (nStretchy != 0)
    {
      stretchyht /= nStretchy;
      for (formChild &fc : *f->controls)
        {
          if (uiControlVisible (fc.c) == 0)
            continue;

          if (fc.stretchy != 0)
            fc.height = stretchyht;
        }
    }

  // 4) get the y offset
  labelyoff = labelYOffset;
  uiWindowsGetSizing (f->hwnd, &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, nullptr, &labelyoff);

  // 5) now we can position controls
  // first, make relative to the top-left corner of the container
  // also prefer left alignment on Windows
  const int x = labelwid + xpadding;
  int       y = 0;
  for (const formChild &fc : *f->controls)
    {
      if (uiControlVisible (fc.c) == 0)
        continue;

      labelht = labelHeight;

      uiWindowsGetSizing (f->hwnd, &sizing);

      uiWindowsSizingDlgUnitsToPixels (&sizing, nullptr, &labelht);

      uiWindowsEnsureMoveWindowDuringResize (fc.label, 0, y + labelyoff - sizing.InternalLeading, labelwid, labelht);

      uiWindowsEnsureMoveWindowDuringResize (reinterpret_cast<HWND> (uiControlHandle (fc.c)), x, y, width, fc.height);

      y += fc.height + ypadding;
    }
}

static void
uiFormDestroy (uiControl *c)
{
  auto *f = uiForm (c);

  for (const formChild &fc : *f->controls)
    {
      uiControlSetParent (fc.c, nullptr);
      uiControlDestroy (fc.c);
      uiWindowsEnsureDestroyWindow (fc.label);
    }

  delete f->controls;

  uiWindowsEnsureDestroyWindow (f->hwnd);

  uiFreeControl (uiControl (f));
}

static uintptr_t
uiFormHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiForm *> (c)->hwnd);
}

static uiControl *
uiFormParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiFormSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiFormToplevel (uiControl *)
{
  return 0;
}

static int
uiFormVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiFormShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiForm *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiFormHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiForm *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiFormEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiFormEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiFormDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiFormSyncEnableState (uiWindowsControl *c, const int enabled)
{
  auto *f = uiForm (c);

  if (uiWindowsShouldStopSyncEnableState (reinterpret_cast<uiWindowsControl *> (f), enabled) != 0)
    return;

  for (const formChild &fc : *f->controls)
    uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (fc.c), enabled);
}

static void
uiFormSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiForm *> (c)->hwnd, parent);
}

static void
uiFormMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const uiForm *f = reinterpret_cast<uiForm *> (c);

  int xpadding;
  int ypadding;

  // these two contain the largest minimum width and height of all stretchy controls in the form
  // all stretchy controls will use this value to determine the final minimum size
  int minimumWidth;
  int minimumHeight;

  *width  = 0;
  *height = 0;
  if (f->controls->empty ())
    return;

  // 0) get this Form's padding
  formPadding (f, &xpadding, &ypadding);

  // 1) determine the longest width of all controls and labels; add in the height of non-stretchy controls and get (but
  // not add in) the largest heights of stretchy controls we still add in like direction of stretchy controls
  int nStretchy         = 0;
  int maxLabelWidth     = 0;
  int maxControlWidth   = 0;
  int maxStretchyHeight = 0;
  int nVisible          = 0;

  for (const formChild &fc : *f->controls)
    {
      if (uiControlVisible (fc.c) == 0)
        continue;

      nVisible++;
      const int labelwid = uiWindowsWindowTextWidth (fc.label);

      maxLabelWidth = std::max (maxLabelWidth, labelwid);

      uiWindowsControlMinimumSize (uiWindowsControl (fc.c), &minimumWidth, &minimumHeight);

      if (fc.stretchy != 0)
        {
          nStretchy++;
          maxStretchyHeight = std::max (maxStretchyHeight, minimumHeight);
        }

      maxControlWidth = std::max (maxControlWidth, minimumWidth);

      if (fc.stretchy == 0)
        *height += minimumHeight;
    }
  if (nVisible == 0) // nothing to show; return 0x0
    return;
  *width += maxLabelWidth + maxControlWidth;

  // 2) outset the desired rect with the needed padding
  *width += xpadding;
  *height += (nVisible - 1) * ypadding;

  // 3) and now we can add in stretchy controls
  *height += nStretchy * maxStretchyHeight;
}

static void
uiFormMinimumSizeChanged (uiWindowsControl *c)
{
  auto *f = reinterpret_cast<uiForm *> (c);

  if (uiWindowsControlTooSmall (reinterpret_cast<uiWindowsControl *> (f)) != 0)
    {
      uiWindowsControlContinueMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (f));
      return;
    }
  formRelayout (f);
}

static void
uiFormLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiForm *> (c)->hwnd, r);
}

static void
uiFormAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiForm *> (c)->hwnd, controlID, insertAfter);
}

static void
uiFormChildVisibilityChanged (uiWindowsControl *c)
{
  uiWindowsControlMinimumSizeChanged (c);
}

static void
formArrangeChildren (const uiForm *f)
{
  LONG_PTR controlID   = 100;
  HWND     insertAfter = nullptr;

  for (const formChild &fc : *f->controls)
    uiWindowsControlAssignControlIDZOrder (reinterpret_cast<uiWindowsControl *> (fc.c), &controlID, &insertAfter);
}

void
uiFormAppend (uiForm *f, const char *label, uiControl *c, const int stretchy)
{
  WCHAR *wlabel = toUTF16 (label);

  formChild fc;
  fc.c     = c;
  fc.label = uiWindowsEnsureCreateControlHWND (0, L"STATIC", wlabel, SS_LEFT | SS_NOPREFIX, hInstance, nullptr, TRUE);

  uiprivFree (wlabel);

  uiWindowsEnsureSetParentHWND (fc.label, f->hwnd);

  fc.stretchy = stretchy;

  uiControlSetParent (fc.c, reinterpret_cast<uiControl *> (f));

  uiWindowsControlSetParentHWND (reinterpret_cast<uiWindowsControl *> (fc.c), f->hwnd);

  f->controls->push_back (fc);

  formArrangeChildren (f);

  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (f));
}

void
uiFormDelete (uiForm *f, const int index)
{
  const formChild fc = (*f->controls)[index];

  uiControlSetParent (fc.c, nullptr);

  uiWindowsControlSetParentHWND (reinterpret_cast<uiWindowsControl *> (fc.c), nullptr);

  uiWindowsEnsureDestroyWindow (fc.label);

  f->controls->erase (f->controls->begin () + index);

  formArrangeChildren (f);

  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (f));
}

int
uiFormNumChildren (const uiForm *f)
{
  return static_cast<int> (f->controls->size ());
}

int
uiFormPadded (const uiForm *f)
{
  return f->padded;
}

void
uiFormSetPadded (uiForm *f, const int padded)
{
  f->padded = padded;
  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (f));
}

static void
onResize (uiWindowsControl *c)
{
  formRelayout (uiForm (c));
}

uiForm *
uiNewForm (void)
{
  auto *f = reinterpret_cast<uiForm *> (uiWindowsAllocControl (sizeof (uiForm), uiFormSignature, "uiForm"));

  auto *control      = reinterpret_cast<uiControl *> (f);
  control->Destroy   = uiFormDestroy;
  control->Disable   = uiFormDisable;
  control->Enable    = uiFormEnable;
  control->Enabled   = uiFormEnabled;
  control->Handle    = uiFormHandle;
  control->Hide      = uiFormHide;
  control->Parent    = uiFormParent;
  control->SetParent = uiFormSetParent;
  control->Show      = uiFormShow;
  control->Toplevel  = uiFormToplevel;
  control->Visible   = uiFormVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (f);
  windows_control->AssignControlIDZOrder  = uiFormAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiFormChildVisibilityChanged;
  windows_control->LayoutRect             = uiFormLayoutRect;
  windows_control->MinimumSize            = uiFormMinimumSize;
  windows_control->MinimumSizeChanged     = uiFormMinimumSizeChanged;
  windows_control->SetParentHWND          = uiFormSetParentHWND;
  windows_control->SyncEnableState        = uiFormSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  f->hwnd = uiWindowsMakeContainer (windows_control, onResize);

  f->controls = new std::vector<formChild>;

  return f;
}
