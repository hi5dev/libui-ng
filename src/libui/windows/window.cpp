#include "window.h"

#include "debug.h"
#include "init.h"
#include "menu.h"
#include "parent.h"

#include "utf16.h"
#include "winutil.h"

#include <controlsigs.h>
#include <map>

#include <ui/userbugs.h>
#include <ui_win32.h>
#include <uipriv.h>

static void
windowMargins (const uiWindow *w, int *mx, int *my)
{
  uiWindowsSizing sizing;

  *mx = 0;
  *my = 0;
  if (w->margined == 0)
    return;

  uiWindowsGetSizing (w->hwnd, &sizing);

  *mx = windowMargin;
  *my = windowMargin;

  uiWindowsSizingDlgUnitsToPixels (&sizing, mx, my);
}

static void
windowRelayout (const uiWindow *w)
{
  RECT r;

  if (w->child == nullptr)
    return;

  uiWindowsEnsureGetClientRect (w->hwnd, &r);

  int width  = r.right - r.left;
  int height = r.bottom - r.top;

  int mx;
  int my;
  windowMargins (w, &mx, &my);

  width -= 2 * mx;
  height -= 2 * my;

  const auto child = reinterpret_cast<HWND> (uiControlHandle (w->child));
  uiWindowsEnsureMoveWindowDuringResize (child, mx, my, width, height);
}

static LRESULT CALLBACK
windowWndProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  auto *const      cs  = reinterpret_cast<CREATESTRUCTW *> (lParam); // NOLINT(*-no-int-to-ptr)
  const WINDOWPOS *wp  = reinterpret_cast<WINDOWPOS *> (lParam);     // NOLINT(*-no-int-to-ptr)
  auto *const      mmi = reinterpret_cast<MINMAXINFO *> (lParam);    // NOLINT(*-no-int-to-ptr)

  int     width;
  int     height;
  LRESULT lResult;

  const LONG_PTR ww = GetWindowLongPtrW (hwnd, GWLP_USERDATA);
  if (ww == 0)
    {
      if (uMsg == WM_CREATE && cs != nullptr)
        SetWindowLongPtrW (hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (cs->lpCreateParams));
      return DefWindowProcW (hwnd, uMsg, wParam, lParam);
    }

  auto *const w = uiWindow (ww); // NOLINT(*-no-int-to-ptr)

  if (handleParentMessages (hwnd, uMsg, wParam, lParam, &lResult) != FALSE)
    return lResult;
  switch (uMsg)
    {
    case WM_COMMAND:
      {
        if (lParam != 0)
          break;

        if (HIWORD (wParam) != 0 || LOWORD (wParam) <= IDCANCEL)
          break;

        runMenuEvent (LOWORD (wParam), uiWindow (w));

        return 0;
      }

    case WM_WINDOWPOSCHANGED:
      {
        if (wp != nullptr && (wp->flags & SWP_NOMOVE) == 0 && w->changingPosition == 0)
          (*w->onPositionChanged) (w, w->onPositionChangedData);

        if (wp != nullptr && (wp->flags & SWP_NOSIZE) != 0)
          break;

        if (w->onContentSizeChanged != nullptr && w->changingSize == 0)
          (*w->onContentSizeChanged) (w, w->onContentSizeChangedData);

        windowRelayout (w);

        return 0;
      }

    case WM_GETMINMAXINFO:
      {
        lResult = DefWindowProcW (hwnd, uMsg, wParam, lParam);
        uiWindowsControlMinimumSize (uiWindowsControl (w), &width, &height);
        clientSizeToWindowSize (w->hwnd, &width, &height, w->hasMenubar);
        if (mmi != nullptr)
          {
            mmi->ptMinTrackSize.x = width;
            mmi->ptMinTrackSize.y = height;
          }
        return lResult;
      }

    case WM_ACTIVATE:
      {
        if (LOWORD (wParam) == WA_INACTIVE)
          w->focused = 0;

        else
          w->focused = 1;

        w->onFocusChanged (w, w->onFocusChangedData);

        return 0;
      }

    case WM_CLOSE:
      {
        if ((*w->onClosing) (w, w->onClosingData) != 0)
          uiControlDestroy (uiControl (w));

        return 0;
      }

    default:
      break;
    }
  return DefWindowProcW (hwnd, uMsg, wParam, lParam);
}

