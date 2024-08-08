#include <ui_test.h>
#include <ui_test_assert.h>

#include "ui_test_ulp.h"

#include <assert.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void
ui_test_assert_set_status (struct ui_test_t *test, const enum ui_test_status_t status, const char *file,
                           const int line, const char *fmt, ...)
{
  assert (test != NULL);

  test->backtrace.filename = file;
  test->backtrace.line     = line;
  test->status             = status;

  if (test->backtrace.message != NULL)
    free (test->backtrace.message);

  test->backtrace.message = NULL;

  if (fmt == NULL)
    return;

  va_list args;

  va_start (args, fmt);
  const size_t message_size = vsnprintf (NULL, 0, fmt, args);
  va_end (args);

  va_start (args, fmt);
  test->backtrace.message = (char *)malloc (message_size + 1);
  assert (test->backtrace.message != NULL);

  vsnprintf (test->backtrace.message, message_size + 1, fmt, args);
  va_end (args);
}

void
ui_test_assert_generic (struct ui_test_t *test, ui_test_assert_cmp_func *a, const int eq, const void *actual,
                        const void *expected, const char *file, const int line, const char *fmt, const char *to_be,
                        const char *not_to_be)
{
  assert (test != NULL);
  assert (a != NULL);

  const volatile int diff = a (actual, expected);

  if (eq != 0 == (diff == 0))
    {
      ui_test_assert_set_status (test, UI_TEST_STATUS_PASSED, file, line, NULL, NULL);
      return;
    }

  const char *equal = eq ? to_be : not_to_be;

  ui_test_assert_set_status (test, UI_TEST_STATUS_FAILED, file, line, fmt, expected, equal, actual);
}

#define ui_test_assert_eq_define(type_name, type, fmt, to_be, not_to_be, ...)

ui_test_assert_eq_define (bool, const int, "expected %d %s", "to be true", "to be false", actual)

    void ui_test_assert_eq_bool (struct ui_test_t *test, const int eq, const int actual, const int expected,
                                 char *file, const int line)
{
  ui_test_assert_generic (test, ui_test_assert_cmp_bool, eq, &actual, &expected, file, line, "", "to be true",
                          "to be false");
}

int
ui_test_assert_cmp_bool (const int l, const int r)
{
  return l == r ? 0 : l != 0 ? 1 : -1;
}

static ui_test_case
ui_test_assert_cmp_bool_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_assert_cmp_bool_test);

  assert (ui_test_assert_cmp_bool (0, 0) == 0);
  assert (ui_test_assert_cmp_bool (1, 1) == 0);
  assert (ui_test_assert_cmp_bool (1, 0) == 1);
  assert (ui_test_assert_cmp_bool (0, 1) == -1);
}

int
ui_test_assert_cmp_double (const double l, const double r)
{
  return ui_test_ulp_compare_double (l, r);
}

static ui_test_case
ui_test_assert_cmp_double_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_assert_cmp_double_test);

  assert (ui_test_assert_cmp_double (0, 0) == 0);
  assert (ui_test_assert_cmp_double (1, 1) == 0);
  assert (ui_test_assert_cmp_double (0.001, 0.002) == -1);
  assert (ui_test_assert_cmp_double (0.002, 0.001) == 1);
}

int
ui_test_assert_cmp_float (const float l, const float r)
{
  return ui_test_ulp_compare_float (l, r);
}

static ui_test_case
ui_test_assert_cmp_float_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_assert_cmp_float_test);

  assert (ui_test_assert_cmp_float (0.0f, 0.0f) == 0);
  assert (ui_test_assert_cmp_float (1.0f, 1.0f) == 0);
  assert (ui_test_assert_cmp_float (0.001f, 0.002f) == -1);
  assert (ui_test_assert_cmp_float (0.002f, 0.001f) == 1);
}

int
ui_test_assert_cmp_int (const int l, const int r)
{
  return l == r ? 0 : l < r ? -1 : 1;
}

static ui_test_case
ui_test_assert_cmp_int_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_assert_cmp_int_test);

  assert (ui_test_assert_cmp_int (0, 0) == 0);
  assert (ui_test_assert_cmp_int (1, 1) == 0);
  assert (ui_test_assert_cmp_int (0, 1) == -1);
  assert (ui_test_assert_cmp_int (1, 0) == 1);
}

int
ui_test_assert_cmp_string (char *l, char *r)
{
  if (l == NULL || r == NULL)
    return l == r ? 0 : l != NULL ? 1 : -1;

  return strcmp (l, r);
}

static ui_test_case
ui_test_assert_cmp_string_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_assert_cmp_string_test);

  assert (ui_test_assert_cmp_string (NULL, "") == -1);
  assert (ui_test_assert_cmp_string ("c", "d") == -1);

  assert (ui_test_assert_cmp_string (NULL, NULL) == 0);
  assert (ui_test_assert_cmp_string ("", "") == 0);
  assert (ui_test_assert_cmp_string ("a", "a") == 0);

  assert (ui_test_assert_cmp_string ("", NULL) == 1);
  assert (ui_test_assert_cmp_string ("b", "a") == 1);
}

static ui_test_case
ui_test_assert_eq_bool_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_assert_eq_bool_test);

  ui_test_assert_eq_bool (&test, 0, 0, 0, __FILE__, __LINE__);
  assert (test.status == UI_TEST_STATUS_FAILED);

  ui_test_assert_eq_bool (&test, 0, 0, 1, __FILE__, __LINE__);
  assert (test.status == UI_TEST_STATUS_PASSED);

  ui_test_assert_eq_bool (&test, 0, 1, 0, __FILE__, __LINE__);
  assert (test.status == UI_TEST_STATUS_PASSED);

  ui_test_assert_eq_bool (&test, 0, 1, 1, __FILE__, __LINE__);
  assert (test.status == UI_TEST_STATUS_FAILED);

  ui_test_assert_eq_bool (&test, 1, 0, 0, __FILE__, __LINE__);
  assert (test.status == UI_TEST_STATUS_PASSED);

  ui_test_assert_eq_bool (&test, 1, 0, 1, __FILE__, __LINE__);
  assert (test.status == UI_TEST_STATUS_FAILED);

  ui_test_assert_eq_bool (&test, 1, 1, 0, __FILE__, __LINE__);
  assert (test.status == UI_TEST_STATUS_FAILED);

  ui_test_assert_eq_bool (&test, 1, 1, 1, __FILE__, __LINE__);
  assert (test.status == UI_TEST_STATUS_PASSED);
}
