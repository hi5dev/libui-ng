#include <ui_test_debug.h>

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void
_ui_test_debug_errno (FILE *stream, const char *context, const char *file, const int line)
{
  assert (stream != NULL);
  assert (context != NULL);
  assert (file != NULL);

  char *message = errno == 0 ? "errno not set" : strerror (errno);

  fprintf (stream, "%s:%d: %s: %s\n", file, line, context, message);

  if (errno != 0)
    free (message);
}
