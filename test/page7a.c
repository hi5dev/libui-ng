#include "test.h"

#include <ui/checkbox.h>
#include <ui/entry.h>
#include <ui/label.h>

#include <stdlib.h>

static uiArea     *area;
static uiEntry    *startAngle;
static uiEntry    *sweep;
static uiCheckbox *negative;
static uiCheckbox *radians;

struct handler
{
  uiAreaHandler ah;
};

static struct handler handler;

static void
handlerDraw (uiAreaHandler *, uiArea *, const uiAreaDrawParams *p)
{
  const double       xc     = 128.0;
  const double       yc     = 128.0;
  const double       radius = 100.0;
  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  source.Type   = uiDrawBrushTypeSolid;
  source.R      = 0;
  source.G      = 0;
  source.B      = 0;
  source.A      = 1;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  char *startText = uiEntryText (startAngle);
  char *sweepText = uiEntryText (sweep);

  double factor = uiPi / 180;
  if (uiCheckboxChecked (radians))
    factor = 1;

  sp.Thickness     = 10.0;
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, xc, yc);
  uiDrawPathArcTo (path, xc, yc, radius, atof (startText) * factor, atof (sweepText) * factor,
                   uiCheckboxChecked (negative));
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  uiFreeText (startText);
  uiFreeText (sweepText);
}

static void
handlerMouseEvent (uiAreaHandler *, uiArea *, const uiAreaMouseEvent *)
{
  // no-op
}

static void
handlerMouseCrossed (uiAreaHandler *, uiArea *, int )
{
  // no-op
}

static void
handlerDragBroken (uiAreaHandler *, uiArea *)
{
  // no-op
}

static int
handlerKeyEvent (uiAreaHandler *, uiArea *, const uiAreaKeyEvent *)
{
  return 0;
}

static void
entryChanged (uiEntry *, void *)
{
  uiAreaQueueRedrawAll (area);
}

static void
checkboxToggled (uiCheckbox *, void *)
{
  uiAreaQueueRedrawAll (area);
}

uiGroup *
makePage7a (void)
{

  handler.ah.Draw         = handlerDraw;
  handler.ah.MouseEvent   = handlerMouseEvent;
  handler.ah.MouseCrossed = handlerMouseCrossed;
  handler.ah.DragBroken   = handlerDragBroken;
  handler.ah.KeyEvent     = handlerKeyEvent;

  uiGroup *group = newGroup ("Arc Test");

  uiBox *box = newVerticalBox ();
  uiGroupSetChild (group, uiControl (box));

  area = uiNewArea ((uiAreaHandler *)(&handler));
  uiBoxAppend (box, uiControl (area), 1);

  uiBox *box2 = newHorizontalBox ();
  uiBoxAppend (box, uiControl (box2), 0);

  uiBoxAppend (box2, uiControl (uiNewLabel ("Start Angle")), 0);
  startAngle = uiNewEntry ();
  uiEntryOnChanged (startAngle, entryChanged, NULL);
  uiBoxAppend (box2, uiControl (startAngle), 1);

  box2 = newHorizontalBox ();
  uiBoxAppend (box, uiControl (box2), 0);

  uiBoxAppend (box2, uiControl (uiNewLabel ("Sweep")), 0);
  sweep = uiNewEntry ();
  uiEntryOnChanged (sweep, entryChanged, NULL);
  uiBoxAppend (box2, uiControl (sweep), 1);

  negative = uiNewCheckbox ("Negative");
  uiCheckboxOnToggled (negative, checkboxToggled, NULL);
  uiBoxAppend (box, uiControl (negative), 0);

  radians = uiNewCheckbox ("Radians");
  uiCheckboxOnToggled (radians, checkboxToggled, NULL);
  uiBoxAppend (box, uiControl (radians), 0);

  return group;
}
