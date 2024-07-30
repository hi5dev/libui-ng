#include <windows.h>

#include "box.h"

#include "winpublic.h"

#include <controlsigs.h>

#include <ui/box.h>

#include <algorithm>

static void
boxPadding (const uiBox *b, int *xpadding, int *ypadding)
{
  uiWindowsSizing sizing;

  *xpadding = 0;
  *ypadding = 0;
  if (b->padded != 0)
    {
      uiWindowsGetSizing (b->hwnd, &sizing);
      uiWindowsSizingStandardPadding (&sizing, xpadding, ypadding);
    }
}

static void
boxRelayout (const uiBox *b)
{
  RECT r;
  int  xpadding;
  int  ypadding;
  int  minimumWidth;
  int  minimumHeight;

  if (b->controls->empty ())
    return;

  uiWindowsEnsureGetClientRect (b->hwnd, &r);
  int width  = r.right - r.left;
  int height = r.bottom - r.top;

  // -1) get this Box's padding
  boxPadding (b, &xpadding, &ypadding);

  // 1) get width and height of non-stretchy controls
  // this will tell us how much space will be left for stretchy controls
  int stretchywid = width;
  int stretchyht  = height;
  int nStretchy   = 0;
  int nVisible    = 0;
  for (boxChild &bc : *b->controls)
    {
      if (uiControlVisible (bc.c) == 0)
        continue;

      nVisible++;

      if (bc.stretchy != 0)
        {
          nStretchy++;
          continue;
        }

      uiWindowsControlMinimumSize (uiWindowsControl (bc.c), &minimumWidth, &minimumHeight);

      if (b->vertical != 0)
        {
          // all controls have same width
          bc.width  = width;
          bc.height = minimumHeight;
          stretchyht -= minimumHeight;
        }

      else
        {
          // all controls have same height
          bc.width  = minimumWidth;
          bc.height = height;
          stretchywid -= minimumWidth;
        }
    }
  if (nVisible == 0)
    return;

  // 2) now inset the available rect by the needed padding
  if (b->vertical != 0)
    stretchyht -= (nVisible - 1) * ypadding;

  else
    stretchywid -= (nVisible - 1) * xpadding;

  // 3) now get the size of stretchy controls
  if (nStretchy != 0)
    {
      if (b->vertical != 0)
        stretchyht /= nStretchy;
      else
        stretchywid /= nStretchy;
      for (boxChild &bc : *b->controls)
        {
          if (uiControlVisible (bc.c) == 0)
            continue;

          if (bc.stretchy != 0)
            {
              bc.width  = stretchywid;
              bc.height = stretchyht;
            }
        }
    }

  // 4) now we can position controls
  // first, make relative to the top-left corner of the container
  int x = 0;
  int y = 0;
  for (const boxChild &bc : *b->controls)
    {
      if (uiControlVisible (bc.c) == 0)
        continue;

      uiWindowsEnsureMoveWindowDuringResize (reinterpret_cast<HWND> (uiControlHandle (bc.c)), x, y, bc.width,
                                             bc.height);

      if (b->vertical != 0)
        y += bc.height + ypadding;

      else
        x += bc.width + xpadding;
    }
}

static void
uiBoxDestroy (uiControl *c)
{
  auto *b = uiBox (c);

  for (const boxChild &bc : *b->controls)
    {
      uiControlSetParent (bc.c, nullptr);
      uiControlDestroy (bc.c);
    }
  delete b->controls;
  uiWindowsEnsureDestroyWindow (b->hwnd);
  uiFreeControl (uiControl (b));
}

static uintptr_t
uiBoxHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiBox *> (c)->hwnd);
}

static uiControl *
uiBoxParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiBoxSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiBoxToplevel (uiControl *)
{
  return 0;
}

static int
uiBoxVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiBoxShow (uiControl *c)
{
  auto *control = reinterpret_cast<uiWindowsControl *> (c);

  control->visible = 1;

  ShowWindow (uiBox (c)->hwnd, SW_SHOW);

  uiWindowsControlNotifyVisibilityChanged (control);
}

static void
uiBoxHide (uiControl *c)
{
  auto *control = reinterpret_cast<uiWindowsControl *> (c);

  control->visible = 0;

  ShowWindow (uiBox (c)->hwnd, SW_HIDE);

  uiWindowsControlNotifyVisibilityChanged (control);
}

static int
uiBoxEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiBoxEnable (uiControl *c)
{
  auto *control = reinterpret_cast<uiWindowsControl *> (c);

  control->enabled = 1;

  uiWindowsControlSyncEnableState (control, uiControlEnabledToUser (c));
}

static void
uiBoxDisable (uiControl *c)
{
  auto *control = reinterpret_cast<uiWindowsControl *> (c);

  control->enabled = 0;

  uiWindowsControlSyncEnableState (control, uiControlEnabledToUser (c));
}

static void
uiBoxSyncEnableState (uiWindowsControl *c, const int enabled)
{
  auto *b = reinterpret_cast<uiBox *> (c);

  if (uiWindowsShouldStopSyncEnableState (uiWindowsControl (b), enabled) != 0)
    return;

  for (const boxChild &bc : *b->controls)
    uiWindowsControlSyncEnableState (uiWindowsControl (bc.c), enabled);
}

