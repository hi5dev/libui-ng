#include "ui_event_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
_ui_event_perror (const int code, FILE *stream, const char *file, const char *func, const int line)
{
  errno = code;

  char *err = errno == 0 ? NULL : strerror (errno);

  if (err == NULL)
    fprintf (stream, "%s:%d: %s: %s\n", file, line, func, "failed (errno not set)");

  else
    fprintf (stream, "%s:%d: %s: %s\n", file, line, func, err);

  free (err);
}
