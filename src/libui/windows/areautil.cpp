#include "areautil.h"
#include "sizing.h"

void
loadAreaSize (const uiArea *a, ID2D1RenderTarget *rt, double *width, double *height)
{
  *width  = 0;
  *height = 0;

  if (a->scrolling == 0)
    {
      if (rt == NULL)
        rt = a->rt;

      const D2D1_SIZE_F size = realGetSize (rt);
      *width                 = size.width;
      *height                = size.height;
    }
}

void
pixelsToDIP (const uiArea *a, double *x, double *y)
{
  FLOAT dpix;
  FLOAT dpiy;

  a->rt->GetDpi (&dpix, &dpiy);

  *x = *x * 96 / dpix;
  *y = *y * 96 / dpiy;
}

void
dipToPixels (const uiArea *a, double *x, double *y)
{
  FLOAT dpix;
  FLOAT dpiy;

  a->rt->GetDpi (&dpix, &dpiy);

  *x = *x * dpix / 96;
  *y = *y * dpiy / 96;
}
