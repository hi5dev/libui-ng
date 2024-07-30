#include <windows.h>

#include <d2d1.h>

#include <ui_win32.h>
#include <uipriv.h>

#include "colordialog.h"
#include "d2dscratch.h"
#include "debug.h"
#include "init.h"
#include "resources.hpp"
#include "sizing.h"
#include "text.h"
#include "uipriv_windows.hpp"
#include "utf16.h"
#include "winpublic.h"
#include "winutil.h"

#include <algorithm>
#include <cmath>
#include <commctrl.h>
#include <cstdint>
#include <map>

static void
rgb2HSV (const double r, const double g, const double b, double *h, double *s, double *v)
{
  double M = r;

  int whichmax = 0;

  if (M < g)
    {
      M        = g;
      whichmax = 1;
    }

  if (M < b)
    {
      M        = b;
      whichmax = 2;
    }

  double m = r;

  m = std::min (m, g);
  m = std::min (m, b);

  const double c = M - m;

  if (c == 0)
    {
      *h = 0;
    }

  else
    {
      switch (whichmax)
        {
        case 0:
          {
            *h = (g - b) / c;
            *h = std::fmod (*h, 6);
            break;
          }

        case 1:
          {
            *h = (b - r) / c + 2;
            break;
          }

        case 2:
          {
            *h = (r - g) / c + 4;
            break;
          }

        default:;
        }

      // put in range [0,1)
      *h /= 6;
    }

  *v = M;

  if (c == 0)
    *s = 0;
  else
    *s = c / *v;
}

static void
hsv2RGB (const double h, const double s, const double v, double *r, double *g, double *b)
{
  const double c = v * s;

  const double hPrime = h * 6;

  // equivalent to splitting into 60° chunks
  const int h60 = static_cast<int> (hPrime);

  const double x = c * (1.0 - fabs (fmod (hPrime, 2) - 1.0));

  const double m = v - c;

  switch (h60)
    {
    case 0:
      {
        *r = c + m;
        *g = x + m;
        *b = m;
        return;
      }

    case 1:
      {
        *r = x + m;
        *g = c + m;
        *b = m;
        return;
      }

    case 2:
      {
        *r = m;
        *g = c + m;
        *b = x + m;
        return;
      }

    case 3:
      {
        *r = m;
        *g = x + m;
        *b = c + m;
        return;
      }

    case 4:
      {
        *r = x + m;
        *g = m;
        *b = c + m;
        return;
      }

    case 5:
      {
        *r = c + m;
        *g = m;
        *b = x + m;
      }

    default:;
    }
}

static void
rgba2Hex (const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a, WCHAR *buf)
{
  buf[0] = L'#';
  buf[1] = hexd[a >> 4 & 0xF];
  buf[2] = hexd[a & 0xF];
  buf[3] = hexd[r >> 4 & 0xF];
  buf[4] = hexd[r & 0xF];
  buf[5] = hexd[g >> 4 & 0xF];
  buf[6] = hexd[g & 0xF];
  buf[7] = hexd[b >> 4 & 0xF];
  buf[8] = hexd[b & 0xF];
  buf[9] = L'\0';
}

static int
convHexDigit (const WCHAR c)
{
  if (c >= L'0' && c <= L'9')
    return c - L'0';

  if (c >= L'A' && c <= L'F')
    return c - L'A' + 0xA;

  if (c >= L'a' && c <= L'f')
    return c - L'a' + 0xA;

  return -1;
}

static BOOL
hex2RGBA (const WCHAR *buf, double *r, double *g, double *b, double *a)
{
  if (*buf == L'#')
    buf++;

  uint8_t component = 0;

  int i = convHexDigit (*buf++);

  if (i < 0)
    return FALSE;

  component |= static_cast<uint8_t> (i) << 4;

  i = convHexDigit (*buf++);
  if (i < 0)
    return FALSE;

  component |= static_cast<uint8_t> (i);

  *a = static_cast<double> (component) / 255;

  component = 0;

  i = convHexDigit (*buf++);
  if (i < 0)
    return FALSE;

  component |= static_cast<uint8_t> (i) << 4;

  i = convHexDigit (*buf++);
  if (i < 0)
    return FALSE;

  component |= static_cast<uint8_t> (i);

  *r = static_cast<double> (component) / 255;

  component = 0;

  i = convHexDigit (*buf++);
  if (i < 0)
    return FALSE;

  component |= static_cast<uint8_t> (i) << 4;

  i = convHexDigit (*buf++);
  if (i < 0)
    return FALSE;

  component |= static_cast<uint8_t> (i);

  *g = static_cast<double> (component) / 255;

  if (*buf == L'\0')
    {
      // #NNNNNN syntax
      *b = *g;
      *g = *r;
      *r = *a;
      *a = 1;
      return TRUE;
    }

  component = 0;

  i = convHexDigit (*buf++);
  if (i < 0)
    return FALSE;

  component |= static_cast<uint8_t> (i) << 4;

  i = convHexDigit (*buf++);
  if (i < 0)
    return FALSE;

  component |= static_cast<uint8_t> (i);

  *b = static_cast<double> (component) / 255;

  return *buf == L'\0'; // NOLINT(*-implicit-bool-conversion)
}

static void
updateDouble (const HWND hwnd, const double d, const HWND whichChanged)
{
  if (whichChanged == hwnd)
    return;

  WCHAR *str = ftoutf16 (d);

  setWindowText (hwnd, str);

  uiprivFree (str);
}

