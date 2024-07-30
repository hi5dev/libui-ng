#include <windowsx.h>

#include "debug.h"
#include "winutil.h"

#include <algorithm>

#define INFINITE_HEIGHT 0x7FFF

void
clientSizeToWindowSize (const HWND hwnd, int *width, int *height, const BOOL hasMenubar)
{
  RECT window;
  window.left   = 0;
  window.top    = 0;
  window.right  = *width;
  window.bottom = *height;

  if (AdjustWindowRectEx (&window, getStyle (hwnd), hasMenubar, getExStyle (hwnd)) == 0)
    {
      (void)logLastError (L"error getting adjusted window rect");

      window.left   = 0;
      window.top    = 0;
      window.right  = *width;
      window.bottom = *height;
    }

  if (hasMenubar != 0)
    {
      RECT temp;

      temp        = window;
      temp.bottom = INFINITE_HEIGHT;
      SendMessageW (hwnd, WM_NCCALCSIZE, FALSE, reinterpret_cast<LPARAM> (&temp));
      window.bottom += temp.top;
    }
  *width  = window.right - window.left;
  *height = window.bottom - window.top;
}

HWND
getDlgItem (const HWND hwnd, const int id)
{
  const HWND out = GetDlgItem (hwnd, id);

  if (out == nullptr)
    (void)logLastError (L"error getting dialog item handle");

  return out;
}

DWORD
getExStyle (const HWND hwnd) { return GetWindowLongPtrW (hwnd, GWL_EXSTYLE); }

DWORD
getStyle (const HWND hwnd) { return GetWindowLongPtrW (hwnd, GWL_STYLE); }

void
invalidateRect (const HWND hwnd, const RECT *r, const BOOL erase)
{
  if (InvalidateRect (hwnd, r, erase) == 0)
    (void)logLastError (L"error invalidating window rect");
}

void
mapWindowRect (const HWND from, const HWND to, RECT *r)
{
  const RECT prevr = *r;

  SetLastError (0);

  if (MapWindowRect (from, to, r) == 0)
    {
      const DWORD le = GetLastError ();
      SetLastError (le);
      if (le != 0)
        {
          (void)logLastError (L"error calling MapWindowRect()");
          *r = prevr;
        }
    }
}

HWND
parentOf (const HWND child)
{
  return GetAncestor (child, GA_PARENT);
}

HWND
parentToplevel (const HWND child)
{
  return GetAncestor (child, GA_ROOT);
}

D2D1_SIZE_F
realGetSize (ID2D1RenderTarget *rt)
{
#ifdef _MSC_VER
  return rt->GetSize ();
#else
  D2D1_SIZE_F size;

  typedef D2D1_SIZE_F *(__stdcall ID2D1RenderTarget::*GetSizeF) (D2D1_SIZE_F *) const;

  const auto gs = static_cast<GetSizeF> (&rt->GetSize);
  (rt->*gs) (&size);

  return size;
#endif
}

void
setExStyle (const HWND hwnd, const DWORD exstyle)
{
  SetWindowLongPtrW (hwnd, GWL_EXSTYLE, exstyle);
}

void
setStyle (const HWND hwnd, const DWORD style)
{
  SetWindowLongPtrW (hwnd, GWL_STYLE, style);
}

void
setWindowInsertAfter (const HWND hwnd, const HWND insertAfter)
{
  static constexpr auto flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE;
  if (SetWindowPos (hwnd, insertAfter, 0, 0, 0, 0, flags) == 0)
    (void)logLastError (L"error reordering window");
}

template <typename Klass, typename... Ts, typename std::enable_if<std::is_integral<Klass>::value>::type *>
int
windowClassOf (const HWND hwnd, Klass *klass1, Ts... klasses)
{
  static constexpr auto MAX_CLASS_NAME = 260;

  WCHAR classname[MAX_CLASS_NAME + 1];

  if (GetClassNameW (hwnd, classname, MAX_CLASS_NAME) == 0)
    {
      (void)logLastError (L"error getting name of window class");
      return -1;
    }

  int i = 0;
  for (auto klass : { klass1, klasses... })
    {
      if (_wcsicmp (classname, klass) == 0)
        return i;
      ++i;
    }

  return -1;
}
