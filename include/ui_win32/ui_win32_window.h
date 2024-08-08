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
 * @brief Gets the state of specific window attribute(s).
 * @param window @p ui_window_t
 * @param index zero-based offset of the value to get.
 * @param attribute value of the attribute(s) in question.
 * @return non-zero when the given attribute(s) are set.
 */
LONG ui_win32_window_get_attribute (struct ui_window_t *window, int index, LONG attribute);

/**
 * @brief Gets a window's attributes.
 * @param window @p ui_window_t
 * @param index zero-based offset of the value to get.
 * @return window attributes.
 */
LONG ui_win32_window_get_attributes (struct ui_window_t *window, int index);

/**
 * @brief Gets a window's position and/or size.
 * @param window @p ui_window_t
 * @param client_size non-zero to get the window's client-rect.
 * @param[out] x position
 * @param[out] y position
 * @param[out] width size
 * @param[out] height size
 * @remark Any of the given pointers can be @p NULL to ignore.
 */
void ui_win32_window_get_rect (struct ui_window_t *window, int client_size, int *x, int *y, int *width, int *height);

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

/**
 * @brief Adds a window flag.
 * @param window @p ui_window_t
 * @param index zero-based offset of the value to set.
 * @param attribute value of the attribute to set.
 * @return updated window attributes.
 */
LONG ui_win32_window_set_attribute (struct ui_window_t *window, int index, LONG attribute);

/**
 * @brief Sets a window's attributes..
 * @param window @p ui_window_t
 * @param index zero-based offset of the value to set.
 * @param attributes updated window attributes.
 */
void ui_win32_window_set_attributes (struct ui_window_t *window, int index, LONG attributes);

/**
 * @brief Removes an attribute from a window.
 * @param window @p ui_window_t
 * @param index zero-based offset of the value to clear.
 * @param attribute value of the attribute to clear.
 * @return updated window attributes.
 */
LONG ui_win32_window_unset_attribute (struct ui_window_t *window, int index, LONG attribute);