static void
updateDialog (colorDialog *c, const HWND whichChanged)
{
  double r;
  double g;
  double b;
  WCHAR *str;

  c->updating = TRUE;

  updateDouble (c->editH, c->h, whichChanged);
  updateDouble (c->editS, c->s, whichChanged);
  updateDouble (c->editV, c->v, whichChanged);

  hsv2RGB (c->h, c->s, c->v, &r, &g, &b);

  updateDouble (c->editRDouble, r, whichChanged);
  updateDouble (c->editGDouble, g, whichChanged);
  updateDouble (c->editBDouble, b, whichChanged);
  updateDouble (c->editADouble, c->a, whichChanged);

  const uint8_t rb = static_cast<uint8_t> (r * 255);
  const uint8_t gb = static_cast<uint8_t> (g * 255);
  const uint8_t bb = static_cast<uint8_t> (b * 255);
  const uint8_t ab = static_cast<uint8_t> (c->a * 255);

  if (whichChanged != c->editRInt)
    {
      str = itoutf16 (rb);
      setWindowText (c->editRInt, str);
      uiprivFree (str);
    }

  if (whichChanged != c->editGInt)
    {
      str = itoutf16 (gb);
      setWindowText (c->editGInt, str);
      uiprivFree (str);
    }

  if (whichChanged != c->editBInt)
    {
      str = itoutf16 (bb);
      setWindowText (c->editBInt, str);
      uiprivFree (str);
    }

  if (whichChanged != c->editAInt)
    {
      str = itoutf16 (ab);
      setWindowText (c->editAInt, str);
      uiprivFree (str);
    }

  if (whichChanged != c->editHex)
    {
      WCHAR hexbuf[16];
      rgba2Hex (rb, gb, bb, ab, hexbuf);
      setWindowText (c->editHex, hexbuf);
    }

  invalidateRect (c->svChooser, nullptr, TRUE);
  invalidateRect (c->hSlider, nullptr, TRUE);
  invalidateRect (c->preview, nullptr, TRUE);
  invalidateRect (c->opacitySlider, nullptr, TRUE);

  c->updating = FALSE;
}

