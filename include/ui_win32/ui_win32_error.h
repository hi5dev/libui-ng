#pragma once

#include <windef.h>

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
  do                                                                                                                  \
    {                                                                                                                 \
      if (ui_win32_log_last_error (message) != ERROR_SUCCESS)                                                         \
        abort ();                                                                                                     \
    }                                                                                                                 \
  while (0)

/**
 * @brief Extended information about a Win32 API error.
 * @see @ref ui_win32_get_last_error
 */
struct ui_win32_error_t
{
  /// @brief Win32 error code.
  DWORD code;

  /// @brief Win32 system message for the error.
  char *message;

  /// @brief Source file with the error.
  const char *file;

  /// @brief Source line number with the error.
  int line;
};

/**
 * @brief Gets a the system message for the last error returned by the Win32 API @p GetLastError function.
 * @param[in] context message to include with the formatted Win32 API error message.
 * @param[in] file of the source.
 * @param[in] line number of the source.
 * @return @p ui_win32_error_t
 * @remark The returned @p ui_win32_error_t.message needs to be freed by the caller.
 */
struct ui_win32_error_t ui_win32_get_last_error (const char *context, const char *file, int line);

/**
 * @brief Logs a Win32 API error to the console for debug builds, or to the event viewer for release builds.
 * @param stream to write the error.
 * @param context message to include with the formatted Win32 API error message.
 * @param file of the source.
 * @param line number of the caller.
 * @return Last Win32 API error code.
 */
DWORD _ui_win32_log_last_error (FILE *stream, const char *context, const char *file, int line);
