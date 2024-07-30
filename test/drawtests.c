// ReSharper disable CppDFAConstantParameter
#include "test.h"

struct drawtest
{
  const char *name;

  void (*draw) (const uiAreaDrawParams *p);
};

static void
drawOriginal (const uiAreaDrawParams *p)
{
  uiDrawBrush        brush;
  uiDrawStrokeParams sp;

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  brush.Type = uiDrawBrushTypeSolid;
  brush.A    = 1;

  brush.R          = 1;
  brush.G          = 0;
  brush.B          = 0;
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, p->ClipX + 5, p->ClipY + 5);
  uiDrawPathLineTo (path, p->ClipX + p->ClipWidth - 5, p->ClipY + p->ClipHeight - 5);
  uiDrawPathEnd (path);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.Thickness  = 1;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, path, &brush, &sp);
  uiDrawFreePath (path);

  brush.R = 0;
  brush.G = 0;
  brush.B = 0.75;
  path    = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, p->ClipX, p->ClipY);
  uiDrawPathLineTo (path, p->ClipX + p->ClipWidth, p->ClipY);
  uiDrawPathLineTo (path, 50, 150);
  uiDrawPathLineTo (path, 50, 50);
  uiDrawPathCloseFigure (path);
  uiDrawPathEnd (path);
  sp.Cap       = uiDrawLineCapFlat;
  sp.Join      = uiDrawLineJoinRound;
  sp.Thickness = 5;
  uiDrawStroke (p->Context, path, &brush, &sp);
  uiDrawFreePath (path);

  brush.R = 0;
  brush.G = 0.75;
  brush.B = 0;
  brush.A = 0.5;
  path    = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 120, 80, 50, 50);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &brush);
  uiDrawFreePath (path);
  brush.A = 1;

  brush.R = 0;
  brush.G = 0.5;
  brush.B = 0;
  path    = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 5.5, 10.5);
  uiDrawPathLineTo (path, 5.5, 50.5);
  uiDrawPathEnd (path);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.Thickness  = 1;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, path, &brush, &sp);
  uiDrawFreePath (path);

  brush.R = 0.5;
  brush.G = 0.75;
  brush.B = 0;
  path    = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 400, 100);
  uiDrawPathArcTo (path, 400, 100, 50, 30. * (uiPi / 180.), 300. * (uiPi / 180.), 0);

  // the sweep test below doubles as a clockwise test so a checkbox isn't needed anymore
  uiDrawPathLineTo (path, 400, 100);
  uiDrawPathNewFigureWithArc (path, 510, 100, 50, 30. * (uiPi / 180.), 300. * (uiPi / 180.), 0);
  uiDrawPathCloseFigure (path);

  // and now with 330 to make sure sweeps work properly
  uiDrawPathNewFigure (path, 400, 210);
  uiDrawPathArcTo (path, 400, 210, 50, 30. * (uiPi / 180.), 330. * (uiPi / 180.), 0);
  uiDrawPathLineTo (path, 400, 210);
  uiDrawPathNewFigureWithArc (path, 510, 210, 50, 30. * (uiPi / 180.), 330. * (uiPi / 180.), 0);
  uiDrawPathCloseFigure (path);
  uiDrawPathEnd (path);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.Thickness  = 1;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, path, &brush, &sp);
  uiDrawFreePath (path);

  brush.R = 0;
  brush.G = 0.5;
  brush.B = 0.75;
  path    = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 300, 300);
  uiDrawPathBezierTo (path, 350, 320, 310, 390, 435, 372);
  uiDrawPathEnd (path);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.Thickness  = 1;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, path, &brush, &sp);
  uiDrawFreePath (path);
}

static void
drawArcs (const uiAreaDrawParams *p)
{
  const int          start = 20;
  const int          step  = 20;
  const int          rad   = 25;
  int                i;
  uiDrawBrush        brush;
  uiDrawStrokeParams sp;

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);

  const double add = 2.0 * uiPi / 12;

  int    x     = start + rad;
  int    y     = start + rad;
  double angle = 0;
  for (i = 0; i < 13; i++)
    {
      uiDrawPathNewFigureWithArc (path, x, y, rad, 0, angle, 0);
      angle += add;
      x += 2 * rad + step;
    }

  y += 2 * rad + step;
  x     = start + rad;
  angle = 0;
  for (i = 0; i < 13; i++)
    {
      uiDrawPathNewFigure (path, x, y);
      uiDrawPathArcTo (path, x, y, rad, 0, angle, 0);
      angle += add;
      x += 2 * rad + step;
    }

  y += 2 * rad + step;
  x     = start + rad;
  angle = 0;
  for (i = 0; i < 13; i++)
    {
      uiDrawPathNewFigureWithArc (path, x, y, rad, uiPi / 4, angle, 0);
      angle += add;
      x += 2 * rad + step;
    }

  y += 2 * rad + step;
  x     = start + rad;
  angle = 0;
  for (i = 0; i < 13; i++)
    {
      uiDrawPathNewFigure (path, x, y);
      uiDrawPathArcTo (path, x, y, rad, uiPi / 4, angle, 0);
      angle += add;
      x += 2 * rad + step;
    }

  y += 2 * rad + step;
  x     = start + rad;
  angle = 0;
  for (i = 0; i < 13; i++)
    {
      uiDrawPathNewFigureWithArc (path, x, y, rad, uiPi + uiPi / 5, angle, 0);
      angle += add;
      x += 2 * rad + step;
    }

  y += 2 * rad + step;
  x     = start + rad;
  angle = 0;
  for (i = 0; i < 13; i++)
    {
      uiDrawPathNewFigure (path, x, y);
      uiDrawPathArcTo (path, x, y, rad, uiPi + uiPi / 5, angle, 0);
      angle += add;
      x += 2 * rad + step;
    }

  uiDrawPathEnd (path);

  brush.Type    = uiDrawBrushTypeSolid;
  brush.R       = 0;
  brush.G       = 0;
  brush.B       = 0;
  brush.A       = 1;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.Thickness  = 1;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, path, &brush, &sp);

  uiDrawFreePath (path);
}