static void
drawGrid (ID2D1RenderTarget *rt, const D2D1_RECT_F *fillRect)
{
  D2D1_SIZE_F size;

  D2D1_PIXEL_FORMAT pformat;

  ID2D1BitmapRenderTarget *brt;

  D2D1_COLOR_F color;

  D2D1_BRUSH_PROPERTIES bprop;

  ID2D1SolidColorBrush *brush;

  D2D1_RECT_F rect;

  ID2D1Bitmap *bitmap;

  D2D1_BITMAP_BRUSH_PROPERTIES bbp;

  ID2D1BitmapBrush *bb;

  HRESULT hr;

  size.width  = 10;
  size.height = 10;

#ifdef _MSC_VER
  pformat = rt->GetPixelFormat ();
#else
  {
    typedef D2D1_PIXEL_FORMAT *(__stdcall ID2D1RenderTarget::*GetPixelFormatF) (D2D1_PIXEL_FORMAT *) const;

    const auto gpf = static_cast<GetPixelFormatF> (&rt->GetPixelFormat);

    (rt->*gpf) (&pformat);
  }
#endif

  hr = rt->CreateCompatibleRenderTarget (&size, nullptr, &pformat, D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &brt);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating render target for grid", hr);

  brt->BeginDraw ();

  color.r = 1.0;
  color.g = 1.0;
  color.b = 1.0;
  color.a = 1.0;
  brt->Clear (&color);

  color = D2D1::ColorF (D2D1::ColorF::LightGray, 1.0);
  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = 1.0;
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  hr = brt->CreateSolidColorBrush (&color, &bprop, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating brush for grid", hr);

  rect.left   = 0;
  rect.top    = 0;
  rect.right  = 5;
  rect.bottom = 5;
  brt->FillRectangle (&rect, brush);

  rect.left   = 5;
  rect.top    = 5;
  rect.right  = 10;
  rect.bottom = 10;
  brt->FillRectangle (&rect, brush);

  brush->Release ();

  hr = brt->EndDraw (nullptr, nullptr);
  if (hr != S_OK)
    (void)logHRESULT (L"error finalizing render target for grid", hr);

  hr = brt->GetBitmap (&bitmap);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting bitmap for grid", hr);

  brt->Release ();

  ZeroMemory (&bbp, sizeof (D2D1_BITMAP_BRUSH_PROPERTIES));
  bbp.extendModeX       = D2D1_EXTEND_MODE_WRAP;
  bbp.extendModeY       = D2D1_EXTEND_MODE_WRAP;
  bbp.interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

  hr = rt->CreateBitmapBrush (bitmap, &bbp, &bprop, &bb);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating bitmap brush for grid", hr);

  rt->FillRectangle (fillRect, bb);
  bb->Release ();
  bitmap->Release ();
}

static void
drawSVChooser (colorDialog *c, ID2D1RenderTarget *rt)
{
  D2D1_SIZE_F size;

  D2D1_RECT_F rect;

  double rTop;

  double gTop;

  double bTop;

  D2D1_GRADIENT_STOP stops[2];

  ID2D1GradientStopCollection *collection;

  D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lprop;

  D2D1_BRUSH_PROPERTIES bprop;

  ID2D1LinearGradientBrush *brush;

  ID2D1LinearGradientBrush *opacity;

  ID2D1Layer *layer;

  D2D1_LAYER_PARAMETERS layerparams;

  D2D1_ELLIPSE mparam;

  D2D1_COLOR_F mcolor;

  ID2D1SolidColorBrush *markerBrush;

  size        = realGetSize (rt);
  rect.left   = 0;
  rect.top    = 0;
  rect.right  = size.width;
  rect.bottom = size.height;

  drawGrid (rt, &rect);

  // first, draw a vertical gradient from the current hue at max S/V to black
  // the source example draws it upside down; let's do so too just to be safe
  hsv2RGB (c->h, 1.0, 1.0, &rTop, &gTop, &bTop);

  stops[0].position = 0;
  stops[0].color.r  = 0.0;
  stops[0].color.g  = 0.0;
  stops[0].color.b  = 0.0;
  stops[0].color.a  = 1.0;

  stops[1].position = 1;
  stops[1].color.r  = rTop; // NOLINT(*-narrowing-conversions)
  stops[1].color.g  = gTop; // NOLINT(*-narrowing-conversions)
  stops[1].color.b  = bTop; // NOLINT(*-narrowing-conversions)
  stops[1].color.a  = 1.0;

  HRESULT hr = rt->CreateGradientStopCollection (stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection);
  if (hr != S_OK)
    (void)logHRESULT (L"error making gradient stop collection for first gradient in SV chooser", hr);

  ZeroMemory (&lprop, sizeof (D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES));
  lprop.startPoint.x = size.width / 2;
  lprop.startPoint.y = size.height;
  lprop.endPoint.x   = size.width / 2;
  lprop.endPoint.y   = 0;

  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = c->a; // NOLINT(*-narrowing-conversions)
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  hr = rt->CreateLinearGradientBrush (&lprop, &bprop, collection, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error making gradient brush for first gradient in SV chooser", hr);

  rt->FillRectangle (&rect, brush);
  brush->Release ();
  collection->Release ();

  // second, create an opacity mask for the third step: a horizontal gradientthat goes from opaque to translucent
  stops[0].position = 0;
  stops[0].color.r  = 0.0;
  stops[0].color.g  = 0.0;
  stops[0].color.b  = 0.0;
  stops[0].color.a  = 1.0;

  stops[1].position = 1;
  stops[1].color.r  = 0.0;
  stops[1].color.g  = 0.0;
  stops[1].color.b  = 0.0;
  stops[1].color.a  = 0.0;

  hr = rt->CreateGradientStopCollection (stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection);
  if (hr != S_OK)
    (void)logHRESULT (L"error making gradient stop collection for opacity mask gradient in SV chooser", hr);

  ZeroMemory (&lprop, sizeof (D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES));
  lprop.startPoint.x = 0;
  lprop.startPoint.y = size.height / 2;
  lprop.endPoint.x   = size.width;
  lprop.endPoint.y   = size.height / 2;

  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = 1.0;
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  hr = rt->CreateLinearGradientBrush (&lprop, &bprop, collection, &opacity);
  if (hr != S_OK)
    (void)logHRESULT (L"error making gradient brush for opacity mask gradient in SV chooser", hr);
  collection->Release ();

  // finally, make a vertical gradient from white at the top to black at the bottom (right side up this time) and with
  // the previous opacity mask
  stops[0].position = 0;
  stops[0].color.r  = 1.0;
  stops[0].color.g  = 1.0;
  stops[0].color.b  = 1.0;
  stops[0].color.a  = 1.0;

  stops[1].position = 1;
  stops[1].color.r  = 0.0;
  stops[1].color.g  = 0.0;
  stops[1].color.b  = 0.0;
  stops[1].color.a  = 1.0;

  hr = rt->CreateGradientStopCollection (stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection);
  if (hr != S_OK)
    (void)logHRESULT (L"error making gradient stop collection for second gradient in SV chooser", hr);

  ZeroMemory (&lprop, sizeof (D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES));
  lprop.startPoint.x = size.width / 2;
  lprop.startPoint.y = 0;
  lprop.endPoint.x   = size.width / 2;
  lprop.endPoint.y   = size.height;

  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = 1.0;
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  hr = rt->CreateLinearGradientBrush (&lprop, &bprop, collection, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error making gradient brush for second gradient in SV chooser", hr);

  hr = rt->CreateLayer (&size, &layer);
  if (hr != S_OK)
    (void)logHRESULT (L"error making layer for second gradient in SV chooser", hr);

  ZeroMemory (&layerparams, sizeof (D2D1_LAYER_PARAMETERS));
  layerparams.contentBounds     = rect;
  layerparams.geometricMask     = nullptr;
  layerparams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
  layerparams.maskTransform._11 = 1;
  layerparams.maskTransform._22 = 1;
  layerparams.opacity           = c->a; // NOLINT(*-narrowing-conversions)
  layerparams.opacityBrush      = opacity;
  layerparams.layerOptions      = D2D1_LAYER_OPTIONS_NONE;

  rt->PushLayer (&layerparams, layer);
  rt->FillRectangle (&rect, brush);
  rt->PopLayer ();

  layer->Release ();
  brush->Release ();
  collection->Release ();
  opacity->Release ();

  // and now we just draw the marker
  ZeroMemory (&mparam, sizeof (D2D1_ELLIPSE));
  mparam.point.x = c->s * size.width;        // NOLINT(*-narrowing-conversions)
  mparam.point.y = (1 - c->v) * size.height; // NOLINT(*-narrowing-conversions)
  mparam.radiusX = 7;
  mparam.radiusY = 7;

  mcolor.r = 1.0;
  mcolor.g = 1.0;
  mcolor.b = 1.0;
  mcolor.a = 1.0;

  bprop.opacity = 1.0; // the marker should always be opaque

  hr = rt->CreateSolidColorBrush (&mcolor, &bprop, &markerBrush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating brush for SV chooser marker", hr);

  rt->DrawEllipse (&mparam, markerBrush, 2, nullptr);
  markerBrush->Release ();
}

static LRESULT CALLBACK
svChooserSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam,
                  const UINT_PTR uIdSubclass, const DWORD_PTR dwRefData)
{
  auto *const c = reinterpret_cast<struct colorDialog *> (dwRefData);

  switch (uMsg)
    {
    case msgD2DScratchPaint:
      {
        auto *rt = reinterpret_cast<ID2D1RenderTarget *> (lParam);
        drawSVChooser (c, rt);
        return 0;
      }
    case msgD2DScratchLButtonDown:
      {
        const auto *pos  = reinterpret_cast<D2D1_POINT_2F *> (wParam);
        const auto *size = reinterpret_cast<D2D1_SIZE_F *> (lParam);

        c->s = pos->x / size->width;
        c->v = 1 - pos->y / size->height;

        updateDialog (c, nullptr);

        return 0;
      }
    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, svChooserSubProc, uIdSubclass) == FALSE)
          (void)logLastError (L"error removing color dialog SV chooser subclass");

        break;
      }

    default:;
    }

  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

static void
drawArrow (ID2D1RenderTarget *rt, const D2D1_POINT_2F center, double hypot)
{
  D2D1_RECT_F           rect;
  D2D1_MATRIX_3X2_F     oldtf;
  D2D1_MATRIX_3X2_F     rotate;
  D2D1_COLOR_F          color;
  D2D1_BRUSH_PROPERTIES bprop;
  ID2D1SolidColorBrush *brush;

  // to avoid needing a geometry, this will just be a rotated square
  // compute the length of each side; the diagonal of the square is 2 * offset to gradient
  // a^2 + a^2 = c^2 -> 2a^2 = c^2
  // a = sqrt(c^2/2)
  hypot *= hypot;
  hypot /= 2;

  const double leg = sqrt (hypot);
  rect.left        = center.x - leg; // NOLINT(*-narrowing-conversions)
  rect.top         = center.y - leg; // NOLINT(*-narrowing-conversions)
  rect.right       = center.x + leg; // NOLINT(*-narrowing-conversions)
  rect.bottom      = center.y + leg; // NOLINT(*-narrowing-conversions)

  // now we need to rotate the render target 45° (either way works) about the center point
  rt->GetTransform (&oldtf);

  rotate = oldtf * D2D1::Matrix3x2F::Rotation (45, center);
  rt->SetTransform (&rotate);

  // and draw
  color.r = 0.0;
  color.g = 0.0;
  color.b = 0.0;
  color.a = 1.0;

  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = 1.0;
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  const HRESULT hr = rt->CreateSolidColorBrush (&color, &bprop, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating brush for arrow", hr);

  rt->FillRectangle (&rect, brush);
  brush->Release ();

  rt->SetTransform (&oldtf);
}

static void
drawHSlider (const colorDialog *c, ID2D1RenderTarget *rt)
{
  D2D1_RECT_F rect;

  D2D1_GRADIENT_STOP stops[nStops];

  double r;
  double g;
  double b;

  int i;

  D2D1_BRUSH_PROPERTIES bprop;

  D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lprop;

  D2D1_POINT_2F center;

  ID2D1GradientStopCollection *collection;

  ID2D1LinearGradientBrush *brush;

  const D2D1_SIZE_F size = realGetSize (rt);

  rect.left   = size.width / 6; // leftmost sixth for arrow
  rect.top    = 0;
  rect.right  = size.width;
  rect.bottom = size.height;

  for (i = 0; i < nStops; i++)
    {
      double h = i * (360.0 / nStops) / 360.0;
      if (i == nStops - 1)
        h = 0;

      hsv2RGB (h, 1.0, 1.0, &r, &g, &b);
      stops[i].position = static_cast<double> (i) * stopIncr; // NOLINT(*-narrowing-conversions)
      stops[i].color.r  = r;                                  // NOLINT(*-narrowing-conversions)
      stops[i].color.g  = g;                                  // NOLINT(*-narrowing-conversions)
      stops[i].color.b  = b;                                  // NOLINT(*-narrowing-conversions)
      stops[i].color.a  = 1.0;
    }

  // and pin the last one
  stops[i - 1].position = 1.0;

  HRESULT hr = rt->CreateGradientStopCollection (stops, nStops, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating stop collection for H slider gradient", hr);

  ZeroMemory (&lprop, sizeof (D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES));
  lprop.startPoint.x = (rect.right - rect.left) / 2;
  lprop.startPoint.y = 0;
  lprop.endPoint.x   = (rect.right - rect.left) / 2;
  lprop.endPoint.y   = size.height;

  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = 1.0;
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  hr = rt->CreateLinearGradientBrush (&lprop, &bprop, collection, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating gradient brush for H slider", hr);

  rt->FillRectangle (&rect, brush);
  brush->Release ();
  collection->Release ();

  // now draw a black arrow
  center.x           = 0;
  center.y           = c->h * size.height; // NOLINT(*-narrowing-conversions)
  const double hypot = rect.left;
  drawArrow (rt, center, hypot);
}

static LRESULT CALLBACK
hSliderSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, const UINT_PTR uIdSubclass,
                const DWORD_PTR dwRefData)
{

  auto *const c = reinterpret_cast<struct colorDialog *> (dwRefData);
  switch (uMsg)
    {
    case msgD2DScratchPaint:
      {
        auto *const rt = reinterpret_cast<ID2D1RenderTarget *> (lParam);

        drawHSlider (c, rt);

        return 0;
      }

    case msgD2DScratchLButtonDown:
      {
        const auto *pos  = reinterpret_cast<D2D1_POINT_2F *> (wParam);
        const auto *size = reinterpret_cast<D2D1_SIZE_F *> (lParam);

        c->h = pos->y / size->height;

        updateDialog (c, nullptr);

        return 0;
      }
    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, hSliderSubProc, uIdSubclass) == FALSE)
          (void)logLastError (L"error removing color dialog H slider subclass");

        break;
      }

    default:;
    }

  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

static void
drawPreview (const colorDialog *c, ID2D1RenderTarget *rt)
{
  D2D1_RECT_F rect;

  double r;

  double g;

  double b;

  D2D1_COLOR_F color;

  D2D1_BRUSH_PROPERTIES bprop;

  ID2D1SolidColorBrush *brush;

  const D2D1_SIZE_F size = realGetSize (rt);

  rect.left   = 0;
  rect.top    = 0;
  rect.right  = size.width;
  rect.bottom = size.height;

  drawGrid (rt, &rect);

  hsv2RGB (c->h, c->s, c->v, &r, &g, &b);
  color.r = r;    // NOLINT(*-narrowing-conversions)
  color.g = g;    // NOLINT(*-narrowing-conversions)
  color.b = b;    // NOLINT(*-narrowing-conversions)
  color.a = c->a; // NOLINT(*-narrowing-conversions)

  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = 1.0;
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  const HRESULT hr = rt->CreateSolidColorBrush (&color, &bprop, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating brush for preview", hr);

  rt->FillRectangle (&rect, brush);
  brush->Release ();
}

static LRESULT CALLBACK
previewSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, const UINT_PTR uIdSubclass,
                const DWORD_PTR dwRefData)
{

  const auto *const c = reinterpret_cast<colorDialog *> (dwRefData);
  switch (uMsg)
    {
    case msgD2DScratchPaint:
      {
        auto *const rt = reinterpret_cast<ID2D1RenderTarget *> (lParam);

        drawPreview (c, rt);

        return 0;
      }
    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, previewSubProc, uIdSubclass) == FALSE)
          (void)logLastError (L"error removing color dialog previewer subclass");
        break;
      }

    default:;
    }

  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

