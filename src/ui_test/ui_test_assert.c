#include <ui_test_assert.h>

#include <assert.h>
#include <stdlib.h>

#pragma region structure

struct ui_test_assert_t
ui_test_assert_init (struct ui_test_t *test, const char *file, const char *func, const int line,
                     const struct ui_test_compare_t compare)
{
  return (struct ui_test_assert_t){ test, file, func, line, compare };
}

#pragma endregion

int
ui_test_assert (struct ui_test_assert_t data)
{
  assert (data.test != NULL);

  struct ui_test_t *test = data.test;
  test->backtrace.file   = data.file;
  test->backtrace.line   = data.line;

  assert (data.compare.compare != NULL);
  const int diff = data.compare.compare (&data.compare);

  assert (data.compare.message != NULL);
  test->backtrace.message = data.compare.message (&data.compare);

  test->status = diff == 0 ? UI_TEST_STATUS_PASSED : UI_TEST_STATUS_FAILED;
  return test->status == UI_TEST_STATUS_PASSED;
}

static ui_test_case
ui_test_assert_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_assert_test);

  double m1 = 0;
  double m2 = 0;

  ui_assert_eq (&m1, &m2);

  ui_assert_eq (1, 1);
}
