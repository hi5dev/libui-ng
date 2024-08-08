#include <windows.h>

#include <ui_test_expect.h>
#include <ui_win32.h>
#include <ui_win32_error.h>
#include <ui_win32_window.h>

#include <assert.h>

#pragma region ui_win32_window

HMENU
ui_win32_window_create_menu (struct ui_window_t *window)
{
  assert (window != NULL);

  return NULL;
}

static ui_test_case
ui_win32_window_create_menu_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_window_create_menu_test);

  ui_test_skip ("TODO");
}

LONG
ui_win32_window_get_attribute (struct ui_window_t *window, const int index, const LONG attribute)
{
  assert (window != NULL);

  const LONG attributes = ui_win32_window_get_attributes (window, index);

  if (attributes == 0)
    return 0;

  return (attributes & attribute) == attribute;
}

LONG
ui_win32_window_get_attributes (struct ui_window_t *window, const int index)
{
  assert (window != NULL);

  const LONG attributes = GetWindowLong (window->handle, index);
  if (attributes == 0)
    (void)ui_win32_assert_on_error (GetWindowLong);

  return attributes;
}

void
ui_win32_window_get_rect (struct ui_window_t *window, const int client_size, int *x, int *y, int *width, int *height)
{
  assert (window != NULL);

  if (x == NULL && y == NULL && width == NULL && height == NULL)
    return;

  RECT rect = {};

  const int success = (client_size ? GetClientRect : GetWindowRect) (window->handle, &rect);
  if (!success && ui_win32_log_last_error (client_size ? "GetClientRect" : "GetWindowRect") != ERROR_SUCCESS)
    return;

  if (x != NULL)
    *x = rect.left;

  if (y != NULL)
    *y = rect.top;

  if (width != NULL)
    *width = rect.right - rect.left;

  if (height != NULL)
    *height = rect.bottom - rect.top;
}

LRESULT CALLBACK
ui_win32_window_procedure (const HWND handle, const UINT message, const WPARAM wparam, const LPARAM lparam)
{
  return DefWindowProc (handle, message, wparam, lparam);
}

void
ui_win32_window_register_class (const HINSTANCE instance)
{
  // NULL is for system-registered classes only
  assert (instance != NULL);

  WNDCLASSEX wc = { sizeof (WNDCLASSEX) };

  // do not register the same class twice for the current instance
  SetLastError (ERROR_SUCCESS);
  if (GetClassInfoEx (instance, UI_WIN32_WINDOW_CLASS, &wc) != 0 && wc.hInstance == instance)
    return;

  // handle unexpected errors raised by GetClassInfoEx
  const DWORD get_class_info_error = GetLastError ();
  if (get_class_info_error != ERROR_SUCCESS && get_class_info_error != ERROR_CLASS_DOES_NOT_EXIST)
    (void)ui_win32_assert_on_error (GetClassInfoEx);

  // a handle to the class cursor - must be a handle to a cursor resource - NULL requires the application to set the
  // the cursor shape whenever the mouse moves into the window
  wc.hCursor = LoadCursor (NULL, IDC_ARROW);
  if (wc.hCursor == NULL)
    (void)ui_win32_assert_on_error (LoadCursor);

  // handle to the class icon -  must be a handle to an icon resource, or NULL for a system-provided default icon
  wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  if (wc.hIcon == NULL)
    (void)ui_win32_assert_on_error (LoadIcon);

  // pointer to a null-terminated string that specifies the window class name
  wc.lpszClassName = UI_WIN32_WINDOW_CLASS;

  // pointer to the window procedure (WindowProc)
  wc.lpfnWndProc = ui_win32_window_procedure;

  // handle to the instance that contains the window procedure for the class
  wc.hInstance = instance;

  // handle to the class background brush
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

  // crash while debugging if the window fails to register
  if (RegisterClassEx (&wc) == 0)
    (void)ui_win32_assert_on_error (RegisterClassEx);
}

static ui_test_case
ui_win32_window_register_class_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_window_register_class_test);

  const HINSTANCE instance = GetModuleHandle (NULL);
  ui_expect_not_null (instance);

  // this will abort on failure to any Win32 API call, which is a good enough test for this case
  ui_win32_window_register_class (instance);

  // calling twice should not register the window class twice, if it does, this will abort and fail the test
  ui_win32_window_register_class (instance);

  // test passes because there were no Win32 API errors
  ui_test_pass ();
}

LONG
ui_win32_window_set_attribute (struct ui_window_t *window, const int index, const LONG attribute)
{
  assert (window != NULL);

  LONG attributes = ui_win32_window_get_attributes (window, index);
  if (attributes == 0)
    return attributes;

  attributes |= attribute;

  if (SetWindowLong (window->handle, index, attributes) == 0)
    (void)ui_win32_assert_on_error (SetWindowLong);

  return attributes;
}