static void
d2dColorToRGB (const uint32_t color, double *r, double *g, double *b)
{

  const uint8_t rr = (color & 0xFF0000) >> 16;
  const uint8_t gg = (color & 0x00FF00) >> 8;
  const uint8_t bb = color & 0x0000FF;
  *r               = (double)rr / 255.0;
  *g               = (double)gg / 255.0;
  *b               = (double)bb / 255.0;
}
#define d2dBlack          0x000000
#define d2dLightSlateGray 0x778899
#define d2dCornflowerBlue 0x6495ED
#define d2dWhite          0xFFFFFF
#define d2dYellowGreen    0x9ACD32
#define d2dYellow         0xFFFF00
#define d2dForestGreen    0x228B22
#define d2dOliveDrab      0x6B8E23
#define d2dLightSkyBlue   0x87CEFA

static void
d2dSolidBrush (uiDrawBrush *brush, const uint32_t color, const double alpha)
{
  brush->Type = uiDrawBrushTypeSolid;
  d2dColorToRGB (color, &brush->R, &brush->G, &brush->B);
  brush->A = alpha;
}

static void
d2dClear (const uiAreaDrawParams *p, const uint32_t color, const double alpha)
{
  uiDrawBrush brush;

  d2dSolidBrush (&brush, color, alpha);
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 0, 0, p->AreaWidth, p->AreaHeight);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &brush);
  uiDrawFreePath (path);
}

static void
drawD2DW8QS (const uiAreaDrawParams *p)
{
  uiDrawBrush brush;

  d2dSolidBrush (&brush, d2dBlack, 1.0);
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 100, 100, p->AreaWidth - 100 - 100, p->AreaHeight - 100 - 100);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &brush);
  uiDrawFreePath (path);
}

static void
drawD2DSimpleApp (const uiAreaDrawParams *p)
{
  uiDrawPath        *path;
  uiDrawBrush        lightSlateGray;
  uiDrawBrush        cornflowerBlue;
  uiDrawStrokeParams sp;

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  d2dSolidBrush (&lightSlateGray, d2dLightSlateGray, 1.0);
  d2dSolidBrush (&cornflowerBlue, d2dCornflowerBlue, 1.0);

  d2dClear (p, d2dWhite, 1.0);

  sp.Thickness  = 0.5;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;

  for (int x = 0; x < p->AreaWidth; x += 10)
    {
      path = uiDrawNewPath (uiDrawFillModeWinding);
      uiDrawPathNewFigure (path, x, 0);
      uiDrawPathLineTo (path, x, p->AreaHeight);
      uiDrawPathEnd (path);
      uiDrawStroke (p->Context, path, &lightSlateGray, &sp);
      uiDrawFreePath (path);
    }

  for (int y = 0; y < p->AreaHeight; y += 10)
    {
      path = uiDrawNewPath (uiDrawFillModeWinding);
      uiDrawPathNewFigure (path, 0, y);
      uiDrawPathLineTo (path, p->AreaWidth, y);
      uiDrawPathEnd (path);
      uiDrawStroke (p->Context, path, &lightSlateGray, &sp);
      uiDrawFreePath (path);
    }

  double left   = p->AreaWidth / 2.0 - 50.0;
  double right  = p->AreaWidth / 2.0 + 50.0;
  double top    = p->AreaHeight / 2.0 - 50.0;
  double bottom = p->AreaHeight / 2.0 + 50.0;
  path          = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, left, top, right - left, bottom - top);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &lightSlateGray);
  uiDrawFreePath (path);

  left   = p->AreaWidth / 2.0 - 100.0;
  right  = p->AreaWidth / 2.0 + 100.0;
  top    = p->AreaHeight / 2.0 - 100.0;
  bottom = p->AreaHeight / 2.0 + 100.0;
  path   = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, left, top, right - left, bottom - top);
  uiDrawPathEnd (path);
  sp.Thickness = 1.0;
  uiDrawStroke (p->Context, path, &cornflowerBlue, &sp);
  uiDrawFreePath (path);
}

static void
drawD2DSolidBrush (const uiAreaDrawParams *p)
{
  uiDrawBrush        black;
  uiDrawBrush        yellowGreen;
  uiDrawStrokeParams sp;

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  d2dSolidBrush (&black, d2dBlack, 1.0);
  d2dSolidBrush (&yellowGreen, d2dYellowGreen, 1.0);

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  // the example doesn't define a rectangle
  // 150x150 seems to be right given the other examples though
  uiDrawPathAddRectangle (path, 25, 25, 150, 150);
  uiDrawPathEnd (path);

  uiDrawFill (p->Context, path, &yellowGreen);
  sp.Thickness  = 1.0;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, path, &black, &sp);

  uiDrawFreePath (path);
}

static void
drawD2DLinearBrush (const uiAreaDrawParams *p)
{
  uiDrawBrush             black;
  uiDrawBrush             gradient;
  uiDrawBrushGradientStop stops[2];
  uiDrawStrokeParams      sp;

  uiDrawMatrix m;

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  // leave some room
  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, 25, 25);
  uiDrawTransform (p->Context, &m);

  gradient.Type = uiDrawBrushTypeLinearGradient;
  gradient.X0   = 0;
  gradient.Y0   = 0;
  gradient.X1   = 150;
  gradient.Y1   = 150;
  stops[0].Pos  = 0.0;

  d2dColorToRGB (d2dYellow, &stops[0].R, &stops[0].G, &stops[0].B);
  stops[0].A   = 1.0;
  stops[1].Pos = 1.0;

  d2dColorToRGB (d2dForestGreen, &stops[1].R, &stops[1].G, &stops[1].B);
  stops[1].A        = 1.0;
  gradient.Stops    = stops;
  gradient.NumStops = 2;

  d2dSolidBrush (&black, d2dBlack, 1.0);

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 0, 0, 150, 150);
  uiDrawPathEnd (path);

  uiDrawFill (p->Context, path, &gradient);
  sp.Thickness  = 1.0;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, path, &black, &sp);

  uiDrawFreePath (path);
}

