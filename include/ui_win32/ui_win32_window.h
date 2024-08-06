#pragma once

#include <windef.h>

#include <ui_window.h>

#define UI_WIN32_WINDOW_CLASS L"ui_window_class"

#define UI_WIN32_WINDOW_MARGIN 7

struct ui_window_t
{
  HWND handle;

  HMENU menu;
};

void ui_win32_window_register_class (HINSTANCE instance);

HMENU ui_win32_window_create_menu (struct ui_window_t *window);

LRESULT CALLBACK ui_win32_window_procedure (HWND handle, UINT message, WPARAM wparam, LPARAM lparam);