static void
drawOpacitySlider (const colorDialog *c, ID2D1RenderTarget *rt)
{
  D2D1_RECT_F rect;

  D2D1_GRADIENT_STOP stops[2];

  ID2D1GradientStopCollection *collection;

  D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lprop;

  D2D1_BRUSH_PROPERTIES bprop;

  ID2D1LinearGradientBrush *brush;

  D2D1_POINT_2F center;

  const D2D1_SIZE_F size = realGetSize (rt);

  rect.left   = 0;
  rect.top    = 0;
  rect.right  = size.width;
  rect.bottom = size.height * (5.0 / 6.0); // NOLINT(*-narrowing-conversions)

  drawGrid (rt, &rect);

  stops[0].position = 0.0;
  stops[0].color.r  = 0.0;
  stops[0].color.g  = 0.0;
  stops[0].color.b  = 0.0;
  stops[0].color.a  = 1.0;

  stops[1].position = 1.0;
  stops[1].color.r  = 1.0;
  stops[1].color.g  = 1.0;
  stops[1].color.b  = 1.0;
  stops[1].color.a  = 0.0;

  HRESULT hr = rt->CreateGradientStopCollection (stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &collection);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating stop collection for opacity slider gradient", hr);

  ZeroMemory (&lprop, sizeof (D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES));
  lprop.startPoint.x = 0;
  lprop.startPoint.y = (rect.bottom - rect.top) / 2;
  lprop.endPoint.x   = size.width;
  lprop.endPoint.y   = (rect.bottom - rect.top) / 2;

  ZeroMemory (&bprop, sizeof (D2D1_BRUSH_PROPERTIES));
  bprop.opacity       = 1.0;
  bprop.transform._11 = 1;
  bprop.transform._22 = 1;

  hr = rt->CreateLinearGradientBrush (&lprop, &bprop, collection, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating gradient brush for opacity slider", hr);

  rt->FillRectangle (&rect, brush);
  brush->Release ();
  collection->Release ();

  // now draw a black arrow
  center.x = (1 - c->a) * size.width; // NOLINT(*-narrowing-conversions)
  center.y = size.height;

  const double hypot = size.height - rect.bottom;
  drawArrow (rt, center, hypot);
}

static LRESULT CALLBACK
opacitySliderSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam,
                      const UINT_PTR uIdSubclass, const DWORD_PTR dwRefData)
{
  auto *const c = reinterpret_cast<colorDialog *> (dwRefData);

  switch (uMsg)
    {
    case msgD2DScratchPaint:
      {
        auto *const rt = reinterpret_cast<ID2D1RenderTarget *> (lParam);
        drawOpacitySlider (c, rt);
        return 0;
      }

    case msgD2DScratchLButtonDown:
      {
        const auto *const pos  = reinterpret_cast<D2D1_POINT_2F *> (wParam);
        const auto *const size = reinterpret_cast<D2D1_SIZE_F *> (lParam);

        c->a = 1 - pos->x / size->width;

        updateDialog (c, nullptr);

        return 0;
      }

    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, opacitySliderSubProc, uIdSubclass) == FALSE)
          (void)logLastError (L"error removing color dialog opacity slider subclass");

        break;
      }

    default:;
    }

  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

