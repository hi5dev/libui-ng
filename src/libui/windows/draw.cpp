#include "draw.h"
#include "debug.h"
#include "drawpath.h"

#include <ui/draw.h>
#include <ui/userbugs.h>
#include <ui_win32.h>
#include <uipriv.h>

#include <cfloat>
#include <vector>

ID2D1Factory *d2dfactory = nullptr;

HRESULT
initDraw ()
{
  D2D1_FACTORY_OPTIONS opts;

  ZeroMemory (&opts, sizeof (D2D1_FACTORY_OPTIONS));

  opts.debugLevel = D2D1_DEBUG_LEVEL_NONE;

  return D2D1CreateFactory (D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_ID2D1Factory, &opts,
                            reinterpret_cast<void **> (&d2dfactory));
}

void
uninitDraw ()
{
  d2dfactory->Release ();
}

ID2D1HwndRenderTarget *
makeHWNDRenderTarget (const HWND hwnd)
{
  D2D1_RENDER_TARGET_PROPERTIES      props;
  D2D1_HWND_RENDER_TARGET_PROPERTIES hprops;

  RECT r;

  ID2D1HwndRenderTarget *rt;

  const HDC dc = GetDC (hwnd);
  if (dc == nullptr)
    (void)logLastError (L"error getting DC to find DPI");

  ZeroMemory (&props, sizeof (D2D1_RENDER_TARGET_PROPERTIES));
  props.type                  = D2D1_RENDER_TARGET_TYPE_DEFAULT;
  props.pixelFormat.format    = DXGI_FORMAT_UNKNOWN;
  props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_UNKNOWN;
  props.dpiX                  = GetDeviceCaps (dc, LOGPIXELSX); // NOLINT(*-narrowing-conversions)
  props.dpiY                  = GetDeviceCaps (dc, LOGPIXELSY); // NOLINT(*-narrowing-conversions)
  props.usage                 = D2D1_RENDER_TARGET_USAGE_NONE;
  props.minLevel              = D2D1_FEATURE_LEVEL_DEFAULT;

  if (ReleaseDC (hwnd, dc) == 0)
    (void)logLastError (L"error releasing DC for finding DPI");

  uiWindowsEnsureGetClientRect (hwnd, &r);

  ZeroMemory (&hprops, sizeof (D2D1_HWND_RENDER_TARGET_PROPERTIES));
  hprops.hwnd             = hwnd;
  hprops.pixelSize.width  = r.right - r.left;
  hprops.pixelSize.height = r.bottom - r.top;
  hprops.presentOptions   = D2D1_PRESENT_OPTIONS_RETAIN_CONTENTS;

  const HRESULT hr = d2dfactory->CreateHwndRenderTarget (&props, &hprops, &rt);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating HWND render target", hr);

  return rt;
}

ID2D1DCRenderTarget *
makeHDCRenderTarget (const HDC dc, const RECT *r)
{
  D2D1_RENDER_TARGET_PROPERTIES props;
  ID2D1DCRenderTarget          *rt;

  ZeroMemory (&props, sizeof (D2D1_RENDER_TARGET_PROPERTIES));
  props.type                  = D2D1_RENDER_TARGET_TYPE_DEFAULT;
  props.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
  props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
  props.dpiX                  = GetDeviceCaps (dc, LOGPIXELSX); // NOLINT(*-narrowing-conversions)
  props.dpiY                  = GetDeviceCaps (dc, LOGPIXELSY); // NOLINT(*-narrowing-conversions)
  props.usage                 = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
  props.minLevel              = D2D1_FEATURE_LEVEL_DEFAULT;

  HRESULT hr = d2dfactory->CreateDCRenderTarget (&props, &rt);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating DC render target", hr);

  hr = rt->BindDC (dc, r);
  if (hr != S_OK)
    (void)logHRESULT (L"error binding DC to DC render target", hr);

  return rt;
}

static void
resetTarget (ID2D1RenderTarget *rt)
{
  D2D1_MATRIX_3X2_F dm;

  // transformations persist
  // reset to the identity matrix
  ZeroMemory (&dm, sizeof (D2D1_MATRIX_3X2_F));
  dm._11 = 1;
  dm._22 = 1;
  rt->SetTransform (&dm);
}

uiDrawContext *
newContext (ID2D1RenderTarget *rt)
{

  auto *c   = uiprivNew (uiDrawContext);
  c->rt     = rt;
  c->states = new std::vector<drawState>;
  resetTarget (c->rt);
  return c;
}

void
freeContext (uiDrawContext *c)
{
  if (c->currentClip != nullptr)
    c->currentClip->Release ();

  if (!c->states->empty ())
    uiprivUserBug ("You did not balance uiDrawSave() and uiDrawRestore() calls.");

  delete c->states;

  uiprivFree (c);
}