void
ui_win32_window_set_attributes (struct ui_window_t *window, const int index, const LONG attributes)
{
  assert (window != NULL);

  if (SetWindowLong (window->handle, index, attributes) == 0)
    (void)ui_win32_assert_on_error (SetWindowLong);
}

LONG
ui_win32_window_unset_attribute (struct ui_window_t *window, const int index, const LONG attribute)
{
  assert (window != NULL);

  LONG attributes = ui_win32_window_get_attributes (window, index);
  if (attributes == 0)
    return attributes;

  attributes &= ~attribute;

  if (SetWindowLong (window->handle, index, attributes) == 0)
    (void)ui_win32_assert_on_error (SetWindowLong);

  return attributes;
}

#pragma endregion

#pragma region ui_window_t

struct ui_window_t *
ui_window_create (const char *title, const int width, const int height, const int has_menu)
{
  static const HMENU  default_menu   = NULL;
  static const HWND   default_parent = NULL;
  static const WCHAR *default_title  = L"";
  static const int    default_x      = CW_USEDEFAULT;
  static const int    default_y      = CW_USEDEFAULT;
  static const int    style          = WS_OVERLAPPEDWINDOW;
  static const int    exended_styles = 0;

  const HINSTANCE instance = GetModuleHandle (NULL);
  if (instance == NULL)
    ui_win32_abort_on_error ("GetModuleHandle");

  ui_win32_window_register_class (instance);

  const HANDLE heap = GetProcessHeap ();
  if (heap == NULL)
    {
      (void)ui_win32_assert_on_error (GetProcessHeap);
      return NULL;
    }

  struct ui_window_t *window = HeapAlloc (heap, HEAP_ZERO_MEMORY, sizeof (*window));
  if (window == NULL)
    {
      (void)ui_win32_assert_on_error (HeapAlloc);
      return NULL;
    }

  window->handle = CreateWindowEx (exended_styles, UI_WIN32_WINDOW_CLASS, default_title, style, default_x, default_y,
                                   width, height, default_parent, default_menu, instance, window);

  if (window->handle == NULL)
    {
      (void)ui_win32_assert_on_error (CreateWindowEx);
      return NULL;
    }

  if (has_menu)
    {
      window->menu = ui_win32_window_create_menu (window);
      assert (window->menu != NULL);

      if (!SetMenu (window->handle, window->menu))
        (void)ui_win32_assert_on_error (SetMenu);
      else
        assert (window->menu != NULL);
    }

  ui_window_set_title (window, title);
  ui_window_set_client_size (window, width, height);

  return window;
}

void
ui_window_destroy (struct ui_window_t *window)
{
  assert (window != NULL);

  if (!DestroyWindow (window->handle))
    (void)ui_win32_assert_on_error (DestroyWindow);

  const HANDLE heap = GetProcessHeap ();

  if (heap == NULL)
    (void)ui_win32_assert_on_error (GetProcessHeap);

  else if (!HeapFree (heap, 0, window))
    (void)ui_win32_assert_on_error (HeapFree);
}

