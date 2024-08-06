#pragma once

#include <minwindef.h>
#include <winuser.h>

/**
 * @brief Logs a Win32 API error.
 * @param message with details about the error.
 * @see @ref _ui_win32_log_last_error
 */
#define ui_win32_log_last_error(message) _ui_win32_log_last_error (message, __FILE__, __LINE__)

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
 * @brief Logs a Win32 API error to the console for debug builds, or to the event viewer for release builds.
 * @param message with details about the error.
 * @param file of the source.
 * @param line number of the caller.
 */
void _ui_win32_log_last_error (const char *message, const char *file, int line);

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
