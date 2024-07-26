#include "test.h"

#include <ui/checkbox.h>

#include <stdio.h>

static uiArea     *area;
static uiCheckbox *label;

struct handler
{
  uiAreaHandler ah;
};

static struct handler handler;

static void
handlerDraw (uiAreaHandler *, uiArea *, const uiAreaDrawParams *)
{
  // do nothing
}

static void
handlerMouseEvent (uiAreaHandler *, uiArea *, const uiAreaMouseEvent *e)
{
  char pos[128];

  (void)sprintf (pos, "X %g Y %g", e->X, e->Y);
  uiCheckboxSetText (label, pos);
}

static void
handlerMouseCrossed (uiAreaHandler *, uiArea *, const int left)
{
  printf ("%d %d\n", left, !left);
  uiCheckboxSetChecked (label, !left);
}

static void
handlerDragBroken (uiAreaHandler *, uiArea *)
{
  // no-op
}

static int
handlerKeyEvent (uiAreaHandler *, uiArea *, const uiAreaKeyEvent *e)
{
  if (e->Key == 'h' && !e->Up)
    return 1;

  return 0;
}

uiGroup *
makePage7b (void)
{
  handler.ah.Draw         = handlerDraw;
  handler.ah.MouseEvent   = handlerMouseEvent;
  handler.ah.MouseCrossed = handlerMouseCrossed;
  handler.ah.DragBroken   = handlerDragBroken;
  handler.ah.KeyEvent     = handlerKeyEvent;

  uiGroup *group = newGroup ("Scrolling Mouse Test");
  uiBox   *box   = newVerticalBox ();
  uiGroupSetChild (group, uiControl (box));

  area = uiNewScrollingArea ((uiAreaHandler *)(&handler), 5000, 5000);
  uiBoxAppend (box, uiControl (area), 1);

  label = uiNewCheckbox ("");
  uiBoxAppend (box, uiControl (label), 0);

  return group;
}
