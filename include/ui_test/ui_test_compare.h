#pragma once

#pragma region macros

#define ui_test_compare_function(value)                                                                               \
  _Generic ((value),                                                                                                  \
      char *: ui_test_compare_string,                                                                                 \
      const char *: ui_test_compare_string,                                                                           \
      double: ui_test_compare_double,                                                                                 \
      double *: ui_tset_compare_double_p,                                                                             \
      float: ui_test_compare_float,                                                                                   \
      int: ui_test_compare_int)

#define ui_test_compare_message(value)                                                                                \
  _Generic ((value),                                                                                                  \
      char *: ui_test_compare_message_string,                                                                         \
      const char *: ui_test_compare_message_string,                                                                   \
      double: ui_test_compare_message_double,                                                                         \
      double *: ui_test_compare_message_double_p,                                                                     \
      float: ui_test_compare_message_float,                                                                           \
      int: ui_test_compare_message_int)

#pragma endregion

#pragma region types

/**
 * @brief Comparison data.
 */
struct ui_test_compare_t;

/**
 * @brief Data-comparison function type.
 * @details The sign of the result is the sign of the difference between the two values, lexicographically for
 * strings, numerically for numbers.
 * @param compare data to compare.
 * @return @p 0 when @p expected and @p actual are equal
 * @return @p -1 when @p actual is less than @p expected
 * @return @p 1 when @p actual is greater than @p expected
 */
typedef int (*ui_test_compare_f) (struct ui_test_compare_t *compare);

/**
 * @brief Creates a message showing what the expected and actual values are.
 * @param compare data.
 * @return description of the actual and expected values.
 */
typedef char *(*ui_test_compare_message_f) (struct ui_test_compare_t *compare);

#pragma endregion

#pragma region structure

struct ui_test_compare_t
{
  /// @brief Actual value being tested.
  const void *actual;

  /// @brief Expected value.
  const void *expected;

  /// @brief Expects @p actual to not equal @p expected when non-zero.
  const int invert;

  /// @brief Comparison function.
  ui_test_compare_f compare;

  /// @brief Comparison message function.
  ui_test_compare_message_f message;
};

struct ui_test_compare_t ui_test_compare_init (const void *actual, const void *expected, int invert,
                                               ui_test_compare_f compare, ui_test_compare_message_f message);

#pragma endregion

#pragma region compare functions

int ui_test_compare_double (struct ui_test_compare_t *compare);
int ui_test_compare_double_p (struct ui_test_compare_t *compare);
int ui_test_compare_float (struct ui_test_compare_t *compare);
int ui_test_compare_int (struct ui_test_compare_t *compare);
int ui_test_compare_null (struct ui_test_compare_t *compare);
int ui_test_compare_string (struct ui_test_compare_t *compare);

#pragma endregion

#pragma region compare message functions

char *ui_test_compare_message_double (struct ui_test_compare_t *c);
char *ui_test_compare_message_double_p (struct ui_test_compare_t *c);
char *ui_test_compare_message_float (struct ui_test_compare_t *c);
char *ui_test_compare_message_int (struct ui_test_compare_t *c);
char *ui_test_compare_message_null (struct ui_test_compare_t *c);
char *ui_test_compare_message_string (struct ui_test_compare_t *c);

#pragma endregion
