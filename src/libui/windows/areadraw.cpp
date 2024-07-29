#include "areadraw.h"
#include "area.h"
#include "areautil.h"
#include "debug.h"
#include "draw.h"
#include "winutil.h"

static HRESULT
doPaint (uiArea *a, ID2D1RenderTarget *rt, const RECT *clip)
{
  uiAreaHandler    *ah = a->ah;
  uiAreaDrawParams  dp;
  D2D1_COLOR_F      bgcolor;
  D2D1_MATRIX_3X2_F scrollTransform;

  dp.Context = newContext (rt);

  loadAreaSize (a, rt, &dp.AreaWidth, &dp.AreaHeight);

  dp.ClipX      = clip->left;
  dp.ClipY      = clip->top;
  dp.ClipWidth  = clip->right - clip->left;
  dp.ClipHeight = clip->bottom - clip->top;

  if (a->scrolling != 0)
    {
      dp.ClipX += a->hscrollpos;
      dp.ClipY += a->vscrollpos;
    }

  rt->BeginDraw ();

  if (a->scrolling != 0)
    {
      ZeroMemory (&scrollTransform, sizeof (D2D1_MATRIX_3X2_F));
      scrollTransform._11 = 1;
      scrollTransform._22 = 1;
      scrollTransform._31 = static_cast<float> (-a->hscrollpos);
      scrollTransform._32 = static_cast<float> (-a->vscrollpos);
      rt->SetTransform (&scrollTransform);
    }

  const COLORREF bgcolorref = GetSysColor (COLOR_BTNFACE);

  bgcolor.r = static_cast<float> (GetRValue (bgcolorref)) / 255.0; // NOLINT(*-narrowing-conversions)

  bgcolor.g
      = static_cast<float> (static_cast<BYTE> ((bgcolorref & 0xFF00) >> 8)) / 255.0; // NOLINT(*-narrowing-conversions)

  bgcolor.b = static_cast<float> (GetBValue (bgcolorref)) / 255.0; // NOLINT(*-narrowing-conversions)

  bgcolor.a = 1.0;

  rt->Clear (&bgcolor);

  (*ah->Draw) (ah, a, &dp);

  freeContext (dp.Context);

  return rt->EndDraw (nullptr, nullptr);
}

static void
onWM_PAINT (uiArea *a)
{
  RECT clip;

  // do not clear the update rect; we do that ourselves in doPaint()
  if (GetUpdateRect (a->hwnd, &clip, FALSE) == 0)
    {
      // set a zero clip rect just in case GetUpdateRect() didn't change clip
      clip.left   = 0;
      clip.top    = 0;
      clip.right  = 0;
      clip.bottom = 0;
    }

  const HRESULT hr = doPaint (a, a->rt, &clip);
  switch (hr)
    {
    case S_OK:
      {
        if (ValidateRect (a->hwnd, nullptr) == 0)
          (void)logLastError (L"error validating rect");
        break;
      }

    case static_cast<HRESULT> (D2DERR_RECREATE_TARGET):
      {
        a->rt->Release ();
        a->rt = nullptr;
        break;
      }

    default:
      (void)logHRESULT (L"error painting", hr);
    }
}

static void
onWM_PRINTCLIENT (uiArea *a, const HDC dc)
{
  RECT client;
  uiWindowsEnsureGetClientRect (a->hwnd, &client);

  ID2D1DCRenderTarget *rt = makeHDCRenderTarget (dc, &client);

  const HRESULT hr = doPaint (a, rt, &client);
  if (hr != S_OK)
    (void)logHRESULT (L"error printing uiArea client area", hr);

  rt->Release ();
}

BOOL
areaDoDraw (uiArea *a, const UINT uMsg, const WPARAM wParam, LPARAM, LRESULT *lResult)
{
  switch (uMsg)
    {
    case WM_PAINT:
      {
        onWM_PAINT (a);
        *lResult = 0;
        return TRUE;
      }

    case WM_PRINTCLIENT:
      {
        onWM_PRINTCLIENT (a, reinterpret_cast<HDC> (wParam));
        *lResult = 0;
        return TRUE;
      }

    default:
      break;
    }
  return FALSE;
}

void
areaDrawOnResize (const uiArea *a, const RECT *newClient)
{
  D2D1_SIZE_U size;

  size.width  = newClient->right - newClient->left;
  size.height = newClient->bottom - newClient->top;

  (void)a->rt->Resize (&size);

  invalidateRect (a->hwnd, nullptr, TRUE);
}
