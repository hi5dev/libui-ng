#pragma once

#include <minwindef.h>
#include <winuser.h>

#include <stdio.h>

/**
 * @brief Logs a Win32 API error.
 * @param message with details about the error.
 * @see @ref _ui_win32_log_last_error
 */
#define ui_win32_log_last_error(message) _ui_win32_log_last_error (stderr, message, __FILE__, __LINE__)

/**
 * @brief Logs the last Win32 API non-error and aborts.
 * @param message with details about the error.
 * @remark Does nothing if @p GetLastError returns a non-error code (i.e. ERROR_SUCCESS).
 */
#define ui_win32_abort_on_error(message)                                                                              \
  if (ui_win32_log_last_error (message) != ERROR_SUCCESS)                                                             \
    abort ();

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
 * @brief Flags used to extract a human-readable message from the @p GetLastError Win32 API function.
 */
static const int UI_WIN32_ERROR_MESSAGE_FLAGS
    = FORMAT_MESSAGE_ALLOCATE_BUFFER | // allocate a buffer large enough to hold the formatted message
      FORMAT_MESSAGE_FROM_SYSTEM |     // search the system message-table resource(s) for the requested message
      FORMAT_MESSAGE_IGNORE_INSERTS |  // insert sequences in the messages, such as %1, are to be ignored
      0;

/**
 * @brief Language identifier used for translating system messages.
 */
static const DWORD UI_WIN32_NEUTRAL_LANGUAGE_ID = MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT);

/**
 * @brief Logs a Win32 API error to the console for debug builds, or to the event viewer for release builds.
 * @param stream to write the error.
 * @param message with details about the error.
 * @param file of the source.
 * @param line number of the caller.
 * @return The last Win32 API error code.
 */
DWORD _ui_win32_log_last_error (FILE *stream, const char *message, const char *file, int line);

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