ATOM
registerWindowClass (const HICON hDefaultIcon, const HCURSOR hDefaultCursor)
{
  WNDCLASSW wc;

  ZeroMemory (&wc, sizeof (WNDCLASSW));

  wc.lpszClassName = windowClass;
  wc.lpfnWndProc   = windowWndProc;
  wc.hInstance     = hInstance;
  wc.hIcon         = hDefaultIcon;
  wc.hCursor       = hDefaultCursor;
  wc.hbrBackground = reinterpret_cast<HBRUSH> (COLOR_BTNFACE + 1); // NOLINT(*-no-int-to-ptr)

  return RegisterClassW (&wc);
}

void
unregisterWindowClass ()
{
  if (UnregisterClassW (windowClass, hInstance) == 0)
    (void)logLastError (L"error unregistering uiWindow window class");
}

static int
defaultOnClosing (uiWindow *, void *)
{
  return 0;
}

static void
defaultOnPositionContentSizeChanged (uiWindow *, void *)
{
  // no-op
}

static void
defaultOnFocusChanged (uiWindow *, void *)
{
  // no-op
}

static std::map<uiWindow *, bool> windows;

static void
uiWindowDestroy (uiControl *c)
{
  auto *const w = uiWindow (c);

  ShowWindow (w->hwnd, SW_HIDE);

  if (w->child != nullptr)
    {
      uiControlSetParent (w->child, nullptr);
      uiControlDestroy (w->child);
    }

  if (w->menubar != nullptr)
    freeMenubar (w->menubar);

  windows.erase (w);

  uiWindowsEnsureDestroyWindow (w->hwnd);

  uiFreeControl (uiControl (w));
}

uiWindowsControlDefaultHandle (uiWindow)

    uiControl *uiWindowParent (uiControl *c)
{
  return nullptr;
}

void
uiWindowSetParent (uiControl *c, uiControl *parent)
{
  uiUserBugCannotSetParentOnToplevel ("uiWindow");
}

static int
uiWindowToplevel (uiControl *c)
{
  return 1;
}

// TODO initial state of windows is hidden; ensure this here and make it so on other platforms
static int
uiWindowVisible (uiControl *c)
{
  const uiWindow *w = uiWindow (c);

  return w->visible;
}

static void
uiWindowShow (uiControl *c)
{
  auto *const w = uiWindow (c);

  w->visible = 1;

  ensureMinimumWindowSize (w);

  if (w->shownOnce != 0)
    {
      ShowWindow (w->hwnd, SW_SHOW);
      return;
    }

  w->shownOnce = TRUE;

  uiWindowsControlMinimumSizeChanged (uiWindowsControl (w));

  ShowWindow (w->hwnd, nCmdShow);

  if (UpdateWindow (w->hwnd) == 0)
    (void)logLastError (L"error calling UpdateWindow() after showing uiWindow for the first time");
}

static void
uiWindowHide (uiControl *c)
{
  auto *const w = uiWindow (c);

  w->visible = 0;

  ShowWindow (w->hwnd, SW_HIDE);
}

uiWindowsControlDefaultEnabled (uiWindow);
uiWindowsControlDefaultEnable (uiWindow);
uiWindowsControlDefaultDisable (uiWindow);
uiWindowsControlDefaultSyncEnableState (uiWindow);
uiWindowsControlDefaultSetParentHWND (uiWindow);

