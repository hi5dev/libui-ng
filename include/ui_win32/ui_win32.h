#pragma once

#include <minwindef.h>
#include <winuser.h>

/**
 * @brief Win32 implementation.
 */
struct ui_win32_t
{
  /**
   * @brief Handle to the application's instance.
   */
  HINSTANCE handle;

  /**
   * @brief Contains message information from a thread's message queue.
   */
  MSG message;

  /**
   * @brief Set when the Win32 API indicates its time to shutdown the application.
   */
  BOOL quit;
};

/**
 * @brief Dispatches a Win32 API message.
 * @param ui_win32 @p ui_win32_t
 */
void ui_win32_dispatch (struct ui_win32_t *ui_win32);

/**
 * @brief Blocks the calling thread until a new Win32 API message is ready for dispatch.
 * @param ui_win32 @p ui_win32_t
 */
void ui_win32_update (struct ui_win32_t *ui_win32);
