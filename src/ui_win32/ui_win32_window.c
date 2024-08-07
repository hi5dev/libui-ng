#include <assert.h>
#include <windows.h>

#include <ui_win32.h>
#include <ui_win32_window.h>

void
ui_win32_window_register_class (const HINSTANCE instance)
{
  WNDCLASSEX wc = {};

  SetLastError (ERROR_SUCCESS);
  if (GetClassInfoEx (instance, UI_WIN32_WINDOW_CLASS, &wc))
    {
      if (ui_win32_log_last_error ("GetClassInfoEx") != ERROR_SUCCESS)
        abort ();

      return;
    }

  wc.lpszClassName = UI_WIN32_WINDOW_CLASS;
  wc.lpfnWndProc   = ui_win32_window_procedure;
  wc.hInstance     = instance;
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

  wc.hCursor = LoadCursor (NULL, IDC_ARROW);
  if (wc.hCursor == NULL && ui_win32_log_last_error ("LoadCursor") != ERROR_SUCCESS)
    abort ();

  wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  if (wc.hIcon == NULL && ui_win32_log_last_error ("LoadIcon") != ERROR_SUCCESS)
    abort ();

  if (RegisterClassEx (&wc) == 0 && ui_win32_log_last_error ("RegisterClassEx") != ERROR_SUCCESS)
    abort ();
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
  if (instance == NULL && ui_win32_log_last_error ("GetModuleHandle") != ERROR_SUCCESS)
    abort ();

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
ui_window_set_title (struct ui_window_t *window, const char *title)
{
  assert (window != NULL);

  if (!SetWindowTextA (window->handle, title))
    (void)ui_win32_log_last_error ("SetWindowTextA");
}

void
ui_window_set_content_size (struct ui_window_t *window, const int width, const int height)
{
  assert (window != NULL);

  const DWORD style = GetWindowLongPtr (window->handle, GWL_STYLE);
  if (style == 0)
    {
      ui_win32_log_last_error ("GetWindowLongPtr");
      return;
    }

  const DWORD style_ex = GetWindowLongPtr (window->handle, GWL_EXSTYLE);
  if (style_ex == 0)
    {
      ui_win32_log_last_error ("GetWindowLongPtr");
      return;
    }

  RECT      rect     = { 0, 0, width, height };
  const int has_menu = window->menu != NULL;
  if (AdjustWindowRectEx (&rect, style, has_menu, style_ex) == 0)
    {
      ui_win32_log_last_error ("AdjustWindowRectEx");
      return;
    }

  if (!has_menu)
    {
      RECT rect_cpy   = rect;
      rect_cpy.bottom = 0x7FFF;
      SetLastError (ERROR_SUCCESS);
      SendMessage (window->handle, WM_NCCALCSIZE, FALSE, (LPARAM)&rect_cpy);
      ui_win32_log_last_error ("SendMessage:WM_NCCALCSIZE");
      rect.bottom += rect_cpy.top;
    }

  static const int flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
  if (SetWindowPos (window->handle, NULL, 0, 0, rect.right, rect.bottom, flags) == 0)
    ui_win32_log_last_error ("SetWindowPos");
}

LRESULT CALLBACK
ui_win32_window_procedure (const HWND handle, const UINT message, const WPARAM wparam, const LPARAM lparam)
{
  return DefWindowProc (handle, message, wparam, lparam);
}