static void
uiWindowMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const uiWindow *w = uiWindow (c);

  int mx;
  int my;

  *width  = 0;
  *height = 0;

  if (w->child != nullptr)
    uiWindowsControlMinimumSize (uiWindowsControl (w->child), width, height);

  windowMargins (w, &mx, &my);

  *width += 2 * mx;
  *height += 2 * my;
}

static void
uiWindowMinimumSizeChanged (uiWindowsControl *c)
{
  auto *const w = uiWindow (c);

  if (uiWindowsControlTooSmall (uiWindowsControl (w)) != 0)
    {
      ensureMinimumWindowSize (w);
      return;
    }

  windowRelayout (w);
}

static void
uiWindowLayoutRect (uiWindowsControl *c, RECT *r)
{
  const uiWindow *w = uiWindow (c);

  // the layout rect is the client rect in this case
  uiWindowsEnsureGetClientRect (w->hwnd, r);
}

uiWindowsControlDefaultAssignControlIDZOrder (uiWindow)

    static void uiWindowChildVisibilityChanged (uiWindowsControl *c)
{
  // TODO eliminate the redundancy
  uiWindowsControlMinimumSizeChanged (c);
}

char *
uiWindowTitle (const uiWindow *w)
{
  return uiWindowsWindowText (w->hwnd);
}

void
uiWindowSetTitle (const uiWindow *w, const char *title)
{
  uiWindowsSetWindowText (w->hwnd, title);
}

static void
windowMonitorRect (const HWND hwnd, RECT *r)
{
  MONITORINFO mi;

  const HMONITOR monitor = MonitorFromWindow (hwnd, MONITOR_DEFAULTTOPRIMARY);

  ZeroMemory (&mi, sizeof (MONITORINFO));

  mi.cbSize = sizeof (MONITORINFO);

  if (GetMonitorInfoW (monitor, &mi) == 0)
    {
      (void)logLastError (L"error getting window monitor rect");
      r->left   = 0;
      r->top    = 0;
      r->right  = GetSystemMetrics (SM_CXSCREEN);
      r->bottom = GetSystemMetrics (SM_CYSCREEN);
      return;
    }

  *r = mi.rcMonitor;
}

void
uiWindowPosition (const uiWindow *w, int *x, int *y)
{
  RECT r;

  uiWindowsEnsureGetWindowRect (w->hwnd, &r);
  *x = r.left;
  *y = r.top;
}

void
uiWindowSetPosition (uiWindow *w, const int x, const int y)
{
  static constexpr auto flags = SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER;

  w->changingPosition = TRUE;

  if (SetWindowPos (w->hwnd, nullptr, x, y, 0, 0, flags) == 0)
    (void)logLastError (L"error setting window position");

  w->changingPosition = FALSE;
}

void
uiWindowOnPositionChanged (uiWindow *w, void (*f) (uiWindow *, void *), void *data)
{
  w->onPositionChanged     = f;
  w->onPositionChangedData = data;
}

void
uiWindowContentSize (const uiWindow *w, int *width, int *height)
{
  RECT r;

  uiWindowsEnsureGetClientRect (w->hwnd, &r);
  *width  = r.right - r.left;
  *height = r.bottom - r.top;
}

void
uiWindowSetContentSize (uiWindow *w, int width, int height)
{
  w->changingSize = TRUE;
  clientSizeToWindowSize (w->hwnd, &width, &height, w->hasMenubar);

  static constexpr auto flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;

  if (SetWindowPos (w->hwnd, nullptr, 0, 0, width, height, flags) == 0)
    (void)logLastError (L"error resizing window");

  w->changingSize = FALSE;
}

int
uiWindowFullscreen (const uiWindow *w)
{
  return w->fullscreen;
}

