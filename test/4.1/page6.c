#include "test.h"

#include <ui/checkbox.h>

#include <inttypes.h>
#include <stdio.h>
#include <ui/button.h>

static uiArea     *area;
static uiCombobox *which;
static uiCheckbox *swallowKeys;

struct handler
{
  uiAreaHandler ah;
};

static struct handler handler;

static void
// ReSharper disable once CppParameterMayBeConstPtrOrRef
handlerDraw (uiAreaHandler *, uiArea *, uiAreaDrawParams *p)
{
  runDrawTest (uiComboboxSelected (which), p);
}

static void
// ReSharper disable once CppParameterMayBeConstPtrOrRef
handlerMouseEvent (uiAreaHandler *, uiArea *, uiAreaMouseEvent *e)
{
  (void)printf ("mouse (%g,%g):(%g,%g) down:%d up:%d count:%d mods:%x held:0x%" PRIX64 "\n", e->X, e->Y, e->AreaWidth,
                e->AreaHeight, e->Down, e->Up, e->Count, (uint32_t)e->Modifiers, e->Held1To64);
}

static void
handlerMouseCrossed (uiAreaHandler *ah, uiArea *a, int left)
{
  (void)printf ("mouse crossed %d\n", left);
}

static void
handlerDragBroken (uiAreaHandler *, uiArea *)
{
  (void)printf ("drag broken\n");
}

static int
// ReSharper disable once CppParameterMayBeConstPtrOrRef
handlerKeyEvent (uiAreaHandler *, uiArea *, uiAreaKeyEvent *e)
{
  char k[4];

  k[0] = '\'';
  k[1] = e->Key;
  k[2] = '\'';
  k[3] = '\0';
  if (e->Key == 0)
    {
      k[0] = '0';
      k[1] = '\0';
    }
  printf ("key key:%s extkey:%d mod:%d mods:%d up:%d\n", k, (int)e->ExtKey, (int)e->Modifier, (int)e->Modifiers,
          e->Up);
  return uiCheckboxChecked (swallowKeys);
}

static void
shouldntHappen (uiCombobox *c, void *data)
{
  fprintf (stderr,
           "YOU SHOULD NOT SEE THIS. If you do, uiComboboxSetSelected() is triggering uiComboboxOnSelected(), which "
           "it should not.\n");
}

static void
redraw (uiCombobox *c, void *data)
{
  uiAreaQueueRedrawAll (area);
}

static void
// ReSharper disable once CppParameterMayBeConstPtrOrRef
enableArea (uiButton *, void *data)
{
  if (data != NULL)
    uiControlEnable (uiControl (area));

  else
    uiControlDisable (uiControl (area));
}

uiBox *
makePage6 (void)
{
  handler.ah.Draw         = handlerDraw;
  handler.ah.MouseEvent   = handlerMouseEvent;
  handler.ah.MouseCrossed = handlerMouseCrossed;
  handler.ah.DragBroken   = handlerDragBroken;
  handler.ah.KeyEvent     = handlerKeyEvent;

  uiBox *page6 = newVerticalBox ();

  uiBox *hbox = newHorizontalBox ();
  uiBoxAppend (page6, uiControl (hbox), 0);

  which = uiNewCombobox ();
  populateComboboxWithTests (which);
  // this is to make sure that uiComboboxOnSelected() doesn't trigger with uiComboboxSetSelected()
  uiComboboxOnSelected (which, shouldntHappen, NULL);
  uiComboboxSetSelected (which, 0);
  uiComboboxOnSelected (which, redraw, NULL);
  uiBoxAppend (hbox, uiControl (which), 0);

  area = uiNewArea ((uiAreaHandler *)(&handler));
  uiBoxAppend (page6, uiControl (area), 1);

  hbox = newHorizontalBox ();
  uiBoxAppend (page6, uiControl (hbox), 0);

  swallowKeys = uiNewCheckbox ("Consider key events handled");
  uiBoxAppend (hbox, uiControl (swallowKeys), 1);

  uiButton *button = uiNewButton ("Enable");
  uiButtonOnClicked (button, enableArea, button);
  uiBoxAppend (hbox, uiControl (button), 0);

  button = uiNewButton ("Disable");
  uiButtonOnClicked (button, enableArea, NULL);
  uiBoxAppend (hbox, uiControl (button), 0);

  return page6;
}
