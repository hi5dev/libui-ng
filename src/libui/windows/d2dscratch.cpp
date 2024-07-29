#include "d2dscratch.h"

#include <d2d1.h>

/**
 * @brief The Direct2D scratch window is a utility for libui internal use to do quick things with Direct2D.
 *
 * To use, call @p newD2DScratch passing in a subclass procedure. This subclass procedure should handle the
 * @p msgD2DScratchPaint message, which has the following usage:
 * - @p wParam  - @p 0
 * - @p lParam  - @p ID2D1RenderTarget*
 * - @p lResult - @p 0
 *
 * You can optionally also handle @p msgD2DScratchLButtonDown, which is sent when the left mouse button is either
 * pressed for the first time or held while the mouse is moving.
 * - @p wParam  - position in DIPs, as @p D2D1_POINT_2F*
 * - @p lParam  - size of render target in DIPs, as @p D2D1_SIZE_F*
 * - @p lResult - @p 0
 *
 * Other messages can also be handled here.
 */

#define d2dScratchClass L"libui_d2dScratchClass"

static HRESULT
d2dScratchDoPaint (const HWND hwnd, ID2D1RenderTarget *rt)
{
  D2D1_COLOR_F bgcolor;

  rt->BeginDraw ();

  const COLORREF bgcolorref = GetSysColor (COLOR_BTNFACE);
  bgcolor.r  = static_cast<float> (GetRValue (bgcolorref)) / 255.0;
  bgcolor.g = static_cast<float> (static_cast<BYTE> ((bgcolorref & 0xFF00) >> 8)) / 255.0;
  bgcolor.b = static_cast<float> (GetBValue (bgcolorref)) / 255.0;
  bgcolor.a = 1.0;
  rt->Clear (&bgcolor);

  SendMessageW (hwnd, msgD2DScratchPaint, 0, (LPARAM)rt);

  return rt->EndDraw (NULL, NULL);
}

static void
d2dScratchDoLButtonDown (const HWND hwnd, ID2D1RenderTarget *rt, LPARAM lParam)
{
  FLOAT         dpix, dpiy;
  D2D1_POINT_2F pos;
  D2D1_SIZE_F   size;

  const double xpix = (double)GET_X_LPARAM (lParam);
  const double ypix = (double)GET_Y_LPARAM (lParam);
  // these are in pixels; we need points
  // TODO separate the function from areautil.cpp?
  rt->GetDpi (&dpix, &dpiy);
  pos.x = (xpix * 96) / dpix;
  pos.y = (ypix * 96) / dpiy;

  size = realGetSize (rt);

  SendMessageW (hwnd, msgD2DScratchLButtonDown, (WPARAM)(&pos), (LPARAM)(&size));
}

static LRESULT CALLBACK
d2dScratchWndProc (HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  ID2D1DCRenderTarget   *dcrt;
  RECT                   client;
  HRESULT                hr;

  const LONG_PTR init = GetWindowLongPtrW (hwnd, 0);
  if (!init)
    {
      if (uMsg == WM_CREATE)
        SetWindowLongPtrW (hwnd, 0, (LONG_PTR)TRUE);
      return DefWindowProcW (hwnd, uMsg, wParam, lParam);
    }

  ID2D1HwndRenderTarget *rt = (ID2D1HwndRenderTarget *)GetWindowLongPtrW (hwnd, GWLP_USERDATA);
  if (rt == NULL)
    {
      rt = makeHWNDRenderTarget (hwnd);
      SetWindowLongPtrW (hwnd, GWLP_USERDATA, (LONG_PTR)rt);
    }

  switch (uMsg)
    {
    case WM_DESTROY:
      rt->Release ();
      SetWindowLongPtrW (hwnd, 0, (LONG_PTR)FALSE);
      break;
    case WM_PAINT:
      hr = d2dScratchDoPaint (hwnd, rt);
      switch (hr)
        {
        case S_OK:
          if (ValidateRect (hwnd, NULL) == 0)
            logLastError (L"error validating D2D scratch control rect");
          break;
        case (HRESULT)D2DERR_RECREATE_TARGET:
          // DON'T validate the rect
          // instead, simply drop the render target
          // we'll get another WM_PAINT and make the render target again
          // TODO would this require us to invalidate the entire client area?
          rt->Release ();
          SetWindowLongPtrW (hwnd, GWLP_USERDATA, (LONG_PTR)NULL);
          break;
        default:
          logHRESULT (L"error drawing D2D scratch window", hr);
        }
      return 0;
    case WM_PRINTCLIENT:
      uiWindowsEnsureGetClientRect (hwnd, &client);
      dcrt = makeHDCRenderTarget ((HDC)wParam, &client);
      hr   = d2dScratchDoPaint (hwnd, dcrt);
      if (hr != S_OK)
        logHRESULT (L"error printing D2D scratch window client area", hr);
      dcrt->Release ();
      return 0;
    case WM_LBUTTONDOWN:
      d2dScratchDoLButtonDown (hwnd, rt, lParam);
      return 0;
    case WM_MOUSEMOVE:
      // also send LButtonDowns when dragging
      if ((wParam & MK_LBUTTON) != 0)
        d2dScratchDoLButtonDown (hwnd, rt, lParam);
      return 0;
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
  wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.cbWndExtra    = sizeof (LONG_PTR); // for the init status
  return RegisterClassW (&wc);
}

void
unregisterD2DScratchClass (void)
{
  if (UnregisterClassW (d2dScratchClass, hInstance) == 0)
    logLastError (L"error unregistering D2D scratch window class");
}

HWND
newD2DScratch (const HWND parent, RECT *rect, const HMENU controlID, SUBCLASSPROC subclass, DWORD_PTR subclassData)
{

  HWND hwnd = CreateWindowExW (0, d2dScratchClass, L"", WS_CHILD | WS_VISIBLE, rect->left, rect->top,
                               rect->right - rect->left, rect->bottom - rect->top, parent, controlID, hInstance, NULL);
  if (hwnd == NULL)
    // TODO return decoy window
    logLastError (L"error creating D2D scratch window");
  if (SetWindowSubclass (hwnd, subclass, 0, subclassData) == FALSE)
    logLastError (L"error subclassing D2D scratch window");
  return hwnd;
}
