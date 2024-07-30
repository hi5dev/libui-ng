#include "draw.h"

#include <uipriv.h>

void
m2d (const uiDrawMatrix *m, D2D1_MATRIX_3X2_F *d)
{
  d->_11 = m->M11; // NOLINT(*-narrowing-conversions)
  d->_12 = m->M12; // NOLINT(*-narrowing-conversions)
  d->_21 = m->M21; // NOLINT(*-narrowing-conversions)
  d->_22 = m->M22; // NOLINT(*-narrowing-conversions)
  d->_31 = m->M31; // NOLINT(*-narrowing-conversions)
  d->_32 = m->M32; // NOLINT(*-narrowing-conversions)
}

static void
d2m (const D2D1_MATRIX_3X2_F *d, uiDrawMatrix *m)
{
  m->M11 = d->_11;
  m->M12 = d->_12;
  m->M21 = d->_21;
  m->M22 = d->_22;
  m->M31 = d->_31;
  m->M32 = d->_32;
}

void
uiDrawMatrixTranslate (uiDrawMatrix *m, const double x, const double y)
{
  D2D1_MATRIX_3X2_F dm;

  m2d (m, &dm);

  dm = dm * D2D1::Matrix3x2F::Translation (x, y); // NOLINT(*-narrowing-conversions)

  d2m (&dm, m);
}

void
uiDrawMatrixScale (uiDrawMatrix *m, const double xCenter, const double yCenter, const double x, const double y)
{
  D2D1_MATRIX_3X2_F dm;
  D2D1_POINT_2F     center;

  m2d (m, &dm);
  center.x = xCenter;                                     // NOLINT(*-narrowing-conversions)
  center.y = yCenter;                                     // NOLINT(*-narrowing-conversions)
  dm       = dm * D2D1::Matrix3x2F::Scale (x, y, center); // NOLINT(*-narrowing-conversions)
  d2m (&dm, m);
}

#define r2d(x) (x * (180.0 / uiPi))

void
uiDrawMatrixRotate (uiDrawMatrix *m, const double x, const double y, const double amount)
{
  D2D1_MATRIX_3X2_F dm;
  D2D1_POINT_2F     center;

  m2d (m, &dm);
  center.x = x; // NOLINT(*-narrowing-conversions)
  center.y = y; // NOLINT(*-narrowing-conversions)
  dm       = dm * D2D1::Matrix3x2F::Rotation (r2d (amount), center);
  d2m (&dm, m);
}

void
uiDrawMatrixSkew (uiDrawMatrix *m, const double x, const double y, const double xamount, const double yamount)
{
  D2D1_MATRIX_3X2_F dm;
  D2D1_POINT_2F     center;

  m2d (m, &dm);
  center.x = x; // NOLINT(*-narrowing-conversions)
  center.y = y; // NOLINT(*-narrowing-conversions)
  dm       = dm * D2D1::Matrix3x2F::Skew (r2d (xamount), r2d (yamount), center);
  d2m (&dm, m);
}

void
uiDrawMatrixMultiply (uiDrawMatrix *dest, const uiDrawMatrix *src)
{
  D2D1_MATRIX_3X2_F c;
  m2d (dest, &c);

  D2D1_MATRIX_3X2_F d;
  m2d (src, &d);

  c = c * d;

  d2m (&c, dest);
}

int
uiDrawMatrixInvertible (const uiDrawMatrix *m)
{
  D2D1_MATRIX_3X2_F d;

  m2d (m, &d);

  return D2D1IsMatrixInvertible (&d) != FALSE; // NOLINT(*-implicit-bool-conversion)
}

int
uiDrawMatrixInvert (uiDrawMatrix *m)
{
  D2D1_MATRIX_3X2_F d;

  m2d (m, &d);
  if (D2D1InvertMatrix (&d) == FALSE)
    return 0;
  d2m (&d, m);
  return 1;
}

void
uiDrawMatrixTransformPoint (const uiDrawMatrix *m, double *x, double *y)
{
  D2D1::Matrix3x2F dm;
  D2D1_POINT_2F    pt;

  m2d (m, &dm);
  pt.x = *x; // NOLINT(*-narrowing-conversions)
  pt.y = *y; // NOLINT(*-narrowing-conversions)
  pt   = dm.TransformPoint (pt);
  *x   = pt.x;
  *y   = pt.y;
}

void
uiDrawMatrixTransformSize (const uiDrawMatrix *m, double *x, double *y)
{
  uiprivFallbackTransformSize (m, x, y);
}
