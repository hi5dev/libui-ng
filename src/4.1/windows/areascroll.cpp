#include <ui_win32.h>

#include "area.h"
#include "areascroll.h"
#include "debug.h"
#include "winutil.h"

#include <algorithm>

static void
scrollto (const uiArea *a, const int which, const scrollParams *p, int pos)
{
  SCROLLINFO si;

  pos = std::max (0, std::min (pos, p->length - p->pagesize));

  invalidateRect (a->hwnd, nullptr, FALSE);

  *p->pos = pos;

  ZeroMemory (&si, sizeof (SCROLLINFO));

  si.cbSize = sizeof (SCROLLINFO);
  si.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;
  si.nPage  = p->pagesize;
  si.nMin   = 0;
  si.nMax   = p->length - 1; // endpoint inclusive
  si.nPos   = *p->pos;

  SetScrollInfo (a->hwnd, which, &si, TRUE);
}

static void
scrollby (const uiArea *a, const int which, const scrollParams *p, const int delta)
{
  scrollto (a, which, p, *p->pos + delta);
}

static void
scroll (const uiArea *a, const int which, const scrollParams *p, const WPARAM wParam, LPARAM)
{
  SCROLLINFO si;

  int pos = *p->pos;
  switch (LOWORD (wParam))
    {
    case SB_LEFT:
      pos = 0;
      break;

    case SB_RIGHT:
      pos = p->length - p->pagesize;
      break;

    case SB_LINELEFT:
      pos--;
      break;

    case SB_LINERIGHT:
      pos++;
      break;

    case SB_PAGELEFT:
      pos -= p->pagesize;
      break;

    case SB_PAGERIGHT:
      pos += p->pagesize;
      break;

    case SB_THUMBPOSITION:
      {
        ZeroMemory (&si, sizeof (SCROLLINFO));

        si.cbSize = sizeof (SCROLLINFO);
        si.fMask  = SIF_POS;

        if (GetScrollInfo (a->hwnd, which, &si) == 0)
          (void)logLastError (L"error getting thumb position for area");

        pos = si.nPos;

        break;
      }

    case SB_THUMBTRACK:
      {
        ZeroMemory (&si, sizeof (SCROLLINFO));

        si.cbSize = sizeof (SCROLLINFO);
        si.fMask  = SIF_TRACKPOS;

        if (GetScrollInfo (a->hwnd, which, &si) == 0)
          (void)logLastError (L"error getting thumb track position for area");

        pos = si.nTrackPos;

        break;
      }

    default:
      break;
    }

  scrollto (a, which, p, pos);
}

static void
wheelscroll (const uiArea *a, const int which, const scrollParams *p, const WPARAM wParam, LPARAM)
{
  UINT scrollAmount;

  int delta = GET_WHEEL_DELTA_WPARAM (wParam);
  if (SystemParametersInfoW (p->wheelSPIAction, 0, &scrollAmount, 0) == 0)
    (void)logLastError (L"error getting area wheel scroll amount");

  if (scrollAmount == WHEEL_PAGESCROLL)
    scrollAmount = p->pagesize;

  if (scrollAmount == 0)
    return;

  delta += *p->wheelCarry;

  const int lines = delta * static_cast<int> (scrollAmount) / WHEEL_DELTA;

  *p->wheelCarry = delta - lines * WHEEL_DELTA / static_cast<int> (scrollAmount);

  scrollby (a, which, p, -lines);
}

static void
hscrollParams (uiArea *a, scrollParams *p)
{
  ZeroMemory (p, sizeof (scrollParams));
  p->pos = &a->hscrollpos;

  RECT r;
  uiWindowsEnsureGetClientRect (a->hwnd, &r);

  p->pagesize       = r.right - r.left;
  p->length         = a->scrollWidth;
  p->wheelCarry     = &a->hwheelCarry;
  p->wheelSPIAction = SPI_GETWHEELSCROLLCHARS;
}

static void
// ReSharper disable once CppDFAConstantParameter
hscrollby (uiArea *a, const int delta)
{
  scrollParams p;
  hscrollParams (a, &p);
  scrollby (a, SB_HORZ, &p, delta);
}

static void
hscroll (uiArea *a, const WPARAM wParam, const LPARAM lParam)
{
  scrollParams p;

  hscrollParams (a, &p);
  scroll (a, SB_HORZ, &p, wParam, lParam);
}

static void
hwheelscroll (uiArea *a, const WPARAM wParam, const LPARAM lParam)
{
  scrollParams p;
  hscrollParams (a, &p);
  wheelscroll (a, SB_HORZ, &p, wParam, lParam);
}

static void
vscrollParams (uiArea *a, scrollParams *p)
{
  ZeroMemory (p, sizeof (scrollParams));
  p->pos = &a->vscrollpos;

  RECT r;
  uiWindowsEnsureGetClientRect (a->hwnd, &r);

  p->pagesize       = r.bottom - r.top;
  p->length         = a->scrollHeight;
  p->wheelCarry     = &a->vwheelCarry;
  p->wheelSPIAction = SPI_GETWHEELSCROLLLINES;
}

static void
// ReSharper disable once CppDFAConstantParameter
vscrollby (uiArea *a, const int delta)
{
  scrollParams p;
  vscrollParams (a, &p);
  scrollby (a, SB_VERT, &p, delta);
}

static void
vscroll (uiArea *a, const WPARAM wParam, const LPARAM lParam)
{
  scrollParams p;
  vscrollParams (a, &p);
  scroll (a, SB_VERT, &p, wParam, lParam);
}

static void
vwheelscroll (uiArea *a, const WPARAM wParam, const LPARAM lParam)
{
  scrollParams p;
  vscrollParams (a, &p);
  wheelscroll (a, SB_VERT, &p, wParam, lParam);
}

BOOL
areaDoScroll (uiArea *a, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, LRESULT *lResult)
{
  switch (uMsg)
    {
    case WM_HSCROLL:
      {
        hscroll (a, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_MOUSEHWHEEL:
      {
        hwheelscroll (a, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_VSCROLL:
      {
        vscroll (a, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_MOUSEWHEEL:
      {
        vwheelscroll (a, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    default:
      break;
    }
  return FALSE;
}

void
areaScrollOnResize (uiArea *a, RECT *)
{
  areaUpdateScroll (a);
}

void
areaUpdateScroll (uiArea *a)
{
  hscrollby (a, 0);
  vscrollby (a, 0);
}
