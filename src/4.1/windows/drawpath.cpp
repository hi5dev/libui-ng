#include <uipriv.h>

#include "debug.h"
#include "draw.h"
#include "drawpath.h"

#include <cfloat>
#include <ui/userbugs.h>

#include <cmath>

uiDrawPath *
uiDrawNewPath (const uiDrawFillMode fillMode)
{
  const auto p = uiprivNew (uiDrawPath);

  HRESULT hr = d2dfactory->CreatePathGeometry (&p->path);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating path", hr);

  hr = p->path->Open (&(p->sink));
  if (hr != S_OK)
    (void)logHRESULT (L"error opening path", hr);

  switch (fillMode)
    {
    case uiDrawFillModeWinding:
      {
        p->sink->SetFillMode (D2D1_FILL_MODE_WINDING);
        break;
      }
    case uiDrawFillModeAlternate:
      {
        p->sink->SetFillMode (D2D1_FILL_MODE_ALTERNATE);
        break;
      }
    }

  return p;
}

void
uiDrawFreePath (uiDrawPath *p)
{
  if (p->inFigure != 0)
    p->sink->EndFigure (D2D1_FIGURE_END_OPEN);

  if (p->sink != nullptr)
    p->sink->Release ();

  p->path->Release ();

  uiprivFree (p);
}

void
uiDrawPathNewFigure (uiDrawPath *p, const double x, const double y)
{
  D2D1_POINT_2F pt;

  if (p->sink == nullptr)
    uiprivUserBug ("You cannot modify a uiDrawPath that has been ended. (path: %p)", p);

  if (p->inFigure != 0)
    p->sink->EndFigure (D2D1_FIGURE_END_OPEN);

  pt.x = x; // NOLINT(*-narrowing-conversions)
  pt.y = y; // NOLINT(*-narrowing-conversions)

  p->sink->BeginFigure (pt, D2D1_FIGURE_BEGIN_FILLED);
  p->inFigure = TRUE;
}

static void
drawArc (uiDrawPath *p, arc *a, void (*startFunction) (uiDrawPath *, double, double))
{
  D2D1_ARC_SEGMENT as;

  BOOL fullCircle = FALSE;

  const double absSweep = fabs (a->sweep);

  if (absSweep > 2 * uiPi)
    {
      fullCircle = TRUE;
    }

  else
    {
      const double aerDiff = fabs (absSweep - (2 * uiPi));
      fullCircle           = static_cast<BOOL> (aerDiff <= absSweep * aerMax);
    }

  if (fullCircle != 0)
    {
      a->sweep = uiPi;
      drawArc (p, a, startFunction);
      a->startAngle += uiPi;
      drawArc (p, a, nullptr);
      return;
    }

  // first, figure out the arc's endpoints
  double       sinx   = sin (a->startAngle);
  double       cosx   = cos (a->startAngle);
  const double startX = a->xCenter + (a->radius * cosx);
  const double startY = a->yCenter + (a->radius * sinx);
  sinx                = sin (a->startAngle + a->sweep);
  cosx                = cos (a->startAngle + a->sweep);
  const double endX   = a->xCenter + (a->radius * cosx);
  const double endY   = a->yCenter + (a->radius * sinx);

  // now do the initial step to get the current point to be the start point
  // this is either creating a new figure, drawing a line, or (in the case of our full circle code above) doing nothing
  if (startFunction != nullptr)
    (*startFunction) (p, startX, startY);

  as.point.x       = endX;      // NOLINT(*-narrowing-conversions)
  as.point.y       = endY;      // NOLINT(*-narrowing-conversions)
  as.size.width    = a->radius; // NOLINT(*-narrowing-conversions)
  as.size.height   = a->radius; // NOLINT(*-narrowing-conversions)
  as.rotationAngle = 0;

  if (a->negative != 0)
    as.sweepDirection = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;

  else
    as.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;

  if (a->negative == 0)
    {
      if (a->sweep > uiPi)
        as.arcSize = D2D1_ARC_SIZE_LARGE;
      else
        as.arcSize = D2D1_ARC_SIZE_SMALL;
    }
  else
    {
      if (a->sweep > uiPi)
        as.arcSize = D2D1_ARC_SIZE_SMALL;

      else
        as.arcSize = D2D1_ARC_SIZE_LARGE;
    }
  p->sink->AddArc (&as);
}

