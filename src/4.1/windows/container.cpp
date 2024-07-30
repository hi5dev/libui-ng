#include "container.h"

#include "debug.h"
#include "init.h"
#include "parent.h"
#include "winutil.h"

#include <ui_win32.h>

struct containerInit
{
  uiWindowsControl *c;

  void (*onResize) (uiWindowsControl *);
};

static HWND
parentWithBackground (const HWND hwnd)
{
  HWND parent = hwnd;

  for (;;)
    {
      parent = parentOf (parent);

      const int cls = windowClassOf (parent, L"button", containerClass, NULL);

      if (cls != 0 && cls != 1)
        break;
    }
  return parent;
}

static LRESULT CALLBACK
containerWndProc (HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  LRESULT lResult;

  if (handleParentMessages (hwnd, uMsg, wParam, lParam, &lResult) != FALSE)
    return lResult;

  int minwid;
  int minht;

  HWND hwndParent;
  HDC  dc;

  RECT        r;
  PAINTSTRUCT ps;
  HBRUSH      bgBrush;

  uiWindowsControl *c;

  const CREATESTRUCTW *cs  = reinterpret_cast<CREATESTRUCTW *> (lParam);
  const auto          *wp  = reinterpret_cast<WINDOWPOS *> (lParam);
  auto                *mmi = reinterpret_cast<MINMAXINFO *> (lParam);

  switch (uMsg)
    {
    case WM_CREATE:
      {
        auto *const init = static_cast<containerInit *> (cs->lpCreateParams);
        SetWindowLongPtrW (hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (init->onResize));
        SetWindowLongPtrW (hwnd, 0, reinterpret_cast<LONG_PTR> (init->c));
        break;
      }

    case WM_WINDOWPOSCHANGED:
      {
        if ((wp->flags & SWP_NOSIZE) != 0)
          break;

        const auto onResize
            = reinterpret_cast<void (*) (uiWindowsControl *)> (GetWindowLongPtrW (hwnd, GWLP_USERDATA));

        c = reinterpret_cast<uiWindowsControl *> (GetWindowLongPtrW (hwnd, 0));

        (*onResize) (c);

        return 0;
      }
    case WM_GETMINMAXINFO:
      {
        lResult = DefWindowProcW (hwnd, uMsg, wParam, lParam);
        c       = reinterpret_cast<uiWindowsControl *> (GetWindowLongPtrW (hwnd, 0));
        uiWindowsControlMinimumSize (c, &minwid, &minht);
        mmi->ptMinTrackSize.x = minwid;
        mmi->ptMinTrackSize.y = minht;
        return lResult;
      }

    case WM_PAINT:
      {
        dc = BeginPaint (hwnd, &ps);

        if (dc == nullptr)
          break;

        hwndParent = parentWithBackground (hwnd);

        bgBrush = reinterpret_cast<HBRUSH> (SendMessage (hwndParent, WM_CTLCOLORSTATIC, reinterpret_cast<WPARAM> (dc),
                                                         reinterpret_cast<LPARAM> (hwnd)));
        FillRect (dc, &ps.rcPaint, bgBrush);

        EndPaint (hwnd, &ps);

        return 0;
      }

    case WM_PRINTCLIENT:
      {
        dc = reinterpret_cast<HDC> (wParam);

        GetClientRect (hwnd, &r);

        hwndParent = parentWithBackground (hwnd);

        bgBrush = reinterpret_cast<HBRUSH> (SendMessage (hwndParent, WM_CTLCOLORSTATIC, reinterpret_cast<WPARAM> (dc),
                                                         reinterpret_cast<LPARAM> (hwnd)));

        FillRect (dc, &r, bgBrush);

        return 0;
      }

    case WM_CTLCOLORBTN:
      [[fallthrough]];

    case WM_CTLCOLORSTATIC:
      {
        dc = reinterpret_cast<HDC> (wParam);

        hwndParent = parentWithBackground (hwnd);

        SetTextColor (dc, GetSysColor (COLOR_WINDOWTEXT));

        bgBrush = reinterpret_cast<HBRUSH> (SendMessage (hwndParent, WM_CTLCOLORSTATIC, wParam, lParam));

        return reinterpret_cast<INT_PTR> (bgBrush);
      }

    case WM_ERASEBKGND:
      return 1;

    default:
      break;
    }

  return DefWindowProcW (hwnd, uMsg, wParam, lParam);
}

ATOM
initContainer (const HICON hDefaultIcon, const HCURSOR hDefaultCursor)
{
  WNDCLASSW wc;

  ZeroMemory (&wc, sizeof (WNDCLASSW));
  wc.lpszClassName = containerClass;
  wc.lpfnWndProc   = containerWndProc;
  wc.hInstance     = hInstance;
  wc.hIcon         = hDefaultIcon;
  wc.hCursor       = hDefaultCursor;
  wc.hbrBackground = reinterpret_cast<HBRUSH> ((COLOR_BTNFACE + 1));
  wc.cbWndExtra    = sizeof (void *);
  return RegisterClassW (&wc);
}

void
uninitContainer ()
{
  if (UnregisterClassW (containerClass, hInstance) == 0)
    (void)logLastError (L"error unregistering container window class");
}

HWND
uiWindowsMakeContainer (uiWindowsControl *c, void (*onResize) (uiWindowsControl *))
{
  containerInit init;

  init.c = c;

  init.onResize = onResize;

  return uiWindowsEnsureCreateControlHWND (WS_EX_CONTROLPARENT, containerClass, L"", 0, hInstance, &init, FALSE);
}