static void
drawD2DRadialBrush (const uiAreaDrawParams *p)
{
  uiDrawBrush             black;
  uiDrawBrush             gradient;
  uiDrawBrushGradientStop stops[2];
  uiDrawStrokeParams      sp;

  uiDrawMatrix m;

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  // leave some room
  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, 25, 25);
  uiDrawTransform (p->Context, &m);

  gradient.Type        = uiDrawBrushTypeRadialGradient;
  gradient.X0          = 75;
  gradient.Y0          = 75;
  gradient.X1          = 75;
  gradient.Y1          = 75;
  gradient.OuterRadius = 75;
  stops[0].Pos         = 0.0;
  d2dColorToRGB (d2dYellow, &stops[0].R, &stops[0].G, &stops[0].B);
  stops[0].A   = 1.0;
  stops[1].Pos = 1.0;
  d2dColorToRGB (d2dForestGreen, &stops[1].R, &stops[1].G, &stops[1].B);
  stops[1].A        = 1.0;
  gradient.Stops    = stops;
  gradient.NumStops = 2;

  d2dSolidBrush (&black, d2dBlack, 1.0);

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 150, 75);
  uiDrawPathArcTo (path, 75, 75, 75, 0, 2 * uiPi, 0);
  uiDrawPathEnd (path);

  uiDrawFill (p->Context, path, &gradient);
  sp.Thickness  = 1.0;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, path, &black, &sp);

  uiDrawFreePath (path);
}

static void
drawD2DPathGeometries (const uiAreaDrawParams *p)
{
  uiDrawBrush             radial;
  uiDrawBrush             scene;
  uiDrawStrokeParams      sp;
  uiDrawBrushGradientStop stops[2];

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  radial.Type        = uiDrawBrushTypeRadialGradient;
  radial.X0          = 75;
  radial.Y0          = 75;
  radial.X1          = 75;
  radial.Y1          = 75;
  radial.OuterRadius = 75;
  stops[0].Pos       = 0.0;
  d2dColorToRGB (d2dYellow, &stops[0].R, &stops[0].G, &stops[0].B);
  stops[0].A   = 1.0;
  stops[1].Pos = 1.0;
  d2dColorToRGB (d2dForestGreen, &stops[1].R, &stops[1].G, &stops[1].B);
  stops[1].A      = 1.0;
  radial.Stops    = stops;
  radial.NumStops = 2;

  uiDrawPath *leftMountain = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (leftMountain, 346, 255);
  uiDrawPathLineTo (leftMountain, 267, 177);
  uiDrawPathLineTo (leftMountain, 236, 192);
  uiDrawPathLineTo (leftMountain, 212, 160);
  uiDrawPathLineTo (leftMountain, 156, 255);
  uiDrawPathLineTo (leftMountain, 346, 255);
  uiDrawPathCloseFigure (leftMountain);
  uiDrawPathEnd (leftMountain);

  uiDrawPath *rightMountain = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (rightMountain, 575, 263);
  uiDrawPathLineTo (rightMountain, 481, 146);
  uiDrawPathLineTo (rightMountain, 449, 181);
  uiDrawPathLineTo (rightMountain, 433, 159);
  uiDrawPathLineTo (rightMountain, 401, 214);
  uiDrawPathLineTo (rightMountain, 381, 199);
  uiDrawPathLineTo (rightMountain, 323, 263);
  uiDrawPathLineTo (rightMountain, 575, 263);
  uiDrawPathCloseFigure (rightMountain);
  uiDrawPathEnd (rightMountain);

  uiDrawPath *sun = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (sun, (440.0 - 270.0) / 2 + 270.0, 255, 85, uiPi, uiPi, 0);
  uiDrawPathCloseFigure (sun);
  uiDrawPathEnd (sun);

  // the original examples had these as hollow figures
  // we don't support them, so we'll have to stroke it separately
  uiDrawPath *sunRays = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (sunRays, 299, 182);
  uiDrawPathBezierTo (sunRays, 299, 182, 294, 176, 285, 178);
  uiDrawPathBezierTo (sunRays, 276, 179, 272, 173, 272, 173);
  uiDrawPathNewFigure (sunRays, 354, 156);
  uiDrawPathBezierTo (sunRays, 354, 156, 358, 149, 354, 142);
  uiDrawPathBezierTo (sunRays, 349, 134, 354, 127, 354, 127);
  uiDrawPathNewFigure (sunRays, 322, 164);
  uiDrawPathBezierTo (sunRays, 322, 164, 322, 156, 314, 152);
  uiDrawPathBezierTo (sunRays, 306, 149, 305, 141, 305, 141);
  uiDrawPathNewFigure (sunRays, 385, 164);
  uiDrawPathBezierTo (sunRays, 385, 164, 392, 161, 394, 152);
  uiDrawPathBezierTo (sunRays, 395, 144, 402, 141, 402, 142);
  uiDrawPathNewFigure (sunRays, 408, 182);
  uiDrawPathBezierTo (sunRays, 408, 182, 416, 184, 422, 178);
  uiDrawPathBezierTo (sunRays, 428, 171, 435, 173, 435, 173);
  uiDrawPathEnd (sunRays);

  uiDrawPath *river = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (river, 183, 392);
  uiDrawPathBezierTo (river, 238, 284, 472, 345, 356, 303);
  uiDrawPathBezierTo (river, 237, 261, 333, 256, 333, 256);
  uiDrawPathBezierTo (river, 335, 257, 241, 261, 411, 306);
  uiDrawPathBezierTo (river, 574, 350, 288, 324, 296, 392);
  uiDrawPathEnd (river);

  d2dClear (p, d2dWhite, 1.0);

  uiDrawFill (p->Context, sun, &radial);

  d2dSolidBrush (&scene, d2dBlack, 1.0);
  sp.Thickness  = 1.0;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  uiDrawStroke (p->Context, sun, &scene, &sp);
  uiDrawStroke (p->Context, sunRays, &scene, &sp);

  d2dSolidBrush (&scene, d2dOliveDrab, 1.0);
  uiDrawFill (p->Context, leftMountain, &scene);

  d2dSolidBrush (&scene, d2dBlack, 1.0);
  uiDrawStroke (p->Context, leftMountain, &scene, &sp);

  d2dSolidBrush (&scene, d2dLightSkyBlue, 1.0);
  uiDrawFill (p->Context, river, &scene);

  d2dSolidBrush (&scene, d2dBlack, 1.0);
  uiDrawStroke (p->Context, river, &scene, &sp);

  d2dSolidBrush (&scene, d2dYellowGreen, 1.0);
  uiDrawFill (p->Context, rightMountain, &scene);

  d2dSolidBrush (&scene, d2dBlack, 1.0);
  uiDrawStroke (p->Context, rightMountain, &scene, &sp);

  uiDrawFreePath (leftMountain);
  uiDrawFreePath (rightMountain);
  uiDrawFreePath (sun);
  uiDrawFreePath (sunRays);
  uiDrawFreePath (river);
}

