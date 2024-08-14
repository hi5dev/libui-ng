#include <ui_test_debug.h>
#include <ui_test_expect.h>
#include <ui_test_time.h>

#include <errno.h>
#include <stdio.h>
#include <time.h>

int
ui_test_msleep (const int ms)
{
  if (ms < 0)
    {
      errno = EINVAL;
      ui_test_debug_errno (__FUNCTION__);
      return -1;
    }

  static const long one_second_ms     = 1000;
  static const long one_nanosecond_ms = 1000000;
  struct timespec   ts = { .tv_sec = ms / one_second_ms, .tv_nsec = ms % one_second_ms * one_nanosecond_ms };

  int success;

  do
    success = nanosleep (&ts, &ts);
  while (success && errno == EINTR);

  if (success == -1)
    ui_test_debug_errno ("nanosleep");

  return success;
}

static ui_test_case
ui_test_msleep_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_msleep_test);

  const int result = ui_test_msleep (1);
  ui_expect_cmp (int, result, is, 0);
}
