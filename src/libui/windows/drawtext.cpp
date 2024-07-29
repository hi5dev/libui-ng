// ReSharper disable CppDFAConstantParameter
#include <dwrite.h>

#include <ui/draw.h>

#include "attrstr.h"
#include "attrstr_win32.h"
#include "debug.h"
#include "drawtext.h"

#include "draw.h"
#include "fontcollection.h"
#include "fontmatch.h"
#include "utf16.h"

#include <cfloat>
#include <map>
#include <tgmath.h>
#include <uipriv.h>

static std::map<uiDrawTextAlign, DWRITE_TEXT_ALIGNMENT>
dwriteAligns ()
{
  static std::map<uiDrawTextAlign, DWRITE_TEXT_ALIGNMENT> aligns = {
    { uiDrawTextAlignLeft,   DWRITE_TEXT_ALIGNMENT_LEADING  },
    { uiDrawTextAlignCenter, DWRITE_TEXT_ALIGNMENT_CENTER   },
    { uiDrawTextAlignRight,  DWRITE_TEXT_ALIGNMENT_TRAILING },
  };

  return aligns;
}

uiDrawTextLayout *
uiDrawNewTextLayout (uiDrawTextLayoutParams *params)
{
  auto *tl = uiprivNew (uiDrawTextLayout);

  WCHAR *wDefaultFamily = toUTF16 (params->DefaultFont->Family);

  HRESULT hr
      = dwfactory->CreateTextFormat (wDefaultFamily, nullptr, uiprivWeightToDWriteWeight (params->DefaultFont->Weight),
                                     uiprivItalicToDWriteStyle (params->DefaultFont->Italic),
                                     uiprivStretchToDWriteStretch (params->DefaultFont->Stretch),
                                     pointSizeToDWriteSize (params->DefaultFont->Size), L"", &tl->format);

  uiprivFree (wDefaultFamily);

  if (hr != S_OK)
    (void)logHRESULT (L"error creating IDWriteTextFormat", hr);

  hr = tl->format->SetTextAlignment (dwriteAligns ()[params->Align]);
  if (hr != S_OK)
    (void)logHRESULT (L"error applying text layout alignment", hr);

  hr = dwfactory->CreateTextLayout (static_cast<const WCHAR *> (uiprivAttributedStringUTF16String (params->String)),
                                    uiprivAttributedStringUTF16Len (params->String), tl->format, FLT_MAX, FLT_MAX,
                                    &tl->layout);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating IDWriteTextLayout", hr);

  DWRITE_WORD_WRAPPING wrap = DWRITE_WORD_WRAPPING_WRAP;

  FLOAT maxWidth = static_cast<FLOAT> (params->Width);
  if (params->Width < 0)
    {
      wrap     = DWRITE_WORD_WRAPPING_NO_WRAP;
      maxWidth = FLT_MAX;
    }

  hr = tl->layout->SetWordWrapping (wrap);
  if (hr != S_OK)
    (void)logHRESULT (L"error setting IDWriteTextLayout word wrapping mode", hr);

  hr = tl->layout->SetMaxWidth (maxWidth);
  if (hr != S_OK)
    (void)logHRESULT (L"error setting IDWriteTextLayout max layout width", hr);

  uiprivAttributedStringApplyAttributesToDWriteTextLayout (params, tl->layout, &(tl->backgroundParams));

  tl->u8tou16 = uiprivAttributedStringCopyUTF8ToUTF16Table (params->String, &(tl->nUTF8));
  tl->u16tou8 = uiprivAttributedStringCopyUTF16ToUTF8Table (params->String, &(tl->nUTF16));

  return tl;
}

void
uiDrawFreeTextLayout (uiDrawTextLayout *tl)
{
  uiprivFree (tl->u16tou8);
  uiprivFree (tl->u8tou16);

  for (auto *p : *tl->backgroundParams)
    uiprivFree (p);

  delete tl->backgroundParams;

  tl->layout->Release ();
  tl->format->Release ();

  uiprivFree (tl);
}

static HRESULT
mkSolidBrush (ID2D1RenderTarget *rt, const double r, const double g, const double b, const double a,
              ID2D1SolidColorBrush **brush)
{
  D2D1_BRUSH_PROPERTIES props;
  D2D1_COLOR_F          color;

  ZeroMemory (&props, sizeof (D2D1_BRUSH_PROPERTIES));

  props.opacity = 1.0;

  props.transform._11 = 1;
  props.transform._22 = 1;

  color.r = r; // NOLINT(*-narrowing-conversions)
  color.g = g; // NOLINT(*-narrowing-conversions)
  color.b = b; // NOLINT(*-narrowing-conversions)
  color.a = a; // NOLINT(*-narrowing-conversions)

  return rt->CreateSolidColorBrush (&color, &props, brush);
}

