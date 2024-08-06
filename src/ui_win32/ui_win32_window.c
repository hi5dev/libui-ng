#include <assert.h>
#include <windows.h>

#include <ui_win32.h>
#include <ui_win32_window.h>

void
ui_win32_window_register_class (const HINSTANCE instance)
{
  WNDCLASSEX wc = {};

  if (GetClassInfoEx (instance, UI_WIN32_WINDOW_CLASS, &wc))
    return;

  wc.lpszClassName = UI_WIN32_WINDOW_CLASS;
  wc.lpfnWndProc   = ui_win32_window_procedure;
  wc.hInstance     = instance;
  wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

  wc.hCursor = LoadCursor (NULL, IDC_ARROW);
  if (wc.hCursor == NULL)
    ui_win32_log_last_error ("LoadCursor");

  wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  if (wc.hIcon == NULL)
    ui_win32_log_last_error ("LoadIcon");

  if (RegisterClassEx (&wc) == 0)
    ui_win32_log_last_error ("RegisterClassEx");
}

struct ui_window_t *
ui_window_create (const char *title, const int width, const int height, const int has_menu)
{
  return NULL;
  // static const HMENU  default_menu   = NULL;
  // static const HWND   default_parent = NULL;
  // static const WCHAR *default_title  = L"";
  // static const int    default_x      = CW_USEDEFAULT;
  // static const int    default_y      = CW_USEDEFAULT;
  // static const int    style          = WS_OVERLAPPEDWINDOW;
  // static const int    exended_styles = 0;
  //
  // const HINSTANCE instance = GetModuleHandle (NULL);
  //
  // ui_win32_window_register_class (instance);
  //
  // struct ui_window_t *window = calloc (1, sizeof (*window));
  //
  // window->handle = CreateWindowEx (exended_styles, UI_WIN32_WINDOW_CLASS, default_title, style, default_x, default_y,
  //                                  width, height, default_parent, default_menu, instance, window);
  //
  // if (window->handle == NULL)
  //   {
  //     ui_win32_log_last_error (L"CreateWindowEx");
  //     return NULL;
  //   }
  //
  // if (has_menu)
  //   {
  //     window->menu = ui_win32_window_create_menu (window);
  //     if (!SetMenu (window->handle, window->menu))
  //       ui_win32_log_last_error (L"SetMenu");
  //
  //     assert (window->menu != NULL);
  //   }
  //
  // ui_window_set_title (window, title);
  // ui_window_set_content_size (window, width, height);
  //
  // return window;
}

void
ui_window_destroy (struct ui_window_t *window)
{
  free (window);
}

LRESULT CALLBACK
ui_win32_window_procedure (const HWND handle, const UINT message, const WPARAM wparam, const LPARAM lparam)
{
  return DefWindowProc (handle, message, wparam, lparam);
}
