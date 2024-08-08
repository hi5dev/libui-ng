#pragma once

#define ui_assert_cmp(actual, expected)                                                                               \
  _Generic ((expected),                                                                                               \
      char *: ui_test_assert_cmp_string,                                                                              \
      double: ui_test_assert_cmp_double,                                                                              \
      float: ui_test_assert_cmp_float,                                                                                \
      int: ui_test_assert_cmp_int) (actual, expected)

#define ui_assert_eq(actual, expected)                                                                                \
  _Generic ((expected),                                                                                               \
      char *: ui_test_assert_eq_string,                                                                               \
      double: ui_test_assert_eq_double,                                                                               \
      float: ui_test_assert_eq_float,                                                                                 \
      int: ui_test_assert_eq_int) (&test, 1, actual, expected, __FILE__, __LINE__)

#define ui_assert_not_eq(actual, expected)                                                                            \
  _Generic ((expected),                                                                                               \
      char *: ui_test_assert_eq_string,                                                                               \
      double: ui_test_assert_eq_double,                                                                               \
      float: ui_test_assert_eq_float,                                                                                 \
      int: ui_test_assert_eq_int) (&test, 0, actual, expected, __FILE__, __LINE__)

#define ui_assert_true(b)  ui_assert_eq (&test, b, 1, __FILE__, __LINE__)
#define ui_assert_false(b) ui_assert_not_eq (&test, b, 1, __FILE__, __LINE__)

#define ui_assert_null(ptr)     ui_test_assert_eq_null (&test, (ptr), 1, __FILE__, __LINE__)
#define ui_assert_not_null(ptr) ui_test_assert_eq_null (&test, (ptr), 0, __FILE__, __LINE__)

typedef int (ui_test_assert_cmp_func) (const void *, const void *);

int ui_test_assert_cmp_bool (int l, int r);

int ui_test_assert_cmp_double (double l, double r);

int ui_test_assert_cmp_float (float l, float r);

int ui_test_assert_cmp_int (int l, int r);

int ui_test_assert_cmp_string (char *l, char *r);

void ui_test_assert_eq_bool (struct ui_test_t *test, int eq, int actual, int expected, char *file, int line);

void ui_test_assert_eq_double (struct ui_test_t *test, int eq, double actual, double expected, char *file, int line);

void ui_test_assert_eq_float (struct ui_test_t *test, int eq, float actual, float expected, char *file, int line);

void ui_test_assert_eq_int (struct ui_test_t *test, int eq, int actual, int expected, char *file, int line);

void ui_test_assert_eq_null (struct ui_test_t *test, int eq, void *ptr, char *file, int line);

void ui_test_assert_eq_string (struct ui_test_t *test, int eq, char *actual, char *expected, char *file, int line);
