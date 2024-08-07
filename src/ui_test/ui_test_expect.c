#include <ui_test_expect.h>

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef ui_test_pass
#undef ui_test_fail

#define ui_float_bias_f(v) ((struct ui_float_bias_t){ .value = { .f = v }, .epsilon = { .f = FLT_EPSILON } })
#define ui_float_bias_d(v) ((struct ui_float_bias_t){ .value = { .d = v }, .epsilon = { .d = DBL_EPSILON } })

#define ui_test_cmp_flt_fmt "expected %f %s equal %f"

#define ui_test_cmp_str_fmt                                                                                           \
  "expected strings %s equal:\n"                                                                                      \
  "  expected value: %s\n"                                                                                            \
  "    actual value: %s"

/**
 * @brief Used to get the adjacent integer representation of a floating point value.
 *
 * The result of subtracting the absolute value of integer representations of two same-sign floats is equal to
 * the number of representable floats between them plus one. This is useful for accurately comparing two
 * floating-point numbers.
 */
struct ui_float_bias_t
{
  union
  {
    uint32_t i;
    float    f;
    double   d;
  } value;

  union
  {
    float  f;
    double d;
  } epsilon;
};

static int
ui_test_pass (struct ui_test_t *test, const char *file, const int line)
{
  ui_test_set_status (test, UI_TEST_STATUS_PASSED, 0, file, line);

  return 1;
}

static int
ui_test_fail (struct ui_test_t *test, const char *message, const char *file, const int line)
{
  ui_test_set_status (test, UI_TEST_STATUS_FAILED, strdup (message), file, line);

  return 0;
}

static int
ui_test_cmp_float_bias (struct ui_test_t *test, const int invert, const struct ui_float_bias_t l,
                        const struct ui_float_bias_t r, const char *file, const int line)
{
  const int same_sign = l.value.i < 0 == r.value.i < 0;

  // when the signs are different, test for +0 == -0
  if ((!same_sign && l.value.i == r.value.i) == (invert == 0))
    return ui_test_pass (test, file, line);

  // compare difference
  if (fabs (l.value.i) - fabs (r.value.i) <= l.epsilon.d == (invert == 0))
    return ui_test_pass (test, file, line);

  const size_t message_size = snprintf (NULL, 0, ui_test_cmp_flt_fmt, l.value.f, invert ? "not to" : "to", r.value.f);
  char        *message      = malloc (message_size + 1);
  assert (message != NULL);

  const size_t bytes_written = sprintf (message, ui_test_cmp_flt_fmt, l.value.f, invert ? "not to" : "to", r.value.f);
  assert (bytes_written == message_size);

  (void)ui_test_fail (test, message, file, line);
  free (message);

  return 0;
}

int
ui_test_cmp_double (struct ui_test_t *test, const int invert, const double l, const double r, const char *file,
                    const int line)
{
  return ui_test_cmp_float_bias (test, invert, ui_float_bias_d (l), ui_float_bias_d (r), file, line);
}

static ui_test_case
ui_test_cmp_double_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_cmp_double_test);

  ui_expect_cmp (double, 0.0, is, 0.0);
  ui_expect_cmp (double, +0.0, is, -0.0);
  ui_expect_cmp (double, 1.0, is, 1.0);
  ui_expect_cmp (double, 1.1, is, 1.1);
  ui_expect_cmp (double, 1.12345678910, is, 1.12345678910);

  ui_expect_cmp (double, 1.1, is_not, 1.2);
  ui_expect_cmp (double, 1.1234567891011, is_not, 1.12345678910);
}

int
ui_test_cmp_float (struct ui_test_t *test, const int invert, const float l, const float r, const char *file,
                   const int line)
{
  return ui_test_cmp_float_bias (test, invert, ui_float_bias_f (l), ui_float_bias_f (r), file, line);
}

static ui_test_case
ui_test_cmp_float_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_cmp_float_test);

  ui_expect_cmp (float, 0.0f, is, 0.0f);
  ui_expect_cmp (float, +0.0f, is, -0.0f);
  ui_expect_cmp (float, 1.0f, is, 1.0f);
  ui_expect_cmp (float, 1.1f, is, 1.1f);
  ui_expect_cmp (float, 1.0000000001f, is, 1.0000000001f);

  ui_expect_cmp (float, 1.1f, is_not, 1.2f);
  ui_expect_cmp (float, 1.0000000001f, is_not, 1.0000000002f);
}

int
ui_test_cmp_int (struct ui_test_t *test, const int invert, const int l, const int r, const char *file, const int line)
{
  const int diff = l > r ? l - r : r - l;

  if (diff == 0 == (invert == 0))
    return ui_test_pass (test, file, line);

  char message[BUFSIZ];

  const int size = sprintf (message, "expected %d %s equal %d", l, invert ? "not to" : "to", r);

  message[(size + 1) / sizeof (char)] = '\0';

  return ui_test_fail (test, message, file, line);
}

static ui_test_case
ui_test_cmp_int_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_cmp_int_test);

  ui_expect_cmp (int, 1, is, 1);
  ui_expect_cmp (int, 1, is_not, 0);
}