void
uiDrawPathNewFigureWithArc (uiDrawPath *p, const double xCenter, const double yCenter, const double radius,
                            const double startAngle, const double sweep, const int negative)
{
  arc a;

  if (p->sink == nullptr)
    uiprivUserBug ("You cannot modify a uiDrawPath that has been ended. (path: %p)", p);

  a.xCenter    = xCenter;
  a.yCenter    = yCenter;
  a.radius     = radius;
  a.startAngle = startAngle;
  a.sweep      = sweep;
  a.negative   = negative;
  drawArc (p, &a, uiDrawPathNewFigure);
}

void
uiDrawPathLineTo (uiDrawPath *p, const double x, const double y)
{
  D2D1_POINT_2F pt;

  if (p->sink == nullptr)
    uiprivUserBug ("You cannot modify a uiDrawPath that has been ended. (path: %p)", p);

  pt.x = x; // NOLINT(*-narrowing-conversions)
  pt.y = y; // NOLINT(*-narrowing-conversions)

  p->sink->AddLine (pt);
}

void
uiDrawPathArcTo (uiDrawPath *p, const double xCenter, const double yCenter, const double radius,
                 const double startAngle, const double sweep, const int negative)
{
  arc a;

  if (p->sink == nullptr)
    uiprivUserBug ("You cannot modify a uiDrawPath that has been ended. (path: %p)", p);

  a.xCenter    = xCenter;
  a.yCenter    = yCenter;
  a.radius     = radius;
  a.startAngle = startAngle;
  a.sweep      = sweep;
  a.negative   = negative;
  drawArc (p, &a, uiDrawPathLineTo);
}

void
uiDrawPathBezierTo (uiDrawPath *p, const double c1x, const double c1y, const double c2x, const double c2y,
                    const double endX, const double endY)
{
  D2D1_BEZIER_SEGMENT s;

  if (p->sink == nullptr)
    uiprivUserBug ("You cannot modify a uiDrawPath that has been ended. (path: %p)", p);

  s.point1.x = c1x;  // NOLINT(*-narrowing-conversions)
  s.point1.y = c1y;  // NOLINT(*-narrowing-conversions)
  s.point2.x = c2x;  // NOLINT(*-narrowing-conversions)
  s.point2.y = c2y;  // NOLINT(*-narrowing-conversions)
  s.point3.x = endX; // NOLINT(*-narrowing-conversions)
  s.point3.y = endY; // NOLINT(*-narrowing-conversions)
  p->sink->AddBezier (&s);
}

void
uiDrawPathCloseFigure (uiDrawPath *p)
{
  if (p->sink == nullptr)
    uiprivUserBug ("You cannot modify a uiDrawPath that has been ended. (path: %p)", p);

  p->sink->EndFigure (D2D1_FIGURE_END_CLOSED);
  p->inFigure = FALSE;
}

void
uiDrawPathAddRectangle (uiDrawPath *p, const double x, const double y, const double width, const double height)
{
  if (p->sink == nullptr)
    uiprivUserBug ("You cannot modify a uiDrawPath that has been ended. (path: %p)", p);

  uiDrawPathNewFigure (p, x, y);
  uiDrawPathLineTo (p, x + width, y);
  uiDrawPathLineTo (p, x + width, y + height);
  uiDrawPathLineTo (p, x, y + height);
  uiDrawPathCloseFigure (p);
}

void
uiDrawPathEnd (uiDrawPath *p)
{
  if (p->inFigure != 0)
    {
      p->sink->EndFigure (D2D1_FIGURE_END_OPEN);
      p->inFigure = FALSE;
    }

  const HRESULT hr = p->sink->Close ();
  if (hr != S_OK)
    (void)logHRESULT (L"error closing path", hr);

  p->sink->Release ();

  p->sink = nullptr;
}

int
uiDrawPathEnded (const uiDrawPath *p)
{
  return p->sink == nullptr ? 1 : 0;
}

ID2D1PathGeometry *
pathGeometry (uiDrawPath *p)
{
  if (p->sink != nullptr)
    uiprivUserBug ("You cannot draw with a uiDrawPath that was not ended. (path: %p)", p);

  return p->path;
}