HWND
replaceWithD2DScratch (const HWND parent, const int id, SUBCLASSPROC subproc, void *data)
{
  RECT r;

  const HWND replace = getDlgItem (parent, id);
  uiWindowsEnsureGetWindowRect (replace, &r);

  mapWindowRect (nullptr, parent, &r);
  uiWindowsEnsureDestroyWindow (replace);

  return newD2DScratch (parent, &r, reinterpret_cast<HMENU> (id), subproc, reinterpret_cast<DWORD_PTR> (data));
}

static LONG
offsetTo (const HWND a, const HWND b)
{
  RECT ra;
  uiWindowsEnsureGetWindowRect (a, &ra);

  RECT rb;
  uiWindowsEnsureGetWindowRect (b, &rb);

  return rb.top - ra.bottom;
}

static void
moveWindowsUp (const colorDialog *c, const LONG by, ...) // NOLINT(*-dcl50-cpp)
{
  va_list ap;

  RECT r;

  va_start (ap, by);
  for (;;)
    {
      const HWND cur = va_arg (ap, HWND);
      if (cur == nullptr)
        break;

      uiWindowsEnsureGetWindowRect (cur, &r);
      mapWindowRect (nullptr, c->hwnd, &r);

      r.top -= by;
      r.bottom -= by;

      uiWindowsEnsureMoveWindowDuringResize (cur, r.left, r.top, r.right - r.left, r.bottom - r.top);
    }
  va_end (ap);
}

static void
fixupControlPositions (const colorDialog *c)
{
  uiWindowsSizing sizing;

  const HWND labelH   = getDlgItem (c->hwnd, rcHLabel);
  const HWND labelS   = getDlgItem (c->hwnd, rcSLabel);
  const HWND labelV   = getDlgItem (c->hwnd, rcVLabel);
  const HWND labelR   = getDlgItem (c->hwnd, rcRLabel);
  const HWND labelG   = getDlgItem (c->hwnd, rcGLabel);
  const HWND labelB   = getDlgItem (c->hwnd, rcBLabel);
  const HWND labelA   = getDlgItem (c->hwnd, rcALabel);
  const HWND labelHex = getDlgItem (c->hwnd, rcHexLabel);

  LONG offset = offsetTo (c->editH, c->editS);
  moveWindowsUp (c, offset, labelS, c->editS, labelG, c->editGDouble, c->editGInt, nullptr);

  offset = offsetTo (c->editS, c->editV);
  moveWindowsUp (c, offset, labelV, c->editV, labelB, c->editBDouble, c->editBInt, nullptr);

  offset = offsetTo (c->editBDouble, c->editADouble);
  moveWindowsUp (c, offset, labelA, c->editADouble, c->editAInt, nullptr);

  getSizing (c->hwnd, &sizing, reinterpret_cast<HFONT> (SendMessageW (labelH, WM_GETFONT, 0, 0)));
  offset = sizing.InternalLeading;

  moveWindowsUp (c, offset, labelH, labelS, labelV, labelR, labelG, labelB, labelA, labelHex, nullptr);
}

static colorDialog *
beginColorDialog (const HWND hwnd, const LPARAM lParam)
{

  auto *const c = uiprivNew (struct colorDialog);
  c->hwnd       = hwnd;
  c->out        = reinterpret_cast<colorDialogRGBA *> (lParam);

  // load initial values now
  rgb2HSV (c->out->r, c->out->g, c->out->b, &c->h, &c->s, &c->v);

  c->a = c->out->a;

  c->editH       = getDlgItem (c->hwnd, rcH);
  c->editS       = getDlgItem (c->hwnd, rcS);
  c->editV       = getDlgItem (c->hwnd, rcV);
  c->editRDouble = getDlgItem (c->hwnd, rcRDouble);
  c->editRInt    = getDlgItem (c->hwnd, rcRInt);
  c->editGDouble = getDlgItem (c->hwnd, rcGDouble);
  c->editGInt    = getDlgItem (c->hwnd, rcGInt);
  c->editBDouble = getDlgItem (c->hwnd, rcBDouble);
  c->editBInt    = getDlgItem (c->hwnd, rcBInt);
  c->editADouble = getDlgItem (c->hwnd, rcADouble);
  c->editAInt    = getDlgItem (c->hwnd, rcAInt);
  c->editHex     = getDlgItem (c->hwnd, rcHex);

  c->svChooser     = replaceWithD2DScratch (c->hwnd, rcColorSVChooser, svChooserSubProc, c);
  c->hSlider       = replaceWithD2DScratch (c->hwnd, rcColorHSlider, hSliderSubProc, c);
  c->preview       = replaceWithD2DScratch (c->hwnd, rcPreview, previewSubProc, c);
  c->opacitySlider = replaceWithD2DScratch (c->hwnd, rcOpacitySlider, opacitySliderSubProc, c);

  fixupControlPositions (c);

  // and get the ball rolling
  updateDialog (c, nullptr);
  return c;
}