static ID2D1SolidColorBrush *
mustMakeSolidBrush (ID2D1RenderTarget *rt, const double r, const double g, const double b, const double a)
{
  ID2D1SolidColorBrush *brush;

  const HRESULT hr = mkSolidBrush (rt, r, g, b, a, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating solid brush", hr);

  return brush;
}

drawingEffectsAttr::
drawingEffectsAttr ()
    : r (0), g (0), b (0), a (0), u (), ur (0), ug (0), ub (0), ua (0)
{
  this->refcount          = 1;
  this->hasColor          = false;
  this->hasUnderline      = false;
  this->hasUnderlineColor = false;
}

HRESULT STDMETHODCALLTYPE
drawingEffectsAttr::QueryInterface (REFIID riid, void **ppvObject)
{
  if (ppvObject == nullptr)
    return E_POINTER;

  if (riid == IID_IUnknown)
    {
      AddRef ();
      *ppvObject = this;
      return S_OK;
    }

  *ppvObject = nullptr;
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE
drawingEffectsAttr::AddRef ()
{
  this->refcount++;

  return refcount;
}

ULONG STDMETHODCALLTYPE
drawingEffectsAttr::Release ()
{
  this->refcount--;

  if (refcount == 0)
    {
      delete this;
      return 0;
    }

  return refcount;
}

void
drawingEffectsAttr::setColor (const double r, const double g, const double b, const double a)
{
  this->hasColor = true;

  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;
}

void
drawingEffectsAttr::setUnderline (const uiUnderline u)
{
  this->hasUnderline = true;

  this->u = u;
}

void
drawingEffectsAttr::setUnderlineColor (const double r, const double g, const double b, const double a)
{
  this->hasUnderlineColor = true;

  this->ur = r;
  this->ug = g;
  this->ub = b;
  this->ua = a;
}

HRESULT
drawingEffectsAttr::mkColorBrush (ID2D1RenderTarget *rt, ID2D1SolidColorBrush **b) const
{
  if (!hasColor)
    {
      *b = nullptr;
      return S_OK;
    }

  return mkSolidBrush (rt, r, g, this->b, a, b);
}

HRESULT
drawingEffectsAttr::underline (uiUnderline *u) const
{
  if (u == nullptr)
    return E_POINTER;

  if (!hasUnderline)
    return E_UNEXPECTED;

  *u = this->u;

  return S_OK;
}

HRESULT
drawingEffectsAttr::mkUnderlineBrush (ID2D1RenderTarget *rt, ID2D1SolidColorBrush **b) const
{
  if (!hasUnderlineColor)
    {
      *b = nullptr;
      return S_OK;
    }

  return mkSolidBrush (rt, ur, ug, ub, ua, b);
}

class textRenderer final : public IDWriteTextRenderer
{
  ULONG                 refcount;
  ID2D1RenderTarget    *rt;
  BOOL                  snap;
  ID2D1SolidColorBrush *black;

public:
  virtual ~textRenderer () = default;

  textRenderer (ID2D1RenderTarget *rt, const BOOL snap, ID2D1SolidColorBrush *black)
  {
    this->refcount = 1;
    this->rt       = rt;
    this->snap     = snap;
    this->black    = black;
  }

  HRESULT STDMETHODCALLTYPE
  QueryInterface (REFIID riid, void **ppvObject) override
  {
    if (ppvObject == nullptr)
      return E_POINTER;

    if (riid == IID_IUnknown || riid == __uuidof (IDWritePixelSnapping) || riid == __uuidof (IDWriteTextRenderer))
      {
        AddRef ();
        *ppvObject = this;
        return S_OK;
      }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE
  AddRef () override
  {
    this->refcount++;

    return refcount;
  }

  ULONG STDMETHODCALLTYPE
  Release () override
  {
    this->refcount--;

    if (refcount == 0)
      {
        delete this;
        return 0;
      }

    return refcount;
  }

  // IDWritePixelSnapping
  HRESULT STDMETHODCALLTYPE
  GetCurrentTransform (void *clientDrawingContext, DWRITE_MATRIX *transform) override
  {
    D2D1_MATRIX_3X2_F d2dtf;

    if (transform == nullptr)
      return E_POINTER;

    rt->GetTransform (&d2dtf);
    transform->m11 = d2dtf._11;
    transform->m12 = d2dtf._12;
    transform->m21 = d2dtf._21;
    transform->m22 = d2dtf._22;
    transform->dx  = d2dtf._31;
    transform->dy  = d2dtf._32;

    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE
  GetPixelsPerDip (void *clientDrawingContext, FLOAT *pixelsPerDip) override
  {
    FLOAT dpix;
    FLOAT dpiy;

    if (pixelsPerDip == nullptr)
      return E_POINTER;

    rt->GetDpi (&dpix, &dpiy);

    *pixelsPerDip = dpix / 96;

    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE
  IsPixelSnappingDisabled (void *clientDrawingContext, BOOL *isDisabled) override
  {
    if (isDisabled == nullptr)
      return E_POINTER;

    *isDisabled = static_cast<BOOL> (snap == 0);

    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE
  DrawGlyphRun (void *clientDrawingContext, const FLOAT baselineOriginX, const FLOAT baselineOriginY,
                const DWRITE_MEASURING_MODE measuringMode, const DWRITE_GLYPH_RUN *glyphRun,
                const DWRITE_GLYPH_RUN_DESCRIPTION *glyphRunDescription, IUnknown *clientDrawingEffect) override
  {
    D2D1_POINT_2F baseline;

    const auto *dea = static_cast<drawingEffectsAttr *> (clientDrawingEffect);

    ID2D1SolidColorBrush *brush;

    baseline.x = baselineOriginX;
    baseline.y = baselineOriginY;
    brush      = nullptr;
    if (dea != nullptr)
      {
        const HRESULT hr = dea->mkColorBrush (this->rt, &brush);
        if (hr != S_OK)
          return hr;
      }

    if (brush == nullptr)
      {
        brush = black;
        brush->AddRef ();
      }
    rt->DrawGlyphRun (baseline, glyphRun, brush, measuringMode);

    brush->Release ();

    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE
  DrawInlineObject (void *clientDrawingContext, const FLOAT originX, const FLOAT originY,
                    IDWriteInlineObject *inlineObject, const BOOL isSideways, const BOOL isRightToLeft,
                    IUnknown *clientDrawingEffect) override
  {
    if (inlineObject == nullptr)
      return E_POINTER;

    return inlineObject->Draw (clientDrawingContext, this, originX, originY, isSideways, isRightToLeft,
                               clientDrawingEffect);
  }

  HRESULT STDMETHODCALLTYPE
  DrawStrikethrough (void *clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY,
                     const DWRITE_STRIKETHROUGH *strikethrough, IUnknown *clientDrawingEffect) override
  {
    return E_UNEXPECTED;
  }

  HRESULT STDMETHODCALLTYPE
  DrawUnderline (void *clientDrawingContext, const FLOAT baselineOriginX, const FLOAT baselineOriginY,
                 const DWRITE_UNDERLINE *underline, IUnknown *clientDrawingEffect) override
  {
    const auto *dea = static_cast<drawingEffectsAttr *> (clientDrawingEffect);

    D2D1::Matrix3x2F pixeltf;

    FLOAT dpix;
    FLOAT dpiy;

    if (underline == nullptr)
      return E_POINTER;

    if (dea == nullptr)
      return E_UNEXPECTED;

    uiUnderline utype;
    HRESULT     hr = dea->underline (&utype);
    if (hr != S_OK)
      return hr;

    ID2D1SolidColorBrush *brush;
    hr = dea->mkUnderlineBrush (rt, &brush);

    if (hr != S_OK)
      return hr;

    if (brush == nullptr)
      {
        hr = dea->mkColorBrush (rt, &brush);
        if (hr != S_OK)
          return hr;
      }
    if (brush == nullptr)
      {
        brush = black;
        brush->AddRef ();
      }

    D2D1_RECT_F rect;
    rect.left   = baselineOriginX;
    rect.top    = baselineOriginY + underline->offset;
    rect.right  = rect.left + underline->width;
    rect.bottom = rect.top + underline->thickness;

    switch (utype)
      {
      case uiUnderlineSingle:
        {
          rt->FillRectangle (&rect, brush);
          break;
        }

      case uiUnderlineDouble:
        {
          D2D1_POINT_2F pt;
          rt->GetTransform (&pixeltf);
          rt->GetDpi (&dpix, &dpiy);

          pixeltf = pixeltf * D2D1::Matrix3x2F::Scale (dpix / 96, dpiy / 96);

          pt.x = 0;
          pt.y = rect.top;
          pt   = pixeltf.TransformPoint (pt);

          rect.top = static_cast<FLOAT> (pt.y + 0.5);

          pixeltf.Invert ();

          pt = pixeltf.TransformPoint (pt);

          rect.top = pt.y;

          // first line
          rect.top -= underline->thickness;

          // and it seems we need to recompute this
          rect.bottom = rect.top + underline->thickness;
          rt->FillRectangle (&rect, brush);

          // second line
          rect.top += 2 * underline->thickness;
          rect.bottom = rect.top + underline->thickness;
          rt->FillRectangle (&rect, brush);

          break;
        }
      case uiUnderlineSuggestion:
        {
          ID2D1PathGeometry *path;
          ID2D1GeometrySink *sink;

          bool first = true;

          auto hresult = d2dfactory->CreatePathGeometry (&path);
          if (hresult != S_OK)
            return hresult;

          hresult = path->Open (&sink);
          if (hresult != S_OK)
            return hresult;

          const double amplitude = underline->thickness;
          const double period    = 5 * underline->thickness;
          const double xOffset   = baselineOriginX;
          const double yOffset   = baselineOriginY + underline->offset;

          for (double t = 0; t < underline->width; t++) // NOLINT(*-flp30-c)
            {
              D2D1_POINT_2F start_point;

              const double x     = t + xOffset;
              const double angle = 2 * uiPi * fmod (x, period) / period;
              const double y     = amplitude * sin (angle) + yOffset; // NOLINT(*-math-missing-parentheses)
              start_point.x      = x;                                 // NOLINT(*-narrowing-conversions)
              start_point.y      = y;                                 // NOLINT(*-narrowing-conversions)

              if (first)
                {
                  sink->BeginFigure (start_point, D2D1_FIGURE_BEGIN_HOLLOW);
                  first = false;
                }

              else
                {
                  sink->AddLine (start_point);
                }
            }

          sink->EndFigure (D2D1_FIGURE_END_OPEN);

          hresult = sink->Close ();
          if (hresult != S_OK)
            return hresult;

          sink->Release ();

          rt->DrawGeometry (path, brush, underline->thickness);
          path->Release ();
        }
        break;

      default:;
      }

    brush->Release ();

    return S_OK;
  }
};

void
uiDrawText (const uiDrawContext *c, const uiDrawTextLayout *tl, const double x, const double y)
{
  ID2D1SolidColorBrush *black = mustMakeSolidBrush (c->rt, 0.0, 0.0, 0.0, 1.0);

  auto *const renderer = new textRenderer (c->rt, TRUE, black);

  const auto hr = tl->layout->Draw (nullptr, renderer, x, y); // NOLINT(*-narrowing-conversions)
  if (hr != S_OK)
    (void)logHRESULT (L"error drawing IDWriteTextLayout", hr);

  renderer->Release ();
  black->Release ();
}

void
uiDrawTextLayoutExtents (const uiDrawTextLayout *tl, double *width, double *height)
{
  DWRITE_TEXT_METRICS metrics;

  const auto hr = tl->layout->GetMetrics (&metrics);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting IDWriteTextLayout layout metrics", hr);

  *width  = metrics.width;
  *height = metrics.height;
}

void
uiLoadControlFont (uiFontDescriptor *f)
{
  NONCLIENTMETRICSW metrics;
  metrics.cbSize = sizeof (metrics);
  if (SystemParametersInfoW (SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0) == 0)
    (void)logLastError (L"error getting non-client metrics");

  IDWriteGdiInterop *gdi;
  auto               hr = dwfactory->GetGdiInterop (&gdi);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting GDI interop", hr);

  IDWriteFont *dwfont;
  hr = gdi->CreateFontFromLOGFONT (&metrics.lfMessageFont, &dwfont);
  if (hr != S_OK)
    (void)logHRESULT (L"error loading font", hr);

  IDWriteFontFamily *dwfamily;
  hr = dwfont->GetFontFamily (&dwfamily);
  if (hr != S_OK)
    (void)logHRESULT (L"error loading font family", hr);

  auto *dc = GetDC (nullptr);
  if (dc == nullptr)
    (void)logLastError (L"error getting DC");

  auto pixels = GetDeviceCaps (dc, LOGPIXELSY);
  if (pixels == 0)
    (void)logLastError (L"error getting device caps");

  double size = abs (metrics.lfMessageFont.lfHeight) * 72.0 / pixels;

  auto *collection = uiprivLoadFontCollection ();
  auto *family     = uiprivFontCollectionFamilyName (collection, dwfamily);

  uiprivFontDescriptorFromIDWriteFont (dwfont, f);
  f->Family = toUTF8 (family);
  f->Size   = size;

  uiprivFree (family);
  uiprivFontCollectionFree (collection);

  dwfamily->Release ();
  gdi->Release ();
  if (ReleaseDC (nullptr, dc) == 0)
    (void)logLastError (L"error releasing DC");
}

void
uiFreeFontDescriptor (const uiFontDescriptor *desc)
{
  uiprivFree (desc->Family);
}
