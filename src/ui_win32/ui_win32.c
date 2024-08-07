#include <windows.h>

#include <ui_test.h>
#include <ui_test_expect.h>
#include <ui_win32.h>

#include <assert.h>
#include <strsafe.h>

/**
 * @brief Gets a the system message for the last error returned by the Win32 API @p GetLastError function.
 * @param[out] dst message buffer - must be a valid pointer to a @p NULL char array.
 * @param[in] context message to include with the formatted Win32 API error message.
 * @param[in] file of the source.
 * @param[in] line number of the source.
 * @return @p GetLastError return value.
 */
static DWORD
ui_win32_get_last_error (char **dst, const char *context, const char *file, const int line)
{
  assert (dst != NULL && *dst == NULL);

  // example: "file:line [context]: last error message"
  static const char *message_format = "%s:%d [%s]: %s";

  // retrieve the calling thread's last-error code value
  const DWORD err = GetLastError ();

  // ignore non-errors
  if (err == ERROR_SUCCESS)
    return err;

  // allocate a buffer and get the system message using the Win32 API
  char *buffer = NULL;
  FormatMessageA (UI_WIN32_ERROR_MESSAGE_FLAGS, NULL, err, UI_WIN32_NEUTRAL_LANGUAGE_ID, (LPSTR)&buffer, 0, NULL);
  if (buffer == NULL)
    return err;

  // create another message that includes the provided context in the message_format described above
  const size_t message_length = snprintf (NULL, 0, message_format, file, line, context, buffer);
  char        *message        = malloc (message_length + 1);
  if (message != NULL)
    {
      (void)snprintf (message, message_length + 1, message_format, file, line, context, buffer);

      // the Win32 API sometimes includes a CRLF, but not always
      for (int i = message_length - 1; i > 0 && message[i] == '\n' || message[i] == '\r'; --i)
        message[i] = '\0';
    }

  // the buffer has been copied into the message
  LocalFree (buffer);

  // null when out of memory
  *dst = message;

  return err;
}

static ui_test_case
ui_win32_get_last_error_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_get_last_error_test);

  char *message = NULL;

  static const char *expected = "file:99 [test]: The handle is invalid.";

  SetLastError (ERROR_INVALID_HANDLE);
  int err = ui_win32_get_last_error (&message, "test", "file", 99);
  SetLastError (ERROR_SUCCESS);

  ui_expect_cmp (int, err, is, 1);
  ui_expect_not_null (message);
  ui_expect_cmp (str, message, is, expected);

  free (message);

  message = NULL;
  err     = ui_win32_get_last_error (&message, "test", "file", 101);
  ui_expect_cmp (int, err, is, 0);
  ui_expect_null (message);
}

DWORD
_ui_win32_log_last_error (FILE *stream, const char *message, const char *file, const int line)
{
  char *log_message = NULL;

  const DWORD last_error = ui_win32_get_last_error (&log_message, message, file, line);
  if (last_error == ERROR_SUCCESS)
    return last_error;

  if (log_message == NULL)
    {
      // provide a fallback message when out of memory
      (void)fprintf (stream, "%s:%d [%s]: error code %lu\n", file, line, message, GetLastError ());
    }
  else
    {
      (void)fprintf (stream, "%s\n", log_message);
      free (log_message);
    }

  return last_error;
}

void
ui_win32_dispatch (struct ui_win32_t *ui_win32)
{
  TranslateMessage (&ui_win32->message);

  DispatchMessage (&ui_win32->message);
}

void
ui_win32_update (struct ui_win32_t *ui_win32)
{
  if (GetMessage (&ui_win32->message, NULL, 0, 0) != 0)
    ui_win32->quit = TRUE;
}

static ui_test_case
ui_win32_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_test);

  ui_test_skip ("TODO");
}
