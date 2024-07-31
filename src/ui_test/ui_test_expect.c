#include <ui_test.h>
#include <ui_test_expect.h>

#include <assert.h>
#include <stdlib.h>

void
_ui_expect (struct ui_test_t *test, const char *message, const char *filename, const int line)
{
  assert (test != NULL);

  test->backtrace.message  = message;
  test->backtrace.filename = filename;
  test->backtrace.line     = line;
}