int
ui_window_get_borderless (struct ui_window_t *window)
{
  assert (window != NULL);

  return !ui_win32_window_get_attribute (window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
}

void
ui_window_get_client_size (struct ui_window_t *window, int *width, int *height)
{
  assert (window != NULL);

  ui_win32_window_get_rect (window, 1, NULL, NULL, width, height);
}

int
ui_window_get_focused (struct ui_window_t *window)
{
  assert (window != NULL);

  return GetFocus () == window->handle;
}

int
ui_window_get_fullscreen (struct ui_window_t *window)
{
  assert (window != NULL);

  return ui_window_get_borderless (window);
}

void
ui_window_get_position (struct ui_window_t *window, int *x, int *y)
{
  assert (window != NULL);

  ui_win32_window_get_rect (window, 0, x, y, NULL, NULL);
}

int
ui_window_get_resizable (struct ui_window_t *window)
{
  assert (window != NULL);

  return ui_win32_window_get_attribute (window, GWL_STYLE, WS_THICKFRAME);
}

char *
ui_window_get_title (struct ui_window_t *window)
{
  assert (window != NULL);

  SetLastError (ERROR_SUCCESS);
  const int length = GetWindowTextLengthA (window->handle);
  if (length == 0)
    (void)ui_win32_assert_on_error (GetWindowTextLengthA);

  char *buf = VirtualAlloc (NULL, length + 1, MEM_COMMIT, PAGE_READWRITE);
  if (buf == NULL)
    (void)ui_win32_assert_on_error (VirtualAlloc);

  if (GetWindowTextA (window->handle, buf, length + 1) == 0)
    (void)ui_win32_assert_on_error (GetWindowTextA);

  char *title = strdup (buf);

  if (VirtualFree (buf, 0, MEM_RELEASE) == 0)
    (void)ui_win32_assert_on_error (VirtualFree);

  return title;
}

void
ui_window_hide (struct ui_window_t *window)
{
  assert (window != NULL);

  if (ShowWindow (window->handle, SW_HIDE) == 0)
    (void)ui_win32_assert_on_error (ShowWindow);
}

int
ui_window_is_visible (struct ui_window_t *window)
{
  assert (window != NULL);

  return IsWindowVisible (window->handle);
}

void
ui_window_set_borderless (struct ui_window_t *window, const int borderless)
{
  assert (window != NULL);

  if (borderless)
    ui_win32_window_unset_attribute (window, GWL_STYLE, WS_OVERLAPPEDWINDOW);

  else
    ui_win32_window_set_attribute (window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
}

void
ui_window_set_client_size (struct ui_window_t *window, const int width, const int height)
{
  assert (window != NULL);

  const DWORD style    = ui_win32_window_get_attributes (window, GWL_STYLE);
  const DWORD style_ex = ui_win32_window_get_attributes (window, GWL_EXSTYLE);

  RECT      rect     = { 0, 0, width, height };
  const int has_menu = window->menu != NULL;
  if (AdjustWindowRectEx (&rect, style, has_menu, style_ex) == 0)
    (void)ui_win32_assert_on_error (AdjustWindowRectEx);

  if (has_menu)
    {
      RECT rect_cpy   = rect;
      rect_cpy.bottom = 0x7FFF;
      SetLastError (ERROR_SUCCESS);
      SendMessage (window->handle, WM_NCCALCSIZE, FALSE, (LPARAM)&rect_cpy);
      (void)ui_win32_assert_on_error (SendMessage);
      rect.bottom += rect_cpy.top;
    }

  const int client_width  = rect.right - rect.left;
  const int client_height = rect.bottom - rect.top;

  static const int flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
  if (SetWindowPos (window->handle, NULL, rect.left, rect.top, client_width, client_height, flags) == 0)
    (void)ui_win32_assert_on_error (SetWindowPos);
}

void
ui_window_set_fullscreen (struct ui_window_t *window, const int fullscreen)
{
  assert (window != NULL);

  const int is_fullscreen = ui_window_get_fullscreen (window);
  if (is_fullscreen == fullscreen)
    return;

  static const UINT WINDOWED_FLAGS   = SWP_NOOWNERZORDER | SWP_FRAMECHANGED;
  static const UINT FULLSCREEN_FLAGS = SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER;

  const UINT flags        = fullscreen ? FULLSCREEN_FLAGS : WINDOWED_FLAGS;
  const HWND insert_after = fullscreen ? HWND_TOP : NULL;

  RECT size = { 0 };

  if (fullscreen)
    {
      const HMONITOR monitor      = MonitorFromWindow (window->handle, MONITOR_DEFAULTTONEAREST);
      MONITORINFO    monitor_info = { sizeof (MONITORINFO) };

      if (GetMonitorInfo (monitor, &monitor_info))
        size = monitor_info.rcMonitor;

      else if (ui_win32_log_last_error ("GetMonitorInfo") != ERROR_SUCCESS)
        return;

      assert (size.right - size.left > 0);
      assert (size.bottom - size.top > 0);
    }

  ui_window_set_borderless (window, fullscreen);

  if (!SetWindowPos (window->handle, insert_after, size.left, size.top, size.right, size.bottom, flags))
    (void)ui_win32_assert_on_error (SetWindowPos);
}

void
ui_window_set_position (struct ui_window_t *window, const int x, const int y)
{
  assert (window != NULL);

  static const UINT flags = SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER;

  if (!SetWindowPos (window->handle, NULL, x, y, 0, 0, flags))
    (void)ui_win32_assert_on_error (SetWindowPos);
}

void
ui_window_set_resizable (struct ui_window_t *window, const int resizable)
{
  assert (window != NULL);

  if (resizable)
    ui_win32_window_set_attribute (window, GWL_STYLE, WS_THICKFRAME | WS_MAXIMIZEBOX);

  else
    ui_win32_window_unset_attribute (window, GWL_STYLE, WS_THICKFRAME | WS_MAXIMIZEBOX);
}

void
ui_window_set_title (struct ui_window_t *window, const char *title)
{
  assert (window != NULL);

  if (!SetWindowTextA (window->handle, title))
    (void)ui_win32_assert_on_error (SetWindowTextA);
}

void
ui_window_show (struct ui_window_t *window)
{
  assert (window != NULL);

  if (ShowWindow (window->handle, SW_SHOW) == 0)
    ui_win32_assert_on_error (ShowWindow);
}

#pragma endregion