static void
drawD2DGeometryGroup (const uiAreaDrawParams *p)
{
  uiDrawBrush        fill;
  uiDrawBrush        stroke;
  uiDrawStrokeParams sp;
  uiDrawMatrix       m;

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  uiDrawPath *alternate = uiDrawNewPath (uiDrawFillModeAlternate);
  uiDrawPathNewFigureWithArc (alternate, 105, 105, 25, 0, 2 * uiPi, 0);
  uiDrawPathNewFigureWithArc (alternate, 105, 105, 50, 0, 2 * uiPi, 0);
  uiDrawPathNewFigureWithArc (alternate, 105, 105, 75, 0, 2 * uiPi, 0);
  uiDrawPathNewFigureWithArc (alternate, 105, 105, 100, 0, 2 * uiPi, 0);
  uiDrawPathEnd (alternate);

  uiDrawPath *winding = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (winding, 105, 105, 25, 0, 2 * uiPi, 0);
  uiDrawPathNewFigureWithArc (winding, 105, 105, 50, 0, 2 * uiPi, 0);
  uiDrawPathNewFigureWithArc (winding, 105, 105, 75, 0, 2 * uiPi, 0);
  uiDrawPathNewFigureWithArc (winding, 105, 105, 100, 0, 2 * uiPi, 0);
  uiDrawPathEnd (winding);

  d2dClear (p, d2dWhite, 1.0);

  d2dSolidBrush (&fill, d2dForestGreen, 1.0);
  d2dSolidBrush (&stroke, d2dCornflowerBlue, 1.0);

  sp.Thickness  = 1.0;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;

  uiDrawFill (p->Context, alternate, &fill);
  uiDrawStroke (p->Context, alternate, &stroke, &sp);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, 300, 0);
  uiDrawTransform (p->Context, &m);
  uiDrawFill (p->Context, winding, &fill);
  uiDrawStroke (p->Context, winding, &stroke, &sp);

  uiDrawFreePath (winding);
  uiDrawFreePath (alternate);
}

static void
drawD2DRotate (const uiAreaDrawParams *p)
{
  uiDrawBrush        original;
  uiDrawBrush        fill;
  uiDrawBrush        transform;
  uiDrawStrokeParams originalsp;
  uiDrawStrokeParams transformsp;
  uiDrawMatrix       m;

  originalsp.Dashes     = NULL;
  originalsp.NumDashes  = 0;
  originalsp.DashPhase  = 0;
  transformsp.Dashes    = NULL;
  transformsp.NumDashes = 0;
  transformsp.DashPhase = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 438.0, 301.5, 498.0 - 438.0, 361.5 - 301.5);
  uiDrawPathEnd (path);

  d2dSolidBrush (&original, d2dBlack, 1.0);
  d2dSolidBrush (&fill, d2dWhite, 0.5);
  d2dSolidBrush (&transform, d2dForestGreen, 1.0);

  originalsp.Thickness   = 1.0;
  originalsp.Cap         = uiDrawLineCapFlat;
  originalsp.Join        = uiDrawLineJoinMiter;
  originalsp.MiterLimit  = uiDrawDefaultMiterLimit;
  transformsp.Thickness  = 1.0;
  transformsp.Cap        = uiDrawLineCapFlat;
  transformsp.Join       = uiDrawLineJoinMiter;
  transformsp.MiterLimit = uiDrawDefaultMiterLimit;

  // save for when we do the translated one
  uiDrawSave (p->Context);

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixRotate (&m, 468.0, 331.5, 45.0 * (uiPi / 180));
  uiDrawTransform (p->Context, &m);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawRestore (p->Context);

  // translate to test the corner axis
  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, -200, -200);
  uiDrawTransform (p->Context, &m);

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixRotate (&m, 438.0, 301.5, 45.0 * (uiPi / 180));
  uiDrawTransform (p->Context, &m);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawFreePath (path);
}

static void
drawD2DScale (const uiAreaDrawParams *p)
{
  uiDrawBrush        original;
  uiDrawBrush        fill;
  uiDrawBrush        transform;
  uiDrawStrokeParams originalsp;
  uiDrawStrokeParams transformsp;
  uiDrawMatrix       m;

  originalsp.Dashes     = NULL;
  originalsp.NumDashes  = 0;
  originalsp.DashPhase  = 0;
  transformsp.Dashes    = NULL;
  transformsp.NumDashes = 0;
  transformsp.DashPhase = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 438.0, 80.5, 498.0 - 438.0, 140.5 - 80.5);
  uiDrawPathEnd (path);

  d2dSolidBrush (&original, d2dBlack, 1.0);
  d2dSolidBrush (&fill, d2dWhite, 0.5);
  d2dSolidBrush (&transform, d2dForestGreen, 1.0);

  originalsp.Thickness   = 1.0;
  originalsp.Cap         = uiDrawLineCapFlat;
  originalsp.Join        = uiDrawLineJoinMiter;
  originalsp.MiterLimit  = uiDrawDefaultMiterLimit;
  transformsp.Thickness  = 1.0;
  transformsp.Cap        = uiDrawLineCapFlat;
  transformsp.Join       = uiDrawLineJoinMiter;
  transformsp.MiterLimit = uiDrawDefaultMiterLimit;

  // save for when we do the translated one
  uiDrawSave (p->Context);

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixScale (&m, 438.0, 80.5, 1.3, 1.3);
  uiDrawTransform (p->Context, &m);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawRestore (p->Context);

  // for testing purposes, show what happens if we scale about (0, 0)
  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, -300, 50);
  uiDrawTransform (p->Context, &m);

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixScale (&m, 0, 0, 1.3, 1.3);
  uiDrawTransform (p->Context, &m);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawFreePath (path);
}