static void
endColorDialog (colorDialog *c, const INT_PTR code)
{
  if (EndDialog (c->hwnd, code) == 0)
    (void)logLastError (L"error ending color dialog");

  uiprivFree (c);
}

static void
tryFinishDialog (colorDialog *c, const WPARAM wParam)
{
  // cancelling
  if (LOWORD (wParam) != IDOK)
    {
      endColorDialog (c, 1);
      return;
    }

  // OK
  hsv2RGB (c->h, c->s, c->v, &c->out->r, &c->out->g, &c->out->b);
  c->out->a = c->a;
  endColorDialog (c, 2);
}

static double
editDouble (const HWND hwnd)
{
  WCHAR *s = windowText (hwnd);

  const double d = _wtof (s);

  uiprivFree (s);

  return d;
}

static void
hChanged (colorDialog *c)
{
  const double h = editDouble (c->editH);

  if (h < 0 || h >= 1.0)
    return;

  c->h = h;

  updateDialog (c, c->editH);
}

static void
sChanged (colorDialog *c)
{

  const double s = editDouble (c->editS);

  if (s < 0 || s > 1)
    return;

  c->s = s;

  updateDialog (c, c->editS);
}

static void
vChanged (colorDialog *c)
{
  const double v = editDouble (c->editV);

  if (v < 0 || v > 1)
    return;

  c->v = v;

  updateDialog (c, c->editV);
}

static void
rDoubleChanged (colorDialog *c)
{
  double r;
  double g;
  double b;

  hsv2RGB (c->h, c->s, c->v, &r, &g, &b);

  r = editDouble (c->editRDouble);
  if (r < 0 || r > 1)
    return;

  rgb2HSV (r, g, b, &c->h, &c->s, &c->v);

  updateDialog (c, c->editRDouble);
}

static void
gDoubleChanged (colorDialog *c)
{
  double r;
  double g;
  double b;

  hsv2RGB (c->h, c->s, c->v, &r, &g, &b);

  g = editDouble (c->editGDouble);
  if (g < 0 || g > 1)
    return;

  rgb2HSV (r, g, b, &c->h, &c->s, &c->v);

  updateDialog (c, c->editGDouble);
}

static void
bDoubleChanged (colorDialog *c)
{
  double r;
  double g;
  double b;

  hsv2RGB (c->h, c->s, c->v, &r, &g, &b);

  b = editDouble (c->editBDouble);
  if (b < 0 || b > 1)
    return;

  rgb2HSV (r, g, b, &c->h, &c->s, &c->v);

  updateDialog (c, c->editBDouble);
}

static void
aDoubleChanged (colorDialog *c)
{
  const double a = editDouble (c->editADouble);
  if (a < 0 || a > 1)
    return;

  c->a = a;

  updateDialog (c, c->editADouble);
}

static int
editInt (const HWND hwnd)
{
  WCHAR *s = windowText (hwnd);

  const int i = _wtoi (s);

  uiprivFree (s);

  return i;
}

static void
rIntChanged (colorDialog *c)
{
  double r;
  double g;
  double b;

  hsv2RGB (c->h, c->s, c->v, &r, &g, &b);

  const int i = editInt (c->editRInt);
  if (i < 0 || i > 255)
    return;

  r = static_cast<double> (i) / 255.0;

  rgb2HSV (r, g, b, &c->h, &c->s, &c->v);

  updateDialog (c, c->editRInt);
}

static void
gIntChanged (colorDialog *c)
{
  double r;
  double g;
  double b;

  hsv2RGB (c->h, c->s, c->v, &r, &g, &b);

  const int i = editInt (c->editGInt);
  if (i < 0 || i > 255)
    return;

  g = static_cast<double> (i) / 255.0;

  rgb2HSV (r, g, b, &c->h, &c->s, &c->v);

  updateDialog (c, c->editGInt);
}

static void
bIntChanged (colorDialog *c)
{
  double r;
  double g;
  double b;

  hsv2RGB (c->h, c->s, c->v, &r, &g, &b);

  const int i = editInt (c->editBInt);
  if (i < 0 || i > 255)
    return;

  b = static_cast<double> (i) / 255.0;

  rgb2HSV (r, g, b, &c->h, &c->s, &c->v);

  updateDialog (c, c->editBInt);
}

static void
aIntChanged (colorDialog *c)
{
  const int a = editInt (c->editAInt);
  if (a < 0 || a > 255)
    return;

  c->a = static_cast<double> (a) / 255;

  updateDialog (c, c->editAInt);
}

static void
hexChanged (colorDialog *c)
{
  double r;
  double g;
  double b;
  double a;

  WCHAR *buf = windowText (c->editHex);

  const BOOL is = hex2RGBA (buf, &r, &g, &b, &a);

  uiprivFree (buf);

  if (is == 0)
    return;

  rgb2HSV (r, g, b, &c->h, &c->s, &c->v);

  c->a = a;

  updateDialog (c, c->editHex);
}

static std::map<int, void (*) (colorDialog *)>
get_changed ()
{
  static const std::map<int, void (*) (colorDialog *)> changed = {
    { rcH,       hChanged       },
    { rcS,       sChanged       },
    { rcV,       vChanged       },
    { rcRDouble, rDoubleChanged },
    { rcGDouble, gDoubleChanged },
    { rcBDouble, bDoubleChanged },
    { rcADouble, aDoubleChanged },
    { rcRInt,    rIntChanged    },
    { rcGInt,    gIntChanged    },
    { rcBInt,    bIntChanged    },
    { rcAInt,    aIntChanged    },
    { rcHex,     hexChanged     },
  };

  return changed;
}