void
uiWindowSetFullscreen (uiWindow *w, const int fullscreen)
{
  RECT r;

  if (w->fullscreen != 0 && fullscreen != 0)
    return;

  if (w->fullscreen == 0 && fullscreen == 0)
    return;

  w->fullscreen   = fullscreen;
  w->changingSize = TRUE;

  if (w->fullscreen != 0)
    {
      ZeroMemory (&(w->fsPrevPlacement), sizeof (WINDOWPLACEMENT));

      w->fsPrevPlacement.length = sizeof (WINDOWPLACEMENT);

      if (GetWindowPlacement (w->hwnd, &w->fsPrevPlacement) == 0)
        (void)logLastError (L"error getting old window placement");

      windowMonitorRect (w->hwnd, &r);

      setStyle (w->hwnd, getStyle (w->hwnd) & ~WS_OVERLAPPEDWINDOW);

      static constexpr auto flags = SWP_FRAMECHANGED | SWP_NOOWNERZORDER;
      if (SetWindowPos (w->hwnd, HWND_TOP, r.left, r.top, r.right - r.left, r.bottom - r.top, flags) == 0)
        (void)logLastError (L"error making window fullscreen");
    }
  else
    {
      if (w->borderless == 0)
        setStyle (w->hwnd, getStyle (w->hwnd) | WS_OVERLAPPEDWINDOW);

      if (SetWindowPlacement (w->hwnd, &(w->fsPrevPlacement)) == 0)
        (void)logLastError (L"error leaving fullscreen");

      static constexpr auto flags = SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER;
      if (SetWindowPos (w->hwnd, nullptr, 0, 0, 0, 0, flags) == 0)
        (void)logLastError (L"error restoring window border after fullscreen");
    }
  w->changingSize = FALSE;
}

void
uiWindowOnContentSizeChanged (uiWindow *w, void (*f) (uiWindow *, void *), void *data)
{
  w->onContentSizeChanged     = f;
  w->onContentSizeChangedData = data;
}

void
uiWindowOnClosing (uiWindow *w, int (*f) (uiWindow *, void *), void *data)
{
  w->onClosing     = f;
  w->onClosingData = data;
}

void
uiWindowOnFocusChanged (uiWindow *w, void (*f) (uiWindow *, void *), void *data)
{
  w->onFocusChanged     = f;
  w->onFocusChangedData = data;
}

int
uiWindowFocused (const uiWindow *w)
{
  return w->focused;
}

int
uiWindowBorderless (const uiWindow *w)
{
  return w->borderless;
}

void
uiWindowSetBorderless (uiWindow *w, const int borderless)
{
  w->borderless = borderless;
  if (w->borderless != 0)
    setStyle (w->hwnd, getStyle (w->hwnd) & ~WS_OVERLAPPEDWINDOW);

  else if (w->fullscreen == 0)
    setStyle (w->hwnd, getStyle (w->hwnd) | WS_OVERLAPPEDWINDOW);
}

void
uiWindowSetChild (uiWindow *w, uiControl *child)
{
  if (w->child != nullptr)
    {
      uiControlSetParent (w->child, nullptr);
      uiWindowsControlSetParentHWND (uiWindowsControl (w->child), nullptr);
    }
  w->child = child;
  if (w->child != nullptr)
    {
      uiControlSetParent (w->child, uiControl (w));
      uiWindowsControlSetParentHWND (uiWindowsControl (w->child), w->hwnd);
      uiWindowsControlAssignSoleControlIDZOrder (uiWindowsControl (w->child));
      windowRelayout (w);
    }
}

int
uiWindowMargined (const uiWindow *w)
{
  return w->margined;
}

void
uiWindowSetMargined (uiWindow *w, const int margined)
{
  w->margined = margined;
  windowRelayout (w);
}

int
uiWindowResizeable (const uiWindow *w)
{
  return w->resizeable;
}

void
uiWindowSetResizeable (uiWindow *w, const int resizeable)
{
  w->resizeable = resizeable;

  if (w->resizeable != 0)
    setStyle (w->hwnd, getStyle (w->hwnd) | WS_THICKFRAME | WS_MAXIMIZEBOX);

  else
    setStyle (w->hwnd, getStyle (w->hwnd) & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX);
}