void
drawD2DSkew (const uiAreaDrawParams *p)
{
  uiDrawBrush        original;
  uiDrawBrush        fill;
  uiDrawBrush        transform;
  uiDrawStrokeParams originalsp;
  uiDrawStrokeParams transformsp;
  uiDrawMatrix       m;

  originalsp.Dashes     = NULL;
  originalsp.NumDashes  = 0;
  originalsp.DashPhase  = 0;
  transformsp.Dashes    = NULL;
  transformsp.NumDashes = 0;
  transformsp.DashPhase = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 126.0, 301.5, 186.0 - 126.0, 361.5 - 301.5);
  uiDrawPathEnd (path);

  d2dSolidBrush (&original, d2dBlack, 1.0);
  d2dSolidBrush (&fill, d2dWhite, 0.5);
  d2dSolidBrush (&transform, d2dForestGreen, 1.0);

  originalsp.Thickness   = 1.0;
  originalsp.Cap         = uiDrawLineCapFlat;
  originalsp.Join        = uiDrawLineJoinMiter;
  originalsp.MiterLimit  = uiDrawDefaultMiterLimit;
  transformsp.Thickness  = 1.0;
  transformsp.Cap        = uiDrawLineCapFlat;
  transformsp.Join       = uiDrawLineJoinMiter;
  transformsp.MiterLimit = uiDrawDefaultMiterLimit;

  // save for when we do the translated one
  uiDrawSave (p->Context);

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixSkew (&m, 126.0, 301.5, 45.0 * (uiPi / 180), 0);
  uiDrawTransform (p->Context, &m);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawRestore (p->Context);

  // for testing purposes, show what happens if we skew about (0, 0)
  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, 0, -200);
  uiDrawTransform (p->Context, &m);

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixSkew (&m, 0, 0, 45.0 * (uiPi / 180), 0);
  uiDrawTransform (p->Context, &m);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawFreePath (path);
}

static void
drawD2DTranslate (const uiAreaDrawParams *p)
{
  uiDrawBrush        original;
  uiDrawBrush        fill;
  uiDrawBrush        transform;
  uiDrawStrokeParams originalsp;
  uiDrawStrokeParams transformsp;
  uiDrawMatrix       m;

  originalsp.Dashes     = NULL;
  originalsp.NumDashes  = 0;
  originalsp.DashPhase  = 0;
  transformsp.Dashes    = NULL;
  transformsp.NumDashes = 0;
  transformsp.DashPhase = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 126.0, 80.5, 186.0 - 126.0, 140.5 - 80.5);
  uiDrawPathEnd (path);

  d2dSolidBrush (&original, d2dBlack, 1.0);
  d2dSolidBrush (&fill, d2dWhite, 0.5);
  d2dSolidBrush (&transform, d2dForestGreen, 1.0);

  originalsp.Thickness   = 1.0;
  originalsp.Cap         = uiDrawLineCapFlat;
  originalsp.Join        = uiDrawLineJoinMiter;
  originalsp.MiterLimit  = uiDrawDefaultMiterLimit;
  transformsp.Thickness  = 1.0;
  transformsp.Cap        = uiDrawLineCapFlat;
  transformsp.Join       = uiDrawLineJoinMiter;
  transformsp.MiterLimit = uiDrawDefaultMiterLimit;

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, 20, 10);
  uiDrawTransform (p->Context, &m);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawFreePath (path);
}

static void
drawD2DMultiTransforms (const uiAreaDrawParams *p)
{
  uiDrawBrush        original;
  uiDrawBrush        fill;
  uiDrawBrush        transform;
  uiDrawStrokeParams originalsp;
  uiDrawStrokeParams transformsp;
  uiDrawMatrix       mtranslate;
  uiDrawMatrix       mrotate;

  originalsp.Dashes     = NULL;
  originalsp.NumDashes  = 0;
  originalsp.DashPhase  = 0;
  transformsp.Dashes    = NULL;
  transformsp.NumDashes = 0;
  transformsp.DashPhase = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 300.0, 40.0, 360.0 - 300.0, 100.0 - 40.0);
  uiDrawPathEnd (path);

  d2dSolidBrush (&original, d2dBlack, 1.0);
  d2dSolidBrush (&fill, d2dWhite, 0.5);
  d2dSolidBrush (&transform, d2dForestGreen, 1.0);

  originalsp.Thickness   = 1.0;
  originalsp.Cap         = uiDrawLineCapFlat;
  originalsp.Join        = uiDrawLineJoinMiter;
  originalsp.MiterLimit  = uiDrawDefaultMiterLimit;
  transformsp.Thickness  = 1.0;
  transformsp.Cap        = uiDrawLineCapFlat;
  transformsp.Join       = uiDrawLineJoinMiter;
  transformsp.MiterLimit = uiDrawDefaultMiterLimit;

  uiDrawMatrixSetIdentity (&mtranslate);
  uiDrawMatrixTranslate (&mtranslate, 20.0, 10.0);
  uiDrawMatrixSetIdentity (&mrotate);
  uiDrawMatrixRotate (&mrotate, 330.0, 70.0, 45.0 * (uiPi / 180));

  // save for when we do the opposite one
  uiDrawSave (p->Context);

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawTransform (p->Context, &mrotate);
  uiDrawTransform (p->Context, &mtranslate);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawRestore (p->Context);
  uiDrawFreePath (path);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 40.0, 40.0, 100.0 - 40.0, 100.0 - 40.0);
  uiDrawPathEnd (path);

  uiDrawMatrixSetIdentity (&mtranslate);
  uiDrawMatrixTranslate (&mtranslate, 20.0, 10.0);
  uiDrawMatrixSetIdentity (&mrotate);
  uiDrawMatrixRotate (&mrotate, 70.0, 70.0, 45.0 * (uiPi / 180));

  uiDrawStroke (p->Context, path, &original, &originalsp);

  uiDrawTransform (p->Context, &mtranslate);
  uiDrawTransform (p->Context, &mrotate);

  uiDrawFill (p->Context, path, &fill);
  uiDrawStroke (p->Context, path, &transform, &transformsp);

  uiDrawFreePath (path);
}

