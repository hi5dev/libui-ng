#include <windows.h>

#include <ui_test_expect.h>
#include <ui_win32.h>
#include <ui_win32_error.h>
#include <ui_win32_window.h>

#include <assert.h>

HMENU
ui_win32_window_create_menu (struct ui_window_t *) { return NULL; }

static ui_test_case
ui_win32_window_create_menu_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_window_create_menu_test);

  ui_test_skip ("TODO");
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
    ui_win32_abort_on_error ("GetClassInfoEx");
  SetLastError (ERROR_SUCCESS);

  // a handle to the class cursor - must be a handle to a cursor resource - NULL requires the application to set the
  // the cursor shape whenever the mouse moves into the window
  wc.hCursor = LoadCursor (NULL, IDC_ARROW);
  if (wc.hCursor == NULL)
    ui_win32_abort_on_error ("LoadCursor");

  // handle to the class icon -  must be a handle to an icon resource, or NULL for a system-provided default icon
  wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  if (wc.hIcon == NULL)
    (void)ui_win32_log_last_error ("LoadIcon");

  // pointer to a null-terminated string that specifies the window class name
  wc.lpszClassName = UI_WIN32_WINDOW_CLASS;

  // pointer to the window procedure (WindowProc)
  wc.lpfnWndProc = ui_win32_window_procedure;

  // handle to the instance that contains the window procedure for the class
  wc.hInstance = instance;

  // handle to the class background brush
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

  // register the class, or abort on failure
  if (RegisterClassEx (&wc) == 0)
    ui_win32_abort_on_error ("RegisterClassEx");
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

  struct ui_window_t *window = calloc (1, sizeof (*window));

  window->handle = CreateWindowEx (exended_styles, UI_WIN32_WINDOW_CLASS, default_title, style, default_x, default_y,
                                   width, height, default_parent, default_menu, instance, window);

  if (window->handle == NULL)
    {
      ui_win32_abort_on_error ("CreateWindowEx");
      return NULL;
    }

  if (has_menu)
    {
      window->menu = ui_win32_window_create_menu (window);

      if (!SetMenu (window->handle, window->menu))
        ui_win32_abort_on_error ("SetMenu");

      assert (window->menu != NULL);
    }

  ui_window_set_title (window, title);
  ui_window_set_content_size (window, width, height);

  return window;
}

void
ui_window_destroy (struct ui_window_t *window)
{
  free (window);
}

void
ui_window_hide (struct ui_window_t *window)
{
  if (ShowWindow (window->handle, SW_HIDE) == 0)
    (void)ui_win32_log_last_error ("ShowWindow:SW_HIDE");
}

void
ui_window_set_content_size (struct ui_window_t *window, const int width, const int height)
{
  assert (window != NULL);

  const DWORD style = GetWindowLongPtr (window->handle, GWL_STYLE);
  if (style == 0)
    {
      (void)ui_win32_log_last_error ("GetWindowLongPtr");
      return;
    }

  const DWORD style_ex = GetWindowLongPtr (window->handle, GWL_EXSTYLE);
  if (style_ex == 0)
    {
      (void)ui_win32_log_last_error ("GetWindowLongPtr");
      return;
    }

  RECT      rect     = { 0, 0, width, height };
  const int has_menu = window->menu != NULL;
  if (AdjustWindowRectEx (&rect, style, has_menu, style_ex) == 0)
    {
      (void)ui_win32_log_last_error ("AdjustWindowRectEx");
      return;
    }

  if (!has_menu)
    {
      RECT rect_cpy   = rect;
      rect_cpy.bottom = 0x7FFF;
      SetLastError (ERROR_SUCCESS);
      SendMessage (window->handle, WM_NCCALCSIZE, FALSE, (LPARAM)&rect_cpy);
      (void)ui_win32_log_last_error ("SendMessage:WM_NCCALCSIZE");
      rect.bottom += rect_cpy.top;
    }

  static const int flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
  if (SetWindowPos (window->handle, NULL, 0, 0, rect.right, rect.bottom, flags) == 0)
    (void)ui_win32_log_last_error ("SetWindowPos");
}

void
ui_window_set_title (struct ui_window_t *window, const char *title)
{
  assert (window != NULL);

  if (!SetWindowTextA (window->handle, title))
    (void)ui_win32_log_last_error ("SetWindowTextA");
}

void
ui_window_show (struct ui_window_t *window)
{
  if (ShowWindow (window->handle, SW_SHOW) == 0)
    (void)ui_win32_log_last_error ("ShowWindow:SW_SHOW");

}