static void

setClientSize (const uiWindow *w, const int width, const int height, const BOOL hasMenubar,
               // ReSharper disable once CppDFAConstantParameter
               const DWORD style,
               // ReSharper disable once CppDFAConstantParameter
               const DWORD exstyle)
{
  RECT window;

  window.left   = 0;
  window.top    = 0;
  window.right  = width;
  window.bottom = height;

  if (AdjustWindowRectEx (&window, style, hasMenubar, exstyle) == 0)
    (void)logLastError (L"error getting real window coordinates");

  if (hasMenubar != 0)
    {
      RECT temp;
      temp        = window;
      temp.bottom = INFINITE_HEIGHT;
      SendMessageW (w->hwnd, WM_NCCALCSIZE, FALSE, reinterpret_cast<LPARAM> (&temp));
      window.bottom += temp.top;
    }

  static constexpr auto flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
  if (SetWindowPos (w->hwnd, nullptr, 0, 0, window.right - window.left, window.bottom - window.top, flags) == 0)
    (void)logLastError (L"error resizing window");
}

uiWindow *
uiNewWindow (const char *title, const int width, const int height, const int hasMenubar)
{
  uiWindow *w;

  uiWindowsNewControl (uiWindow, w);

  w->resizeable       = TRUE;
  BOOL hasMenubarBOOL = FALSE;

  if (hasMenubar != 0)
    hasMenubarBOOL = TRUE;

  w->hasMenubar = hasMenubarBOOL;

  static auto constexpr style   = WS_OVERLAPPEDWINDOW;
  static auto constexpr exstyle = 0;

  WCHAR *wtitle = toUTF16 (title);
  w->hwnd = CreateWindowExW (exstyle, windowClass, wtitle, style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr,
                             nullptr, hInstance, w);
  if (w->hwnd == nullptr)
    (void)logLastError (L"error creating window");

  uiprivFree (wtitle);

  if (hasMenubar != 0)
    {
      w->menubar = makeMenubar ();
      if (SetMenu (w->hwnd, w->menubar) == 0)
        (void)logLastError (L"error giving menu to window");
    }

  setClientSize (w, width, height, hasMenubarBOOL, style, exstyle);

  uiWindowOnClosing (w, defaultOnClosing, nullptr);
  uiWindowOnContentSizeChanged (w, defaultOnPositionContentSizeChanged, nullptr);
  uiWindowOnFocusChanged (w, defaultOnFocusChanged, nullptr);
  uiWindowOnPositionChanged (w, defaultOnPositionContentSizeChanged, nullptr);

  windows[w] = true;

  return w;
}

void
ensureMinimumWindowSize (uiWindow *w)
{
  int width;
  int height;
  uiWindowsControlMinimumSize (uiWindowsControl (w), &width, &height);

  RECT r;
  uiWindowsEnsureGetClientRect (w->hwnd, &r);

  width  = std::max<LONG> (width, r.right - r.left);
  height = std::max<LONG> (height, r.bottom - r.top);

  clientSizeToWindowSize (w->hwnd, &width, &height, w->hasMenubar);

  static constexpr auto flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
  if (SetWindowPos (w->hwnd, nullptr, 0, 0, width, height, flags) == 0)
    (void)logLastError (L"error resizing window");
}

void
disableAllWindowsExcept (const uiWindow *which)
{
  for (const auto &w : windows)
    {
      if (w.first == which)
        continue;
      EnableWindow (w.first->hwnd, FALSE);
    }
}

void
enableAllWindowsExcept (const uiWindow *which)
{
  for (const auto &w : windows)
    {
      if (w.first == which)
        continue;

      if (!uiControlEnabled (uiControl (w.first)))
        continue;

      EnableWindow (w.first->hwnd, TRUE);
    }
}