static void
drawD2DComplexShape (const uiAreaDrawParams *p)
{
  uiDrawBrush             black;
  uiDrawBrush             gradient;
  uiDrawBrushGradientStop stops[2];
  uiDrawStrokeParams      sp;
  uiDrawMatrix            m;

  sp.Dashes    = NULL;
  sp.NumDashes = 0;
  sp.DashPhase = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 0, 0);
  uiDrawPathLineTo (path, 200, 0);
  uiDrawPathBezierTo (path, 150, 50, 150, 150, 200, 200);
  uiDrawPathLineTo (path, 0, 200);
  uiDrawPathBezierTo (path, 50, 150, 50, 50, 0, 0);
  uiDrawPathCloseFigure (path);
  uiDrawPathEnd (path);

  d2dSolidBrush (&black, d2dBlack, 1.0);

  stops[0].Pos      = 0.0;
  stops[0].R        = 0.0;
  stops[0].G        = 1.0;
  stops[0].B        = 1.0;
  stops[0].A        = 0.25;
  stops[1].Pos      = 1.0;
  stops[1].R        = 0.0;
  stops[1].G        = 0.0;
  stops[1].B        = 1.0;
  stops[1].A        = 1.0;
  gradient.Type     = uiDrawBrushTypeLinearGradient;
  gradient.X0       = 100;
  gradient.Y0       = 0;
  gradient.X1       = 100;
  gradient.Y1       = 200;
  gradient.Stops    = stops;
  gradient.NumStops = 2;

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, 20, 20);
  uiDrawTransform (p->Context, &m);

  sp.Thickness  = 10.0;
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;

  uiDrawStroke (p->Context, path, &black, &sp);
  uiDrawFill (p->Context, path, &gradient);

  uiDrawFreePath (path);
}

static void
crsourcergba (uiDrawBrush *brush, const double r, const double g, const double b, const double a)
{
  brush->Type = uiDrawBrushTypeSolid;
  brush->R    = r;
  brush->G    = g;
  brush->B    = b;
  brush->A    = a;
}

static void
drawCSArc (const uiAreaDrawParams *p)
{
  const double       xc     = 128.0;
  const double       yc     = 128.0;
  const double       radius = 100.0;
  const double       angle1 = 45.0 * (uiPi / 180.0);
  const double       angle2 = 180.0 * (uiPi / 180.0);
  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  sp.Thickness     = 10.0;
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (path, xc, yc, radius, angle1, angle2 - angle1, 0);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  crsourcergba (&source, 1, 0.2, 0.2, 0.6);
  sp.Thickness = 6.0;

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (path, xc, yc, 10.0, 0, 2 * uiPi, 0);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &source);
  uiDrawFreePath (path);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (path, xc, yc, radius, angle1, 0, 0);
  uiDrawPathLineTo (path, xc, yc);
  uiDrawPathNewFigureWithArc (path, xc, yc, radius, angle2, 0, 0);
  uiDrawPathLineTo (path, xc, yc);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSArcNegative (const uiAreaDrawParams *p)
{
  const double       xc     = 128.0;
  const double       yc     = 128.0;
  const double       radius = 100.0;
  const double       angle1 = 45.0 * (uiPi / 180.0);
  const double       angle2 = 180.0 * (uiPi / 180.0);
  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  sp.Thickness     = 10.0;
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (path, xc, yc, radius, angle1, angle2 - angle1, 1);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  crsourcergba (&source, 1, 0.2, 0.2, 0.6);
  sp.Thickness = 6.0;

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (path, xc, yc, 10.0, 0, 2 * uiPi, 0);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &source);
  uiDrawFreePath (path);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigureWithArc (path, xc, yc, radius, angle1, 0, 0);
  uiDrawPathLineTo (path, xc, yc);
  uiDrawPathNewFigureWithArc (path, xc, yc, radius, angle2, 0, 0);
  uiDrawPathLineTo (path, xc, yc);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSClip (const uiAreaDrawParams *p)
{
  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);

  uiDrawPathNewFigureWithArc (path, 128.0, 128.0, 76.8, 0, 2 * uiPi, 0);
  uiDrawPathEnd (path);
  uiDrawClip (p->Context, path);
  uiDrawFreePath (path);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 0, 0, 256, 256);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &source);
  uiDrawFreePath (path);

  crsourcergba (&source, 0, 1, 0, 1);
  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 0, 0);
  uiDrawPathLineTo (path, 256, 256);
  uiDrawPathNewFigure (path, 256, 0);
  uiDrawPathLineTo (path, 0, 256);
  uiDrawPathEnd (path);
  sp.Thickness = 10.0;
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSCurveRectangle (const uiAreaDrawParams *p)
{
  const double x0          = 25.6;
  const double y0          = 25.6;
  const double rect_width  = 204.8;
  const double rect_height = 204.8;
  const double radius      = 102.4;

  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);

  const double x1 = x0 + rect_width;
  const double y1 = y0 + rect_height;

  uiDrawPathNewFigure (path, x0, y0 + radius);
  uiDrawPathBezierTo (path, x0, y0, x0, y0, x0 + radius, y0);
  uiDrawPathLineTo (path, x1 - radius, y0);
  uiDrawPathBezierTo (path, x1, y0, x1, y0, x1, y0 + radius);
  uiDrawPathLineTo (path, x1, y1 - radius);
  uiDrawPathBezierTo (path, x1, y1, x1, y1, x1 - radius, y1);
  uiDrawPathLineTo (path, x0 + radius, y1);
  uiDrawPathBezierTo (path, x0, y1, x0, y1, x0, y1 - radius);
  uiDrawPathCloseFigure (path);
  uiDrawPathEnd (path);

  crsourcergba (&source, 0.5, 0.5, 1, 1.0);
  uiDrawFill (p->Context, path, &source);
  crsourcergba (&source, 0.5, 0, 0, 0.5);
  sp.Thickness = 10.0;
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

