#pragma once

#include <windef.h>

#include <ui_window.h>

/// @brief Win32 API window class name.
#define UI_WIN32_WINDOW_CLASS L"ui_window_class"

/// @brief Window margin in pixels.
#define UI_WIN32_WINDOW_MARGIN 8

/// @brief Win32 implementation of @p ui_window_t
struct ui_window_t
{
  /// @brief Win32 API window handle.
  HWND handle;

  /// @brief Win32 API menu handle.
  HMENU menu;
};

/**
 * @brief Creates the window's main menu bar.
 * @param window @p ui_window_t
 * @return Win32 API menu handle.
 */
HMENU ui_win32_window_create_menu (struct ui_window_t *window);

/**
 * @brief Win32 API @p WNDPROC callback.
 * @param handle to the window provided by the Win32 API when it was created.
 * @param message system-defined message sent from the API to communicate with the window.
 * @param wparam pointer to an unsigned integer.
 * @param lparam signed long-pointer.
 * @return the result of the message processing.
 */
LRESULT CALLBACK ui_win32_window_procedure (HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

/**
 * @brief Registers a window class with the Win32 API.
 * @param instance of the application that's registering the class.
 * @remark @p instance must not be @p NULL
 */
void ui_win32_window_register_class (HINSTANCE instance);
