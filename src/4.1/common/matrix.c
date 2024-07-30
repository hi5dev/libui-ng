#include "uipriv.h"

#include <math.h>

void
uiDrawMatrixSetIdentity (uiDrawMatrix *m)
{
  m->M11 = 1;
  m->M12 = 0;
  m->M21 = 0;
  m->M22 = 1;
  m->M31 = 0;
  m->M32 = 0;
}

void
uiprivFallbackSkew (uiDrawMatrix *m, const double x, const double y, const double xamount, const double yamount)
{
  uiDrawMatrix n;

  uiDrawMatrixSetIdentity (&n);

  n.M12 = tan (yamount);
  n.M21 = tan (xamount);
  n.M31 = -y * tan (xamount);
  n.M32 = -x * tan (yamount);

  uiDrawMatrixMultiply (m, &n);
}

void
uiprivFallbackTransformSize (const uiDrawMatrix *m, double *x, double *y)
{
  uiDrawMatrix m2;

  m2     = *m;
  m2.M31 = 0;
  m2.M32 = 0;

  uiDrawMatrixTransformPoint (&m2, x, y);
}