static ID2D1Brush *
makeSolidBrush (const uiDrawBrush *b, ID2D1RenderTarget *rt, const D2D1_BRUSH_PROPERTIES *props)
{
  D2D1_COLOR_F          color;
  ID2D1SolidColorBrush *brush;

  color.r = b->R; // NOLINT(*-narrowing-conversions)
  color.g = b->G; // NOLINT(*-narrowing-conversions)
  color.b = b->B; // NOLINT(*-narrowing-conversions)
  color.a = b->A; // NOLINT(*-narrowing-conversions)

  const HRESULT hr = rt->CreateSolidColorBrush (&color, props, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating solid brush", hr);

  return brush;
}

static ID2D1GradientStopCollection *
mkstops (const uiDrawBrush *b, ID2D1RenderTarget *rt)
{
  ID2D1GradientStopCollection *s;

  auto *const stops = static_cast<D2D1_GRADIENT_STOP *> (
      uiprivAlloc (b->NumStops * sizeof (D2D1_GRADIENT_STOP), "D2D1_GRADIENT_STOP[]"));

  for (size_t i = 0; i < b->NumStops; i++)
    {
      stops[i].position = b->Stops[i].Pos; // NOLINT(*-narrowing-conversions)
      stops[i].color.r  = b->Stops[i].R;   // NOLINT(*-narrowing-conversions)
      stops[i].color.g  = b->Stops[i].G;   // NOLINT(*-narrowing-conversions)
      stops[i].color.b  = b->Stops[i].B;   // NOLINT(*-narrowing-conversions)
      stops[i].color.a  = b->Stops[i].A;   // NOLINT(*-narrowing-conversions)
    }

  const HRESULT hr = rt->CreateGradientStopCollection (stops, b->NumStops, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &s);

  if (hr != S_OK)
    (void)logHRESULT (L"error creating stop collection", hr);

  uiprivFree (stops);
  return s;
}

static ID2D1Brush *
makeLinearBrush (const uiDrawBrush *b, ID2D1RenderTarget *rt, const D2D1_BRUSH_PROPERTIES *props)
{
  ID2D1LinearGradientBrush             *brush;
  D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gprops;

  ZeroMemory (&gprops, sizeof (D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES));
  gprops.startPoint.x = b->X0; // NOLINT(*-narrowing-conversions)
  gprops.startPoint.y = b->Y0; // NOLINT(*-narrowing-conversions)
  gprops.endPoint.x   = b->X1; // NOLINT(*-narrowing-conversions)
  gprops.endPoint.y   = b->Y1; // NOLINT(*-narrowing-conversions)

  ID2D1GradientStopCollection *stops = mkstops (b, rt);

  const HRESULT hr = rt->CreateLinearGradientBrush (&gprops, props, stops, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating gradient brush", hr);

  stops->Release ();
  return brush;
}

static ID2D1Brush *
makeRadialBrush (const uiDrawBrush *b, ID2D1RenderTarget *rt, const D2D1_BRUSH_PROPERTIES *props)
{
  ID2D1RadialGradientBrush             *brush;
  D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES gprops;

  ZeroMemory (&gprops, sizeof (D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES));
  gprops.gradientOriginOffset.x = b->X0 - b->X1;  // NOLINT(*-narrowing-conversions)
  gprops.gradientOriginOffset.y = b->Y0 - b->Y1;  // NOLINT(*-narrowing-conversions)
  gprops.center.x               = b->X1;          // NOLINT(*-narrowing-conversions)
  gprops.center.y               = b->Y1;          // NOLINT(*-narrowing-conversions)
  gprops.radiusX                = b->OuterRadius; // NOLINT(*-narrowing-conversions)
  gprops.radiusY                = b->OuterRadius; // NOLINT(*-narrowing-conversions)

  ID2D1GradientStopCollection *stops = mkstops (b, rt);

  const HRESULT hr = rt->CreateRadialGradientBrush (&gprops, props, stops, &brush);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating gradient brush", hr);

  stops->Release ();
  return brush;
}

static ID2D1Brush *
makeBrush (const uiDrawBrush *b, ID2D1RenderTarget *rt)
{
  D2D1_BRUSH_PROPERTIES props;

  ZeroMemory (&props, sizeof (D2D1_BRUSH_PROPERTIES));
  props.opacity = 1.0;
  // identity matrix
  props.transform._11 = 1;
  props.transform._22 = 1;

  switch (b->Type)
    {
    case uiDrawBrushTypeSolid:
      return makeSolidBrush (b, rt, &props);

    case uiDrawBrushTypeLinearGradient:
      return makeLinearBrush (b, rt, &props);

    case uiDrawBrushTypeRadialGradient:
      return makeRadialBrush (b, rt, &props);

    default:
      break;
    }

  uiprivUserBug ("Invalid brush type %d given to drawing operation.", b->Type);

  return nullptr;
}

// how clipping works:
// every fill and stroke is done on a temporary layer with the clip geometry applied to it
// this is really the only way to clip in Direct2D that doesn't involve opacity images
// reference counting:
// - initially the clip is @p nullptr, which means do not use a layer
// - the first time uiDrawClip() is called, we take a reference on the path passed in (this is also why uiPathEnd() is
// needed)
// - every successive time, we create a new PathGeometry and merge the current clip with the new path, releasing the
// reference we took earlier and taking a reference to the new one
// - in Save, we take another reference; in Restore we drop the refernece to the existing path geometry and transfer
// that saved ref to the new path geometry over to the context uiDrawFreePath() doesn't destroy the path geometry, it
// just drops the reference count, so a clip can exist independent of its path

static ID2D1Layer *
applyClip (const uiDrawContext *c)
{
  ID2D1Layer           *layer;
  D2D1_LAYER_PARAMETERS params;

  if (c->currentClip == nullptr)
    return nullptr;

  // create a layer for clipping
  const HRESULT hr = c->rt->CreateLayer (nullptr, &layer);

  if (hr != S_OK)
    (void)logHRESULT (L"error creating clip layer", hr);

  // apply it as the clip
  ZeroMemory (&params, sizeof (D2D1_LAYER_PARAMETERS));

  // this is the equivalent of InfiniteRect() in d2d1helper.h
  params.contentBounds.left   = -FLT_MAX;
  params.contentBounds.top    = -FLT_MAX;
  params.contentBounds.right  = FLT_MAX;
  params.contentBounds.bottom = FLT_MAX;
  params.geometricMask        = static_cast<ID2D1Geometry *> (c->currentClip);
  params.maskAntialiasMode    = c->rt->GetAntialiasMode ();
  params.maskTransform._11    = 1;
  params.maskTransform._22    = 1;
  params.opacity              = 1.0;
  params.opacityBrush         = nullptr;
  params.layerOptions         = D2D1_LAYER_OPTIONS_NONE;

  if (c->rt->GetTextAntialiasMode () == D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE)
    params.layerOptions = D2D1_LAYER_OPTIONS_INITIALIZE_FOR_CLEARTYPE;

  c->rt->PushLayer (&params, layer);

  // return the layer so it can be freed later
  return layer;
}

static void
unapplyClip (const uiDrawContext *c, ID2D1Layer *layer)
{
  if (layer == nullptr)
    return;
  c->rt->PopLayer ();
  layer->Release ();
}

void
uiDrawStroke (const uiDrawContext *c, uiDrawPath *p, const uiDrawBrush *b, const uiDrawStrokeParams *sp)
{
  ID2D1StrokeStyle            *style;
  D2D1_STROKE_STYLE_PROPERTIES dsp;

  ID2D1Brush *brush = makeBrush (b, c->rt);

  ZeroMemory (&dsp, sizeof (D2D1_STROKE_STYLE_PROPERTIES));
  switch (sp->Cap)
    {
    case uiDrawLineCapFlat:
      dsp.startCap = D2D1_CAP_STYLE_FLAT;
      dsp.endCap   = D2D1_CAP_STYLE_FLAT;
      dsp.dashCap  = D2D1_CAP_STYLE_FLAT;
      break;
    case uiDrawLineCapRound:
      dsp.startCap = D2D1_CAP_STYLE_ROUND;
      dsp.endCap   = D2D1_CAP_STYLE_ROUND;
      dsp.dashCap  = D2D1_CAP_STYLE_ROUND;
      break;
    case uiDrawLineCapSquare:
      dsp.startCap = D2D1_CAP_STYLE_SQUARE;
      dsp.endCap   = D2D1_CAP_STYLE_SQUARE;
      dsp.dashCap  = D2D1_CAP_STYLE_SQUARE;
      break;
    }

  switch (sp->Join)
    {
    case uiDrawLineJoinMiter:
      {
        dsp.lineJoin   = D2D1_LINE_JOIN_MITER_OR_BEVEL;
        dsp.miterLimit = sp->MiterLimit; // NOLINT(*-narrowing-conversions)
        break;
      }
    case uiDrawLineJoinRound:
      {
        dsp.lineJoin = D2D1_LINE_JOIN_ROUND;
        break;
      }

    case uiDrawLineJoinBevel:
      {
        dsp.lineJoin = D2D1_LINE_JOIN_BEVEL;
        break;
      }
    }

  dsp.dashStyle = D2D1_DASH_STYLE_SOLID;

  FLOAT *dashes = nullptr;

  // note that dash widths and the dash phase are scaled up by the thickness by Direct2D
  if (sp->NumDashes != 0)
    {
      dsp.dashStyle = D2D1_DASH_STYLE_CUSTOM;
      dashes        = static_cast<FLOAT *> (uiprivAlloc (sp->NumDashes * sizeof (FLOAT), "FLOAT[]"));
      for (size_t i = 0; i < sp->NumDashes; i++)
        dashes[i] = sp->Dashes[i] / sp->Thickness; // NOLINT(*-narrowing-conversions)
    }
  dsp.dashOffset = sp->DashPhase / sp->Thickness; // NOLINT(*-narrowing-conversions)

  const HRESULT hr = d2dfactory->CreateStrokeStyle (&dsp, dashes, sp->NumDashes, &style);

  if (hr != S_OK)
    (void)logHRESULT (L"error creating stroke style", hr);

  if (sp->NumDashes != 0)
    uiprivFree (dashes);

  ID2D1Layer *cliplayer = applyClip (c);
  c->rt->DrawGeometry (pathGeometry (p), brush, sp->Thickness, style); // NOLINT(*-narrowing-conversions)
  unapplyClip (c, cliplayer);

  style->Release ();
  brush->Release ();
}

void
uiDrawFill (const uiDrawContext *c, uiDrawPath *p, const uiDrawBrush *b)
{
  ID2D1Brush *brush     = makeBrush (b, c->rt);
  ID2D1Layer *cliplayer = applyClip (c);
  c->rt->FillGeometry (pathGeometry (p), brush, nullptr);
  unapplyClip (c, cliplayer);
  brush->Release ();
}

void
uiDrawTransform (const uiDrawContext *c, uiDrawMatrix *m)
{
  D2D1_MATRIX_3X2_F dm;
  D2D1_MATRIX_3X2_F cur;

  c->rt->GetTransform (&cur);
  m2d (m, &dm);

  dm = dm * cur;
  c->rt->SetTransform (&dm);
}

void
uiDrawClip (uiDrawContext *c, uiDrawPath *path)
{
  ID2D1PathGeometry *newPath;
  ID2D1GeometrySink *newSink;

  // if there's no current clip, borrow the path
  if (c->currentClip == nullptr)
    {
      c->currentClip = pathGeometry (path);
      // we have to take our own reference to that clip
      c->currentClip->AddRef ();
      return;
    }

  // otherwise we have to intersect the current path with the new one
  // we do that into a new path, and then replace c->currentClip with that new path
  HRESULT hr = d2dfactory->CreatePathGeometry (&newPath);

  if (hr != S_OK)
    (void)logHRESULT (L"error creating new path", hr);

  hr = newPath->Open (&newSink);
  if (hr != S_OK)
    (void)logHRESULT (L"error opening new path", hr);

  hr = c->currentClip->CombineWithGeometry (pathGeometry (path), D2D1_COMBINE_MODE_INTERSECT, nullptr,
                                            D2D1_DEFAULT_FLATTENING_TOLERANCE, newSink);
  if (hr != S_OK)
    (void)logHRESULT (L"error intersecting old path with new path", hr);

  hr = newSink->Close ();
  if (hr != S_OK)
    (void)logHRESULT (L"error closing new path", hr);

  newSink->Release ();

  c->currentClip->Release ();
  c->currentClip = newPath;
}

struct drawState
{
  ID2D1DrawingStateBlock *dsb;
  ID2D1PathGeometry      *clip;
};

void
uiDrawSave (const uiDrawContext *c)
{
  drawState state;

  const HRESULT hr = d2dfactory->CreateDrawingStateBlock (nullptr, nullptr, &state.dsb);

  if (hr != S_OK)
    (void)logHRESULT (L"error creating drawing state block", hr);

  c->rt->SaveDrawingState (state.dsb);

  // if we have a clip, we need to hold another reference to it
  if (c->currentClip != nullptr)
    c->currentClip->AddRef ();

  state.clip = c->currentClip; // even if nullptr assign it

  c->states->push_back (state);
}

void
uiDrawRestore (uiDrawContext *c)
{

  const auto state = (*c->states)[c->states->size () - 1];
  c->states->pop_back ();

  c->rt->RestoreDrawingState (state.dsb);
  state.dsb->Release ();

  // if we have a current clip, we need to drop it
  if (c->currentClip != nullptr)
    c->currentClip->Release ();

  // no need to explicitly addref or release; just transfer the ref
  c->currentClip = state.clip;
}
