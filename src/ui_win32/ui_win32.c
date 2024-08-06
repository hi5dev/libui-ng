#include <windows.h>

#include <ui_test.h>
#include <ui_test_expect.h>
#include <ui_win32.h>

#include <assert.h>
#include <strsafe.h>

static char *
ui_win32_get_last_error (const char *context, const char *file, const int line)
{
  const DWORD last_error = GetLastError ();
  if (last_error == NO_ERROR)
    return NULL;

  static const int flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

  char *buffer = NULL;
  FormatMessageA (flags, NULL, last_error, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buffer, 0, NULL);
  if (buffer == NULL)
    return NULL;

  const char  *message_format = "%s:%d [%s]: %s";
  const size_t message_length = snprintf (NULL, 0, message_format, file, line, context, buffer);
  char        *message        = malloc (message_length + 1);

  if (message != NULL)
    {
      (void)snprintf (message, message_length + 1, message_format, file, line, context, buffer);

      // remove trailing CRLF when present
      for (int i = message_length - 1; i > 0 && message[i] == '\n' || message[i] == '\r'; --i)
        message[i] = '\0';
    }

  LocalFree (buffer);

  return message;
}

static ui_test_case
ui_win32_get_last_error_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_get_last_error_test);

  SetLastError (ERROR_INVALID_HANDLE);
  char *message = ui_win32_get_last_error ("test", "file", 99);
  SetLastError (NO_ERROR);

  static const char *expected = "file:99 [test]: The handle is invalid.";
  ui_expect_cmp (str, message, is, expected);

  free (message);
}

void
_ui_win32_log_last_error (const char *message, const char *file, const int line)
{
  char *log_message = ui_win32_get_last_error (message, file, line);

  (void)fprintf (stderr, "%s\n", log_message);

  free (log_message);
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