static void
uiBoxSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (uiBox (c)->hwnd, parent);
}

static void
uiBoxMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const auto *b = uiBox (c);

  int xpadding;
  int ypadding;

  // these two contain the largest minimum width and height of all stretchy controls in the box
  // all stretchy controls will use this value to determine the final minimum size
  int minimumWidth;
  int minimumHeight;

  *width  = 0;
  *height = 0;

  if (b->controls->empty ())
    return;

  // 0) get this Box's padding
  boxPadding (b, &xpadding, &ypadding);

  // 1) add in the size of non-stretchy controls and get (but not add in) the largest widths and heights of stretchy
  // controls we still add in like direction of stretchy controls
  int nStretchy         = 0;
  int maxStretchyWidth  = 0;
  int maxStretchyHeight = 0;
  int nVisible          = 0;
  for (const boxChild &bc : *b->controls)
    {
      if (uiControlVisible (bc.c) == 0)
        continue;

      nVisible++;

      uiWindowsControlMinimumSize (uiWindowsControl (bc.c), &minimumWidth, &minimumHeight);

      if (bc.stretchy != 0)
        {
          nStretchy++;
          maxStretchyWidth  = std::max (maxStretchyWidth, minimumWidth);
          maxStretchyHeight = std::max (maxStretchyHeight, minimumHeight);
        }

      if (b->vertical != 0)
        {
          *width = std::max (*width, minimumWidth);

          if (bc.stretchy == 0)
            *height += minimumHeight;
        }

      else
        {
          if (bc.stretchy == 0)
            *width += minimumWidth;

          *height = std::max (*height, minimumHeight);
        }
    }
  if (nVisible == 0) // just return 0x0
    return;

  // 2) now outset the desired rect with the needed padding
  if (b->vertical != 0)
    *height += (nVisible - 1) * ypadding;

  else
    *width += (nVisible - 1) * xpadding;

  // 3) and now we can add in stretchy controls
  if (b->vertical != 0)
    *height += nStretchy * maxStretchyHeight;

  else
    *width += nStretchy * maxStretchyWidth;
}

static void
uiBoxMinimumSizeChanged (uiWindowsControl *c)
{
  auto *b = uiBox (c);

  if (uiWindowsControlTooSmall (uiWindowsControl (b)) != 0)
    {
      uiWindowsControlContinueMinimumSizeChanged (uiWindowsControl (b));
      return;
    }

  boxRelayout (b);
}

static void
uiBoxLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (uiBox (c)->hwnd, r);
}

static void
uiBoxAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (uiBox (c)->hwnd, controlID, insertAfter);
}

static void
uiBoxChildVisibilityChanged (uiWindowsControl *c)
{
  uiWindowsControlMinimumSizeChanged (c);
}

static void
boxArrangeChildren (const uiBox *b)
{
  LONG_PTR controlID   = 100;
  HWND     insertAfter = nullptr;

  for (const boxChild &bc : *b->controls)
    uiWindowsControlAssignControlIDZOrder (uiWindowsControl (bc.c), &controlID, &insertAfter);
}

void
uiBoxAppend (uiBox *b, uiControl *child, const int stretchy)
{
  boxChild bc;

  bc.c        = child;
  bc.stretchy = stretchy;

  uiControlSetParent (bc.c, uiControl (b));
  uiWindowsControlSetParentHWND (uiWindowsControl (bc.c), b->hwnd);

  b->controls->push_back (bc);
  boxArrangeChildren (b);
  uiWindowsControlMinimumSizeChanged (uiWindowsControl (b));
}

void
uiBoxDelete (uiBox *b, const int index)
{
  uiControl *c = (*b->controls)[index].c;
  uiControlSetParent (c, nullptr);
  uiWindowsControlSetParentHWND (uiWindowsControl (c), nullptr);

  b->controls->erase (b->controls->begin () + index);
  boxArrangeChildren (b);
  uiWindowsControlMinimumSizeChanged (uiWindowsControl (b));
}

int
uiBoxNumChildren (const uiBox *b)
{
  return static_cast<int> (b->controls->size ());
}

int
uiBoxPadded (const uiBox *b)
{
  return b->padded;
}

void
uiBoxSetPadded (uiBox *b, const int padded)
{
  b->padded = padded;
  uiWindowsControlMinimumSizeChanged (uiWindowsControl (b));
}

static void
onResize (uiWindowsControl *c)
{
  boxRelayout (uiBox (c));
}

static uiBox *
finishNewBox (const int vertical)
{
  uiBox *b;

  uiWindowsNewControl (uiBox, b);

  b->hwnd = uiWindowsMakeContainer (uiWindowsControl (b), onResize);

  b->vertical = vertical;
  b->controls = new std::vector<boxChild>;

  return b;
}

uiBox *
uiNewHorizontalBox ()
{
  return finishNewBox (0);
}

uiBox *
uiNewVerticalBox ()
{
  return finishNewBox (1);
}