// curve to
static void
drawCSCurveTo (const uiAreaDrawParams *p)
{
  const double x  = 25.6;
  const double y  = 128.0;
  const double x1 = 102.4;
  const double y1 = 230.4;
  const double x2 = 153.6;
  const double y2 = 25.6;
  const double x3 = 230.4;
  const double y3 = 128.0;

  uiDrawBrush source;

  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);

  uiDrawPathNewFigure (path, x, y);
  uiDrawPathBezierTo (path, x1, y1, x2, y2, x3, y3);
  uiDrawPathEnd (path);
  sp.Thickness = 10.0;
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  crsourcergba (&source, 1, 0.2, 0.2, 0.6);
  sp.Thickness = 6.0;
  path         = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, x, y);
  uiDrawPathLineTo (path, x1, y1);
  uiDrawPathNewFigure (path, x2, y2);
  uiDrawPathLineTo (path, x3, y3);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSDash (const uiAreaDrawParams *p)
{
  double dashes[] = {
    50.0, /* ink */
    10.0, /* skip */
    10.0, /* ink */
    10.0  /* skip*/
  };
  const int    ndash  = sizeof (dashes) / sizeof (dashes[0]);
  const double offset = -50.0;

  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = dashes;
  sp.NumDashes  = ndash;
  sp.DashPhase  = offset;
  sp.Thickness  = 10.0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 128.0, 25.6);
  uiDrawPathLineTo (path, 230.4, 230.4);
  uiDrawPathLineTo (path, 230.4 - 102.4, 230.4 + 0.0);
  uiDrawPathBezierTo (path, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);
  uiDrawPathEnd (path);

  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSFillAndStroke2 (const uiAreaDrawParams *p)
{
  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);

  uiDrawPathNewFigure (path, 128.0, 25.6);
  uiDrawPathLineTo (path, 230.4, 230.4);
  uiDrawPathLineTo (path, 230.4 - 102.4, 230.4 + 0.0);
  uiDrawPathBezierTo (path, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);
  uiDrawPathCloseFigure (path);

  uiDrawPathNewFigure (path, 64.0, 25.6);
  uiDrawPathLineTo (path, 64.0 + 51.2, 25.6 + 51.2);
  uiDrawPathLineTo (path, 64.0 + 51.2 - 51.2, 25.6 + 51.2 + 51.2);
  uiDrawPathLineTo (path, 64.0 + 51.2 - 51.2 - 51.2, 25.6 + 51.2 + 51.2 - 51.2);
  uiDrawPathCloseFigure (path);

  uiDrawPathEnd (path);

  sp.Thickness = 10.0;
  crsourcergba (&source, 0, 0, 1, 1);
  uiDrawFill (p->Context, path, &source);
  crsourcergba (&source, 0, 0, 0, 1);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSFillStyle (const uiAreaDrawParams *p)
{
  uiDrawBrush        source;
  uiDrawStrokeParams sp;
  uiDrawMatrix       m;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  sp.Thickness = 6;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeAlternate);
  uiDrawPathAddRectangle (path, 12, 12, 232, 70);
  uiDrawPathNewFigureWithArc (path, 64, 64, 40, 0, 2 * uiPi, 0);
  uiDrawPathNewFigureWithArc (path, 192, 64, 40, 0, -2 * uiPi, 1);
  uiDrawPathEnd (path);

  crsourcergba (&source, 0, 0.7, 0, 1);
  uiDrawFill (p->Context, path, &source);
  crsourcergba (&source, 0, 0, 0, 1);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  uiDrawMatrixSetIdentity (&m);
  uiDrawMatrixTranslate (&m, 0, 128);
  uiDrawTransform (p->Context, &m);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathAddRectangle (path, 12, 12, 232, 70);
  uiDrawPathNewFigureWithArc (path, 64, 64, 40, 0, 2 * uiPi, 0);
  uiDrawPathNewFigureWithArc (path, 192, 64, 40, 0, -2 * uiPi, 1);
  uiDrawPathEnd (path);

  crsourcergba (&source, 0, 0, 0.9, 1);
  uiDrawFill (p->Context, path, &source);
  crsourcergba (&source, 0, 0, 0, 1);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSMultiCaps (const uiAreaDrawParams *p)
{
  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);

  uiDrawPathNewFigure (path, 50.0, 75.0);
  uiDrawPathLineTo (path, 200.0, 75.0);

  uiDrawPathNewFigure (path, 50.0, 125.0);
  uiDrawPathLineTo (path, 200.0, 125.0);

  uiDrawPathNewFigure (path, 50.0, 175.0);
  uiDrawPathLineTo (path, 200.0, 175.0);
  uiDrawPathEnd (path);

  sp.Thickness = 30.0;
  sp.Cap       = uiDrawLineCapRound;
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSRoundRect (const uiAreaDrawParams *p)
{
  const double x      = 25.6;
  const double y      = 25.6;
  const double width  = 204.8;
  const double height = 204.8;
  const double aspect = 1.0;

  const double corner_radius = height / 10.0;
  const double radius        = corner_radius / aspect;
  const double degrees       = uiPi / 180.0;

  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);

  // top right corner
  uiDrawPathNewFigureWithArc (path, x + width - radius, y + radius, radius, -90 * degrees, uiPi / 2, 0);

  // bottom right corner
  uiDrawPathArcTo (path, x + width - radius, y + height - radius, radius, 0 * degrees, uiPi / 2, 0);

  // bottom left corner
  uiDrawPathArcTo (path, x + radius, y + height - radius, radius, 90 * degrees, uiPi / 2, 0);

  // top left corner
  uiDrawPathArcTo (path, x + radius, y + radius, radius, 180 * degrees, uiPi / 2, 0);
  uiDrawPathCloseFigure (path);
  uiDrawPathEnd (path);

  crsourcergba (&source, 0.5, 0.5, 1, 1);
  uiDrawFill (p->Context, path, &source);
  crsourcergba (&source, 0.5, 0, 0, 0.5);
  sp.Thickness = 10.0;
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSSetLineCap (const uiAreaDrawParams *p)
{
  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  sp.Thickness = 30.0;

  sp.Cap           = uiDrawLineCapFlat;
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 64.0, 50.0);
  uiDrawPathLineTo (path, 64.0, 200.0);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  sp.Cap = uiDrawLineCapRound;
  path   = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 128.0, 50.0);
  uiDrawPathLineTo (path, 128.0, 200.0);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  sp.Cap = uiDrawLineCapSquare;
  path   = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 192.0, 50.0);
  uiDrawPathLineTo (path, 192.0, 200.0);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  crsourcergba (&source, 1, 0.2, 0.2, 1);
  sp.Thickness = 2.56;
  path         = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 64.0, 50.0);
  uiDrawPathLineTo (path, 64.0, 200.0);
  uiDrawPathNewFigure (path, 128.0, 50.0);
  uiDrawPathLineTo (path, 128.0, 200.0);
  uiDrawPathNewFigure (path, 192.0, 50.0);
  uiDrawPathLineTo (path, 192.0, 200.0);
  uiDrawPathEnd (path);
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
drawCSSetLineJoin (const uiAreaDrawParams *p)
{
  uiDrawBrush        source;
  uiDrawStrokeParams sp;

  crsourcergba (&source, 0, 0, 0, 1);
  sp.Cap        = uiDrawLineCapFlat;
  sp.Join       = uiDrawLineJoinMiter;
  sp.MiterLimit = uiDrawDefaultMiterLimit;
  sp.Dashes     = NULL;
  sp.NumDashes  = 0;
  sp.DashPhase  = 0;

  sp.Thickness     = 40.96;
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 76.8, 84.48);
  uiDrawPathLineTo (path, 76.8 + 51.2, 84.48 - 51.2);
  uiDrawPathLineTo (path, 76.8 + 51.2 + 51.2, 84.48 - 51.2 + 51.2);
  uiDrawPathEnd (path);
  sp.Join = uiDrawLineJoinMiter;
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 76.8, 161.28);
  uiDrawPathLineTo (path, 76.8 + 51.2, 161.28 - 51.2);
  uiDrawPathLineTo (path, 76.8 + 51.2 + 51.2, 161.28 - 51.2 + 51.2);
  uiDrawPathEnd (path);
  sp.Join = uiDrawLineJoinBevel;
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);

  path = uiDrawNewPath (uiDrawFillModeWinding);
  uiDrawPathNewFigure (path, 76.8, 238.08);
  uiDrawPathLineTo (path, 76.8 + 51.2, 238.08 - 51.2);
  uiDrawPathLineTo (path, 76.8 + 51.2 + 51.2, 238.08 - 51.2 + 51.2);
  uiDrawPathEnd (path);
  sp.Join = uiDrawLineJoinRound;
  uiDrawStroke (p->Context, path, &source, &sp);
  uiDrawFreePath (path);
}

