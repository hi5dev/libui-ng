#include "uipriv_windows.hpp"

#define INFINITE_HEIGHT 0x7FFF

/**
 * @brief Ttakes the logic of determining window classes and puts it all in one place.
 *
 * There are a number of places where we need to know what window class an arbitrary handle has theoretically we
 * could use the class atom to avoid a @p _wcsicmp()  however, raymond chen advises against this.
 *
 * @example
 * @code
 * windowClassOf(hwnd, L"class 1", L"class 2", ..., NULL)
 * @endcode
 */
int
windowClassOf (const HWND hwnd, ...) // NOLINT(*-dcl50-cpp)
{
#define maxClassName 260
  WCHAR   classname[maxClassName + 1];
  va_list ap;

  if (GetClassNameW (hwnd, classname, maxClassName) == 0)
    {
      (void)logLastError (L"error getting name of window class");
      return -1;
    }

  va_start (ap, hwnd);
  int i = 0;
  for (;;)
    {
      const WCHAR *curname = va_arg (ap, WCHAR *);

      if (curname == nullptr)
        break;

      if (_wcsicmp (classname, curname) == 0)
        {
          va_end (ap);
          return i;
        }

      i++;
    }
  va_end (ap);

  // no match
  return -1;
}

// wrapper around MapWindowRect() that handles the complex error handling
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

DWORD
getStyle (const HWND hwnd) { return GetWindowLongPtrW (hwnd, GWL_STYLE); }

void
setStyle (const HWND hwnd, const DWORD style)
{
  SetWindowLongPtrW (hwnd, GWL_STYLE, style);
}

DWORD
getExStyle (const HWND hwnd) { return GetWindowLongPtrW (hwnd, GWL_EXSTYLE); }

void
setExStyle (const HWND hwnd, const DWORD exstyle)
{
  SetWindowLongPtrW (hwnd, GWL_EXSTYLE, exstyle);
}

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
parentOf (const HWND child)
{
  return GetAncestor (child, GA_PARENT);
}

HWND
parentToplevel (const HWND child)
{
  return GetAncestor (child, GA_ROOT);
}

void
setWindowInsertAfter (const HWND hwnd, const HWND insertAfter)
{
  if (SetWindowPos (hwnd, insertAfter, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE) == 0)
    (void)logLastError (L"error reordering window");
}

HWND
getDlgItem (const HWND hwnd, const int id)
{

  const HWND out = GetDlgItem (hwnd, id);

  if (out == nullptr)
    (void)logLastError (L"error getting dialog item handle");

  return out;
}

void
invalidateRect (const HWND hwnd, const RECT *r, const BOOL erase)
{
  if (InvalidateRect (hwnd, r, erase) == 0)
    (void)logLastError (L"error invalidating window rect");
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
