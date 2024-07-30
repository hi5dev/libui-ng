#pragma once

#include <windows.h>

#include <d2d1.h>

#include <ui/draw.h>

#define aerMax (6 * DBL_EPSILON)

typedef struct uiDrawPath
{
  ID2D1PathGeometry *path;

  ID2D1GeometrySink *sink;

  BOOL inFigure;
} uiDrawPath;

/**
 * @brief An arc in Direct2D is defined by the chord between the endpoints.
 *
 * There are four possible arcs with the same two endpoints that you can draw this way.
 *
 * There is a property rotationAngle which deals with the rotation /of the entire ellipse that forms an ellpitical
 * arc. It's effectively a transformation on the arc. That is to say, it's NOT THE SWEEP. The sweep is defined by the
 * start and end points and whether the arc is "large". As a result, this design does not allow for full circles or
 * ellipses with a single arc; they have to be simulated with two.
 */
typedef struct arc
{
  double xCenter;

  double yCenter;

  double radius;

  double startAngle;

  double sweep;

  int negative;
} arc;

extern ID2D1PathGeometry *pathGeometry (uiDrawPath *p);