static void
cgaddrect (uiDrawPath *path, const uiAreaDrawParams *p, const double x, const double y, const double width,
           const double height)
{
  uiDrawPathAddRectangle (path, x, p->AreaHeight - y - height, width, height);
}

static void
drawQ2DCreateWindowGC (const uiAreaDrawParams *p)
{
  uiDrawBrush brush;

  crsourcergba (&brush, 1, 0, 0, 1);
  uiDrawPath *path = uiDrawNewPath (uiDrawFillModeWinding);
  cgaddrect (path, p, 0, 0, 200, 100);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &brush);
  uiDrawFreePath (path);

  crsourcergba (&brush, 0, 0, 1, .5);
  path = uiDrawNewPath (uiDrawFillModeWinding);
  cgaddrect (path, p, 0, 0, 100, 200);
  uiDrawPathEnd (path);
  uiDrawFill (p->Context, path, &brush);
  uiDrawFreePath (path);
}

static const struct drawtest tests[] = {
  { "Original uiArea test",                                         drawOriginal           },
  { "Arc test",                                                     drawArcs               },
  { "Direct2D: Direct2D Quickstart for Windows 8",                  drawD2DW8QS            },
  { "Direct2D: Creating a Simple Direct2D Application",             drawD2DSimpleApp       },
  { "Direct2D: How to Create a Solid Color Brush",                  drawD2DSolidBrush      },
  { "Direct2D: How to Create a Linear Gradient Brush",              drawD2DLinearBrush     },
  { "Direct2D: How to Create a Radial Gradient Brush",              drawD2DRadialBrush     },
  { "Direct2D: Path Geometries Overview",                           drawD2DPathGeometries  },
  { "Direct2D: How to Create Geometry Groups",                      drawD2DGeometryGroup   },
  { "Direct2D: How to Rotate an Object",                            drawD2DRotate          },
  { "Direct2D: How to Scale an Object",                             drawD2DScale           },
  { "Direct2D: How to Skew an Object",                              drawD2DSkew            },
  { "Direct2D: How to Translate an Object",                         drawD2DTranslate       },
  { "Direct2D: How to Apply Multiple Transforms to an Object",      drawD2DMultiTransforms },
  { "Direct2D: How to Draw and Fill a Complex Shape",               drawD2DComplexShape    },
  { "cairo samples: arc",                                           drawCSArc              },
  { "cairo samples: arc negative",                                  drawCSArcNegative      },
  { "cairo samples: clip",                                          drawCSClip             },
  { "cairo samples: curve rectangle",                               drawCSCurveRectangle   },
  { "cairo samples: curve to",                                      drawCSCurveTo          },
  { "cairo samples: dash",                                          drawCSDash             },
  { "cairo samples: fill and stroke2",                              drawCSFillAndStroke2   },
  { "cairo samples: fill style",                                    drawCSFillStyle        },
  { "cairo samples: multi segment caps",                            drawCSMultiCaps        },
  { "cairo samples: rounded rectangle",                             drawCSRoundRect        },
  { "cairo samples: set line cap",                                  drawCSSetLineCap       },
  { "cairo samples: set line join",                                 drawCSSetLineJoin      },
  { "Quartz 2D PG: Creating a Window Graphics Context in Mac OS X", drawQ2DCreateWindowGC  },
  { NULL,                                                           NULL                   },
};

void
runDrawTest (const int n, const uiAreaDrawParams *p)
{
  (*tests[n].draw) (p);
}

void
populateComboboxWithTests (uiCombobox *c)
{
  for (size_t i = 0; tests[i].name != NULL; i++)
    uiComboboxAppend (c, tests[i].name);
}
