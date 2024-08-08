#include <windef.h>

#include <winbase.h>

#include <ui_test_expect.h>
#include <ui_win32_error.h>

#include <assert.h>

/// @brief Flags used to extract a human-readable message from the @p GetLastError Win32 API function.
static const int UI_WIN32_ERROR_MESSAGE_FLAGS
    = FORMAT_MESSAGE_ALLOCATE_BUFFER | // allocate a buffer large enough to hold the formatted message
      FORMAT_MESSAGE_FROM_SYSTEM |     // search the system message-table resource(s) for the requested message
      FORMAT_MESSAGE_IGNORE_INSERTS |  // insert sequences in the messages, such as %1, are to be ignored
      0;

/// @brief Language identifier used for translating system messages.
static const DWORD UI_WIN32_NEUTRAL_LANGUAGE_ID = MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT);

struct ui_win32_error_t
ui_win32_get_last_error (const char *context, const char *file, const int line)
{
  // example: "file:line [context]: last error message"
  static const char *message_format = "%s:%d [%s]: %s";

  // get the calling thread's last error-code, create an empty buffer for the message, and store the backtrace info
  struct ui_win32_error_t error = { GetLastError (), NULL, file, line };

  // buffer for the Win32 API system message - allocated by FormatMessageA using LocalAlloc (maximum of 64K bytes)
  char *buf = NULL;

  // retreives the system message for error.code, translates, formats it, allocates buf, copies the message into buf
  const int buf_size = FormatMessageA (UI_WIN32_ERROR_MESSAGE_FLAGS, NULL, error.code, UI_WIN32_NEUTRAL_LANGUAGE_ID,
                                       (LPSTR)&buf, 0, NULL);

  // FormatMessageA returns the number of TCHARs stored in the output buffer, excluding the terminating null
  // character, or zero on failure.
  if (buf_size == 0)
    (void)fprintf (stderr, "%s:%d [%s, %s]: error #%lu\n", file, line, context, "FormatMessageA", GetLastError ());

  // cover some edge-cases
  assert (buf_size > 0);
  assert (buf != NULL);

  // get the length of the formatted message
  const size_t message_length = snprintf (NULL, 0, message_format, file, line, context, buf);

  // should have allocated enough memory for the provided context, error message, and backtrace info
  assert (message_length >= buf_size + strlen (context) + strlen (file) + 1);

  // allocate memory for another message that includes the provided context in the message_format described above
  error.message = LocalAlloc (LPTR, sizeof (TCHAR) * (message_length + 1));
  if (error.message == NULL)
    (void)fprintf (stderr, "%s:%d [%s, LocalAlloc]: error #%lu\n", file, line, context, GetLastError ());
  else
    // write the error message to the struct with the provided context
    (void)snprintf (error.message, message_length + 1, message_format, file, line, context, buf);

  // the buffer has been copied into the message
  if (LocalFree (buf) != NULL)
    (void)fprintf (stderr, "%s:%d [%s, LocalFree]: failed with error code %lu", file, line, context, GetLastError ());

  // the Win32 API sometimes includes a CR/LF, but not always
  for (int i = message_length - 1; i > 0 && error.message[i] == '\r' || error.message[i] == '\n'; --i)
    error.message[i] = '\0';

  return error;
}

static ui_test_case
ui_win32_get_last_error_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_get_last_error_test);

  SetLastError (ERROR_INVALID_HANDLE);
  struct ui_win32_error_t error = ui_win32_get_last_error ("test", "file", 99);
  SetLastError (ERROR_SUCCESS);

  ui_expect_cmp (int, error.code, is, ERROR_INVALID_HANDLE);
  ui_expect_not_null (error.message);
  ui_expect_cmp (str, error.message, is, "file:99 [test]: The handle is invalid.");

  ui_expect_null (LocalFree (error.message));
  error = ui_win32_get_last_error ("test", "file", 101);

  ui_expect_cmp (int, error.code, is, 0);
  ui_expect_cmp (str, error.message, is, "file:101 [test]: The operation completed successfully.");
}

DWORD
_ui_win32_log_last_error (FILE *stream, const char *context, const char *file, const int line)
{
  const struct ui_win32_error_t error = ui_win32_get_last_error (context, file, line);

  // do not log when disabled, or if GetLastError returned a non-error code
  if (stream != NULL && error.code != ERROR_SUCCESS)
    (void)fprintf (stream, "%s\n", error.message);

  if (LocalFree (error.message) != NULL)
    (void)fprintf (stream, "%s:%d [LocalFree]: failed with error code %lu", file, line, GetLastError ());

  return error.code;
}

static ui_test_case
ui_win32_log_last_error_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_log_last_error_test);

  // get full path to a temporary file storage directory that doesn't require elevated privileges
  char tmpfile_path[BUFSIZ];
  ui_expect_cmp (int, GetTempPathA (BUFSIZ, tmpfile_path), is_not, 0);

  // generate a unique temporary file name
  char tmpfile_name[BUFSIZ];
  ui_expect_cmp (int, GetTempFileNameA (tmpfile_path, "tmpfile", 0, tmpfile_name), is_not, 0);

  // log to a temporary file so the content can be tested
  FILE *tmpfile = fopen (tmpfile_name, "w");
  ui_expect_not_null (tmpfile);

  // log a Win32 API error to the temporary file
  SetLastError (ERROR_INVALID_HANDLE);
  _ui_win32_log_last_error (tmpfile, "test", "file", 99);
  SetLastError (ERROR_SUCCESS);

  // close the file and reopen it with read-only access to ensure the data has persisted
  fclose (tmpfile);
  tmpfile = fopen (tmpfile_name, "rb");
  ui_expect_not_null (tmpfile);

  // get the file's size
  fseek (tmpfile, 0, SEEK_END);
  const long length = ftell (tmpfile);
  fseek (tmpfile, 0, SEEK_SET);

  // read the entire file into memory
  char *actual = malloc (length + 1);
  ui_expect_not_null (actual);
  fread (actual, 1, length, tmpfile);

  // close and delete the temporary file
  fclose (tmpfile);
  remove (tmpfile_name);

  // add null-terminator, and remove CR/LF when present
  actual[length] = '\0';
  for (int i = length - 1; i > 0 && (actual[i] == '\r' || actual[i] == '\n'); --i)
    actual[i] = '\0';

  // test the expected log message and free the file content buffer
  ui_expect_cmp (str, actual, is, "file:99 [test]: The handle is invalid.");
  free (actual);
}
