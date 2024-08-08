#pragma once

#include <windows.h>

/**
 * @brief Win32-specific runtime options.
 */
struct ui_win32_options_t
{
  /**
   * @brief Default behavior when switching to fullscreen mode and the window doesn't intersect any display monitor.
   *
   * Possible values:
   * - `MONITOR_DEFAULTTONEAREST` - use the nearest monitor (default)
   * - `MONITOR_DEFAULTTOPRIMARY` - use the primary monitor
   */
  DWORD fullscreen_monitor;
};

/**
 * @brief Resets the given @p ui_win32_options_t instance to the default values.
 * @param options to update with the default values.
 */
void ui_win32_options_init (struct ui_win32_options_t *options);