static INT_PTR CALLBACK
colorDialogDlgProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  auto *c = reinterpret_cast<colorDialog *> (GetWindowLongPtrW (hwnd, DWLP_USER));

  if (c == nullptr)
    {
      if (uMsg == WM_INITDIALOG)
        {
          c = beginColorDialog (hwnd, lParam);
          SetWindowLongPtrW (hwnd, DWLP_USER, reinterpret_cast<LONG_PTR> (c));
          return TRUE;
        }
      return FALSE;
    }

  switch (uMsg)
    {
    case WM_COMMAND:
      SetWindowLongPtrW (c->hwnd, DWLP_MSGRESULT, 0);

      switch (LOWORD (wParam))
        {
        case IDOK:
          [[fallthrough]];

        case IDCANCEL:
          {
            if (HIWORD (wParam) != BN_CLICKED)
              return FALSE;

            tryFinishDialog (c, wParam);

            return TRUE;
          }
        case rcH:
          [[fallthrough]];

        case rcS:
          [[fallthrough]];

        case rcV:
          [[fallthrough]];

        case rcRDouble:
          [[fallthrough]];

        case rcGDouble:
          [[fallthrough]];

        case rcBDouble:
          [[fallthrough]];

        case rcADouble:
          [[fallthrough]];

        case rcRInt:
          [[fallthrough]];

        case rcGInt:
          [[fallthrough]];

        case rcBInt:
          [[fallthrough]];

        case rcAInt:
          [[fallthrough]];

        case rcHex:
          {
            if (HIWORD (wParam) != EN_CHANGE)
              return FALSE;

            if (c->updating != 0) // prevent infinite recursion during an update
              return FALSE;

            (*get_changed ()[LOWORD (wParam)]) (c);

            return TRUE;
          }

        default:
          break;
        }

      return FALSE;

    default:
      break;
    }

  return FALSE;
}

