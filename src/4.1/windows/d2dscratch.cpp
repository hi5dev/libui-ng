#include <windowsx.h>

#include <d2d1.h>

#include "d2dscratch.h"
#include "debug.h"
#include "draw.h"
#include "init.h"
#include "sizing.h"
#include "uipriv_windows.hpp"

static HRESULT
d2dScratchDoPaint (const HWND hwnd, ID2D1RenderTarget *rt)
{
  D2D1_COLOR_F bgcolor;

  rt->BeginDraw ();

  const COLORREF bgcolorref = GetSysColor (COLOR_BTNFACE);

  bgcolor.r = GetRValue (bgcolorref) / 255.0;
  bgcolor.g = ((bgcolorref & 0xFF00) >> 8) / 255.0; // NOLINT(*-narrowing-conversions)
  bgcolor.b = GetBValue (bgcolorref) / 255.0;
  bgcolor.a = 1.0;

  rt->Clear (&bgcolor);

  SendMessageW (hwnd, msgD2DScratchPaint, 0, reinterpret_cast<LPARAM> (rt));

  return rt->EndDraw (nullptr, nullptr);
}

static void
d2dScratchDoLButtonDown (const HWND hwnd, ID2D1RenderTarget *rt, const LPARAM lParam)
{
  FLOAT dpix;
  FLOAT dpiy;

  D2D1_POINT_2F pos;
  D2D1_SIZE_F   size;

  const double xpix = GET_X_LPARAM (lParam);
  const double ypix = GET_Y_LPARAM (lParam);

  rt->GetDpi (&dpix, &dpiy);
  pos.x = static_cast<float> (xpix) * 96 / dpix;
  pos.y = static_cast<float> (ypix) * 96 / dpiy;

  size = realGetSize (rt);

  SendMessageW (hwnd, msgD2DScratchLButtonDown, reinterpret_cast<WPARAM> (&pos), reinterpret_cast<LPARAM> (&size));
}

static LRESULT CALLBACK
d2dScratchWndProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  RECT    client;
  HRESULT hr;

  const LONG_PTR init = GetWindowLongPtrW (hwnd, 0);
  if (init == 0)
    {
      if (uMsg == WM_CREATE)
        SetWindowLongPtrW (hwnd, 0, TRUE);
      return DefWindowProcW (hwnd, uMsg, wParam, lParam);
    }

  auto *rt = reinterpret_cast<ID2D1HwndRenderTarget *> (GetWindowLongPtrW (hwnd, GWLP_USERDATA));
  if (rt == nullptr)
    {
      rt = makeHWNDRenderTarget (hwnd);
      SetWindowLongPtrW (hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (rt));
    }

  switch (uMsg)
    {
    case WM_DESTROY:
      {
        rt->Release ();
        SetWindowLongPtrW (hwnd, 0, FALSE);
        break;
      }

    case WM_PAINT:
      {
        hr = d2dScratchDoPaint (hwnd, rt);
        switch (hr)
          {
          case S_OK:
            {
              if (ValidateRect (hwnd, nullptr) == 0)
                (void)logLastError (L"error validating D2D scratch control rect");

              break;
            }

          case static_cast<HRESULT> (D2DERR_RECREATE_TARGET):
            {
              rt->Release ();
              SetWindowLongPtrW (hwnd, GWLP_USERDATA, 0);
              break;
            }

          default:
            {
              (void)logHRESULT (L"error drawing D2D scratch window", hr);
            }
          }

        return 0;
      }

    case WM_PRINTCLIENT:
      {
        uiWindowsEnsureGetClientRect (hwnd, &client);

        auto *dcrt = makeHDCRenderTarget (reinterpret_cast<HDC> (wParam), &client);

        hr = d2dScratchDoPaint (hwnd, dcrt);
        if (hr != S_OK)
          (void)logHRESULT (L"error printing D2D scratch window client area", hr);

        dcrt->Release ();

        return 0;
      }

    case WM_LBUTTONDOWN:
      {
        d2dScratchDoLButtonDown (hwnd, rt, lParam);
        return 0;
      }

    case WM_MOUSEMOVE:
      {
        // also send LButtonDowns when dragging
        if ((wParam & MK_LBUTTON) != 0)
          d2dScratchDoLButtonDown (hwnd, rt, lParam);
        return 0;
      }

    default:;
    }

  return DefWindowProcW (hwnd, uMsg, wParam, lParam);
}

ATOM
registerD2DScratchClass (const HICON hDefaultIcon, const HCURSOR hDefaultCursor)
{
  WNDCLASSW wc;

  ZeroMemory (&wc, sizeof (WNDCLASSW));
  wc.lpszClassName = d2dScratchClass;
  wc.lpfnWndProc   = d2dScratchWndProc;
  wc.hInstance     = hInstance;
  wc.hIcon         = hDefaultIcon;
  wc.hCursor       = hDefaultCursor;
  wc.hbrBackground = reinterpret_cast<HBRUSH> (COLOR_BTNFACE + 1);
  wc.cbWndExtra    = sizeof (LONG_PTR);
  return RegisterClassW (&wc);
}

void
unregisterD2DScratchClass ()
{
  if (UnregisterClassW (d2dScratchClass, hInstance) == 0)
    (void)logLastError (L"error unregistering D2D scratch window class");
}

HWND
newD2DScratch (const HWND parent, const RECT *rect, const HMENU controlID, const SUBCLASSPROC subclass,
               const DWORD_PTR subclassData)
{

  const HWND hwnd
      = CreateWindowExW (0, d2dScratchClass, L"", WS_CHILD | WS_VISIBLE, rect->left, rect->top,
                         rect->right - rect->left, rect->bottom - rect->top, parent, controlID, hInstance, nullptr);

  if (hwnd == nullptr)
    (void)logLastError (L"error creating D2D scratch window");

  if (SetWindowSubclass (hwnd, subclass, 0, subclassData) == FALSE)
    (void)logLastError (L"error subclassing D2D scratch window");

  return hwnd;
}
