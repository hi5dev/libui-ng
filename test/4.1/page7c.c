#include "test.h"

#include <string.h>

static uiArea *area;

struct handler
{
  uiAreaHandler ah;
};

static struct handler handler;

#define areaSize        250
#define borderThickness 1
#define padding         30
#define circleRadius    ((areaSize - padding) / 2)

static void
// ReSharper disable once CppParameterMayBeConstPtrOrRef
handlerDraw (uiAreaHandler *, uiArea *, uiAreaDrawParams *dp)
{
  uiDrawBrush             brush;
  uiDrawStrokeParams      sp;
  uiDrawBrushGradientStop stops[2];

  memset (&brush, 0, sizeof (uiDrawBrush));
  memset (&sp, 0, sizeof (uiDrawStrokeParams));

  // add some buffering to detect scrolls that aren't on the dot and draws that are outside the scroll area on Windows
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, -50, -50, areaSize + 100, areaSize + 100);
  uiDrawPathEnd (path);
  brush.Type = uiDrawBrushTypeSolid;
  brush.R    = 0;
  brush.G    = 1;
  brush.B    = 0;
  brush.A    = 1;
  uiDrawFill (dp->Context, path, &brush);
  uiDrawFreePath (path);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 0, 0, areaSize, areaSize);
  uiDrawPathEnd (path);
  brush.Type = uiDrawBrushTypeSolid;
  brush.R    = 1;
  brush.G    = 1;
  brush.B    = 1;
  brush.A    = 1;
  uiDrawFill (dp->Context, path, &brush);
  brush.Type    = uiDrawBrushTypeSolid;
  brush.R       = 1;
  brush.G       = 0;
  brush.B       = 0;
  brush.A       = 1;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.Thickness  = 1;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (dp->Context, path, &brush, &sp);
  uiDrawFreePath (path);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (path, areaSize / 2, areaSize / 2, circleRadius, 0, 2 * uiPi, 0);
  uiDrawPathEnd (path);
  stops[0].Pos   = 0.0;
  stops[0].R     = 0.0;
  stops[0].G     = 1.0;
  stops[0].B     = 1.0;
  stops[0].A     = 1.0;
  stops[1].Pos   = 1.0;
  stops[1].R     = 0.0;
  stops[1].G     = 0.0;
  stops[1].B     = 1.0;
  stops[1].A     = 1.0;
  brush.Type     = uiDrawBrushTypeLinearGradient;
  brush.X0       = areaSize / 2;
  brush.Y0       = padding;
  brush.X1       = areaSize / 2;
  brush.Y1       = areaSize - padding;
  brush.Stops    = stops;
  brush.NumStops = 2;
  uiDrawFill (dp->Context, path, &brush);
  uiDrawFreePath (path);
}

static void
handlerMouseEvent (uiAreaHandler *, uiArea *, uiAreaMouseEvent *)
{
  // do nothing
}

static void
handlerMouseCrossed (uiAreaHandler *, uiArea *, int)
{
  // no-op
}

static void
handlerDragBroken (uiAreaHandler *, uiArea *)
{
  // no-op
}

static int
// ReSharper disable once CppParameterMayBeConstPtrOrRef
handlerKeyEvent (uiAreaHandler *, uiArea *, uiAreaKeyEvent *e)
{
  if (e->Key == 'h' && !e->Up)
    return 1;

  return 0;
}

uiGroup *
makePage7c (void)
{
  handler.ah.Draw         = handlerDraw;
  handler.ah.MouseEvent   = handlerMouseEvent;
  handler.ah.MouseCrossed = handlerMouseCrossed;
  handler.ah.DragBroken   = handlerDragBroken;
  handler.ah.KeyEvent     = handlerKeyEvent;

  uiGroup *group = newGroup ("Scrolling Drawing Test");

  area = uiNewScrollingArea ((uiAreaHandler *)&handler, areaSize, areaSize);
  uiGroupSetChild (group, uiControl (area));

  return group;
}