// because Windows doesn't really support resources in static libraries, we have to embed this directly; oh well
/*
rcColorDialog DIALOGEX 13, 54, 344, 209
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_3DLOOK
CAPTION "Color"
FONT 9, "Segoe UI"
BEGIN
        // this size should be big enough to get at least 256x256 on font sizes >= 8 pt
        CTEXT		"AaBbYyZz", rcColorSVChooser, 7, 7, 195, 195, SS_NOPREFIX | SS_BLACKRECT

        // width is the suggested slider height since this is vertical
        CTEXT		"AaBbYyZz", rcColorHSlider, 206, 7, 15, 195, SS_NOPREFIX | SS_BLACKRECT

        LTEXT		"Preview:", -1, 230, 7, 107, 9, SS_NOPREFIX
        CTEXT		"AaBbYyZz", rcPreview, 230, 16, 107, 20, SS_NOPREFIX | SS_BLACKRECT

        LTEXT		"Opacity:", -1, 230, 45, 107, 9, SS_NOPREFIX
        CTEXT		"AaBbYyZz", rcOpacitySlider, 230, 54, 107, 15, SS_NOPREFIX | SS_BLACKRECT

        LTEXT		"&H:", rcHLabel, 230, 81, 8, 8
        EDITTEXT		rcH, 238, 78, 30, 14, ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE
        LTEXT		"&S:", rcSLabel, 230, 95, 8, 8
        EDITTEXT		rcS, 238, 92, 30, 14, ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE
        LTEXT		"&V:", rcVLabel, 230, 109, 8, 8
        EDITTEXT		rcV, 238, 106, 30, 14, ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE

        LTEXT		"&R:", rcRLabel, 277, 81, 8, 8
        EDITTEXT		rcRDouble, 285, 78, 30, 14, ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE
        EDITTEXT		rcRInt, 315, 78, 20, 14, ES_LEFT | ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP,
WS_EX_CLIENTEDGE LTEXT		"&G:", rcGLabel, 277, 95, 8, 8 EDITTEXT		rcGDouble, 285, 92, 30, 14, ES_LEFT |
ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE EDITTEXT		rcGInt, 315, 92, 20, 14, ES_LEFT | ES_AUTOHSCROLL |
ES_NUMBER | WS_TABSTOP, WS_EX_CLIENTEDGE LTEXT		"&B:", rcBLabel, 277, 109, 8, 8 EDITTEXT
rcBDouble, 285, 106, 30, 14, ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE EDITTEXT		rcBInt, 315,
106, 20, 14, ES_LEFT | ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP, WS_EX_CLIENTEDGE LTEXT		"&A:", rcALabel, 277,
123, 8, 8 EDITTEXT		rcADouble, 285, 120, 30, 14, ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE
        EDITTEXT		rcAInt, 315, 120, 20, 14, ES_LEFT | ES_AUTOHSCROLL | ES_NUMBER | WS_TABSTOP,
WS_EX_CLIENTEDGE

        LTEXT		"He&x:", rcHexLabel, 269, 146, 16, 8
        EDITTEXT		rcHex, 285, 143, 50, 14, ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE

        DEFPUSHBUTTON	"OK", IDOK, 243, 188, 45, 14, WS_GROUP
        PUSHBUTTON		"Cancel", IDCANCEL, 292, 188, 45, 14, WS_GROUP
END
*/
static const uint8_t data_rcColorDialog[] = {
  0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC4, 0x00, 0xC8, 0x80, 0x1C, 0x00, 0x0D,
  0x00, 0x36, 0x00, 0x58, 0x01, 0xD1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x6C, 0x00, 0x6F, 0x00,
  0x72, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x01, 0x53, 0x00, 0x65, 0x00, 0x67, 0x00, 0x6F, 0x00, 0x65,
  0x00, 0x20, 0x00, 0x55, 0x00, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x85, 0x00, 0x02, 0x50, 0x07, 0x00, 0x07, 0x00, 0xC3, 0x00, 0xC3, 0x00, 0x4C, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82,
  0x00, 0x41, 0x00, 0x61, 0x00, 0x42, 0x00, 0x62, 0x00, 0x59, 0x00, 0x79, 0x00, 0x5A, 0x00, 0x7A, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x00, 0x02, 0x50, 0xCE, 0x00, 0x07, 0x00, 0x0F,
  0x00, 0xC3, 0x00, 0x4D, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x41, 0x00, 0x61, 0x00, 0x42, 0x00, 0x62, 0x00,
  0x59, 0x00, 0x79, 0x00, 0x5A, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x00, 0x02, 0x50, 0xE6, 0x00, 0x07, 0x00, 0x6B, 0x00, 0x09, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x82, 0x00, 0x50, 0x00, 0x72, 0x00, 0x65, 0x00, 0x76, 0x00, 0x69, 0x00, 0x65, 0x00, 0x77, 0x00, 0x3A, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x00, 0x02, 0x50, 0xE6, 0x00, 0x10, 0x00,
  0x6B, 0x00, 0x14, 0x00, 0x4E, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x41, 0x00, 0x61, 0x00, 0x42, 0x00, 0x62,
  0x00, 0x59, 0x00, 0x79, 0x00, 0x5A, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x00, 0x02, 0x50, 0xE6, 0x00, 0x2D, 0x00, 0x6B, 0x00, 0x09, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x82, 0x00, 0x4F, 0x00, 0x70, 0x00, 0x61, 0x00, 0x63, 0x00, 0x69, 0x00, 0x74, 0x00, 0x79, 0x00, 0x3A, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x00, 0x02, 0x50, 0xE6, 0x00, 0x36,
  0x00, 0x6B, 0x00, 0x0F, 0x00, 0x4F, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x41, 0x00, 0x61, 0x00, 0x42, 0x00,
  0x62, 0x00, 0x59, 0x00, 0x79, 0x00, 0x5A, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0xE6, 0x00, 0x51, 0x00, 0x08, 0x00, 0x08, 0x00, 0x5C, 0x04, 0x00, 0x00,
  0xFF, 0xFF, 0x82, 0x00, 0x26, 0x00, 0x48, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x81, 0x50, 0xEE, 0x00, 0x4E, 0x00, 0x1E, 0x00, 0x0E, 0x00, 0x50, 0x04,
  0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x50, 0xE6, 0x00, 0x5F, 0x00, 0x08, 0x00, 0x08, 0x00, 0x5D, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00,
  0x26, 0x00, 0x53, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x80, 0x00, 0x81, 0x50, 0xEE, 0x00, 0x5C, 0x00, 0x1E, 0x00, 0x0E, 0x00, 0x51, 0x04, 0x00, 0x00, 0xFF, 0xFF,
  0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0xE6,
  0x00, 0x6D, 0x00, 0x08, 0x00, 0x08, 0x00, 0x5E, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x26, 0x00, 0x56, 0x00,
  0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x81,
  0x50, 0xEE, 0x00, 0x6A, 0x00, 0x1E, 0x00, 0x0E, 0x00, 0x52, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0x15, 0x01, 0x51, 0x00, 0x08,
  0x00, 0x08, 0x00, 0x5F, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x26, 0x00, 0x52, 0x00, 0x3A, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x81, 0x50, 0x1D, 0x01, 0x4E,
  0x00, 0x1E, 0x00, 0x0E, 0x00, 0x53, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x20, 0x81, 0x50, 0x3B, 0x01, 0x4E, 0x00, 0x14, 0x00, 0x0E, 0x00, 0x54,
  0x04, 0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x50, 0x15, 0x01, 0x5F, 0x00, 0x08, 0x00, 0x08, 0x00, 0x60, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82,
  0x00, 0x26, 0x00, 0x47, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x80, 0x00, 0x81, 0x50, 0x1D, 0x01, 0x5C, 0x00, 0x1E, 0x00, 0x0E, 0x00, 0x55, 0x04, 0x00, 0x00, 0xFF,
  0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x20, 0x81, 0x50,
  0x3B, 0x01, 0x5C, 0x00, 0x14, 0x00, 0x0E, 0x00, 0x56, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0x15, 0x01, 0x6D, 0x00, 0x08, 0x00,
  0x08, 0x00, 0x61, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x26, 0x00, 0x42, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x81, 0x50, 0x1D, 0x01, 0x6A, 0x00,
  0x1E, 0x00, 0x0E, 0x00, 0x57, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x20, 0x81, 0x50, 0x3B, 0x01, 0x6A, 0x00, 0x14, 0x00, 0x0E, 0x00, 0x58, 0x04,
  0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x50, 0x15, 0x01, 0x7B, 0x00, 0x08, 0x00, 0x08, 0x00, 0x62, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00,
  0x26, 0x00, 0x41, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x80, 0x00, 0x81, 0x50, 0x1D, 0x01, 0x78, 0x00, 0x1E, 0x00, 0x0E, 0x00, 0x59, 0x04, 0x00, 0x00, 0xFF, 0xFF,
  0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x20, 0x81, 0x50, 0x3B,
  0x01, 0x78, 0x00, 0x14, 0x00, 0x0E, 0x00, 0x5A, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0x0D, 0x01, 0x92, 0x00, 0x10, 0x00, 0x08,
  0x00, 0x63, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x48, 0x00, 0x65, 0x00, 0x26, 0x00, 0x78, 0x00, 0x3A, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x81, 0x50, 0x1D,
  0x01, 0x8F, 0x00, 0x32, 0x00, 0x0E, 0x00, 0x5B, 0x04, 0x00, 0x00, 0xFF, 0xFF, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x50, 0xF3, 0x00, 0xBC, 0x00, 0x2D, 0x00, 0x0E,
  0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x4F, 0x00, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x50, 0x24, 0x01, 0xBC, 0x00, 0x2D, 0x00, 0x0E, 0x00, 0x02,
  0x00, 0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x43, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x63, 0x00, 0x65, 0x00, 0x6C, 0x00,
  0x00, 0x00, 0x00, 0x00,
};
static_assert (ARRAYSIZE (data_rcColorDialog) == 1144, "wrong size for resource rcColorDialog");

BOOL
showColorDialog (const HWND parent, colorDialogRGBA *c)
{
  const auto result = DialogBoxIndirectParamW (hInstance, reinterpret_cast<const DLGTEMPLATE *> (data_rcColorDialog),
                                               parent, colorDialogDlgProc, reinterpret_cast<LPARAM> (c));

  switch (result)
    {
    case 1:
      // cancel
      return FALSE;

    case 2:
      // ok
      break;

    default:
      (void)logLastError (L"error running color dialog");
    }

  return TRUE;
}