int
ui_test_cmp_str (struct ui_test_t *test, const int invert, const char *l, const char *r, const char *file,
                 const int line)
{
  if (l == NULL || r == NULL)
    {
      if (l == NULL == (r == NULL) == (invert == 0))
        return ui_test_pass (test, file, line);
    }
  else
    {
      if (strcmp (l, r) == 0 == (invert == 0))
        return ui_test_pass (test, file, line);
    }

  const size_t message_size = snprintf (NULL, 0, ui_test_cmp_str_fmt, invert ? "not to" : "to", r, l);
  char        *message      = malloc (message_size + 1);
  assert (message != NULL);

  const size_t bytes_written = sprintf (message, ui_test_cmp_str_fmt, invert ? "not to" : "to", r, l);
  assert (bytes_written == message_size);

  (void)ui_test_fail (test, message, file, line);
  free (message);

  return 0;
}

static ui_test_case
ui_test_cmp_str_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_cmp_str_test);

  ui_expect (ui_test_cmp_str (&test, 0, "hello", "hello", __FILE__, __LINE__) == 1, "ui_test_cmp_str failed");
  ui_expect (ui_test_cmp_str (&test, 0, "hello", "world", __FILE__, __LINE__) == 0, "ui_test_cmp_str failed");

  ui_test_cmp_str (&test, 0, "hello", "world", __FILE__, __LINE__);
  ui_expect (test.status == UI_TEST_STATUS_FAILED, "expected ui_test_cmp_str to update test to failed status");
  ui_expect (test.backtrace.message == NULL, "expected ui_test_cmp_str to set test.backtrace.message");

  ui_test_cmp_str (&test, 1, "hello", "world", __FILE__, __LINE__);
  ui_expect (test.status == UI_TEST_STATUS_PASSED, "expected ui_test_cmp_str to update test to passed status");
  ui_expect (test.backtrace.message == NULL, "expected ui_test_cmp_str to set test.backtrace.message");

  ui_expect_cmp (str, "hello", is, "hello");
  ui_expect_cmp (str, "hello world", is_not, "hello earth");

  ui_expect_cmp (str, NULL, is_not, "");
  ui_expect_cmp (str, "", is_not, NULL);
}

int
ui_test_expect_null (struct ui_test_t *test, const int invert, const char *ptr_name, void *actual, const char *file,
                     const int line)
{
  if (actual == NULL == (invert == 0))
    return ui_test_pass (test, file, line);

  char message[BUFSIZ];

  const int size = sprintf (message, "expected %s %s be NULL", ptr_name, invert ? "not to" : "to");

  message[(size + 1) / sizeof (char)] = '\0';

  return ui_test_fail (test, message, file, line);
}

static ui_test_case
ui_test_expect_null_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_expect_null_test);

  char *is_null     = NULL;
  char *is_not_null = "";

  (void)ui_test_expect_null (&test, 0, "is_not_null", is_not_null, __FILE__, __LINE__);
  ui_expect_cmp (str, test.backtrace.message, is, "expected is_not_null to be NULL");

  (void)ui_test_expect_null (&test, 1, "is_null", is_null, __FILE__, __LINE__);
  ui_expect_cmp (str, test.backtrace.message, is, "expected is_null not to be NULL");

  ui_expect_cmp (int, ui_test_expect_null (&test, 0, "is_null", is_null, __FILE__, __LINE__), is, 1);
  ui_expect_cmp (int, ui_test_expect_null (&test, 0, "is_not_null", is_not_null, __FILE__, __LINE__), is, 0);
  ui_expect_cmp (int, ui_test_expect_null (&test, 1, "is_null", is_null, __FILE__, __LINE__), is, 0);
  ui_expect_cmp (int, ui_test_expect_null (&test, 1, "is_not_null", is_not_null, __FILE__, __LINE__), is, 1);
}

int
ui_test_expect_bool (struct ui_test_t *test, const int invert, const int actual, const char *file, const int line)
{
  if (actual == 1 == (invert == 0))
    return ui_test_pass (test, file, line);

  char message[BUFSIZ];

  const int size = sprintf (message, "expected %d to be %s", actual, invert ? "zero" : "non-zero");

  message[(size + 1) / sizeof (char)] = '\0';

  return ui_test_fail (test, message, file, line);
}

static ui_test_case
ui_test_expect_bool_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_expect_bool_test);

  const int t00 = ui_test_expect_bool (&test, 0, 0, __FILE__, __LINE__);
  ui_expect_cmp (str, test.backtrace.message, is, "expected 0 to be non-zero");
  ui_expect_cmp (int, t00, is, 0);

  const int t11 = ui_test_expect_bool (&test, 1, 1, __FILE__, __LINE__);
  ui_expect_cmp (str, test.backtrace.message, is, "expected 1 to be zero");
  ui_expect_cmp (int, t11, is, 0);

  const int t10 = ui_test_expect_bool (&test, 1, 0, __FILE__, __LINE__);
  ui_expect_null (test.backtrace.message);
  ui_expect_cmp (int, t10, is, 1);

  const int t01 = ui_test_expect_bool (&test, 0, 1, __FILE__, __LINE__);
  ui_expect_null (test.backtrace.message);
  ui_expect_cmp (int, t01, is, 1);
}
