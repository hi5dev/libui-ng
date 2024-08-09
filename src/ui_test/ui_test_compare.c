#include <ui_test_compare.h>

#include "ui_test_ulp.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma region structure

struct ui_test_compare_t
ui_test_compare_init (const void *actual, const void *expected, const int invert, const ui_test_compare_f compare,
                      const ui_test_compare_message_f message)
{
  return (struct ui_test_compare_t){ actual, expected, invert, compare, message };
}

#pragma endregion

#pragma region compare functions

int
ui_test_compare_double (struct ui_test_compare_t *compare)
{
  assert (compare != NULL);

  assert (compare->actual != NULL);
  const double actual = *(const double *)compare->actual;

  assert (compare->expected != NULL);
  const double expected = *(const double *)compare->expected;

  return ui_test_ulp_compare_double (actual, expected);
}

int
ui_test_compare_float (struct ui_test_compare_t *compare)
{
  assert (compare != NULL);

  assert (compare->actual != NULL);
  const float actual = *(const float *)compare->actual;

  assert (compare->expected != NULL);
  const float expected = *(const float *)compare->expected;

  return ui_test_ulp_compare_float (actual, expected);
}

int
ui_test_compare_int (struct ui_test_compare_t *compare)
{
  assert (compare != NULL);

  assert (compare->actual != NULL);
  const int actual = *(const int *)compare->actual;

  assert (compare->expected != NULL);
  const int expected = *(const int *)compare->expected;

  return actual == expected ? 0 : actual < expected ? -1 : 1;
}

int
ui_test_compare_null (struct ui_test_compare_t *compare)
{
  assert (compare != NULL);
  assert (compare->expected == NULL);

  return compare->actual == NULL ? 0 : 1;
}

int
ui_test_compare_string (struct ui_test_compare_t *compare)
{
  assert (compare != NULL);

  assert (compare->actual != NULL);
  const char *actual = *(const char **)compare->actual;

  assert (compare->expected != NULL);
  const char *expected = *(const char **)compare->expected;

  return strcmp (actual, expected);
}

#pragma endregion

#pragma region compare message functions

/**
 * @brief Writes a formatted message to a string buffer.
 * @details Calculates the size required to format the message, allocates memory for it, and populates the returned
 * buffer with the formatted message. Also checks for out of memory errors, but only with @p assert since this is
 * part of the test framework.
 * @param fmt formatted message.
 * @param ... data for the formatted message.
 * @return the message.
 * @remark The returned data needs to be freed by the caller.
 */
__attribute__ ((format (printf, 1, 2))) static char *
ui_test_compare_printf (const char *fmt, ...)
{
  assert (fmt != NULL);

  va_list args;

  va_start (args, fmt);
  const size_t message_size = vsnprintf (NULL, 0, fmt, args);
  va_end (args);

  va_start (args, fmt);
  char *buffer = malloc (message_size + 1);
  assert (buffer != NULL);

  vsnprintf (buffer, message_size + 1, fmt, args);
  va_end (args);

  return buffer;
}

char *
ui_test_compare_message_double (struct ui_test_compare_t *c)
{
  assert (c->actual != NULL);
  const double actual = *(const double *)c->actual;

  assert (c->expected != NULL);
  const double expected = *(const double *)c->expected;

  return ui_test_compare_printf ("expected %f to %s %f", actual, c->invert ? "not equal" : "equal", expected);
}

char *
ui_test_compare_message_float (struct ui_test_compare_t *c)
{
  assert (c->actual != NULL);
  const float actual = *(const float *)c->actual;

  assert (c->expected != NULL);
  const float expected = *(const float *)c->expected;

  return ui_test_compare_printf ("expected %f to %s %f", actual, c->invert ? "not equal" : "equal", expected);
}

char *
ui_test_compare_message_int (struct ui_test_compare_t *c)
{
  assert (c->actual != NULL);
  const int actual = *(const int *)c->actual;

  assert (c->expected != NULL);
  const int expected = *(const int *)c->expected;

  return ui_test_compare_printf ("expected %d to %s %d", actual, c->invert ? "not equal" : "equal", expected);
}

char *
ui_test_compare_message_null (struct ui_test_compare_t *c)
{
  assert (c != NULL);
  return ui_test_compare_printf ("expected %s", c->invert ? "not null" : "null");
}

char *
ui_test_compare_message_string (struct ui_test_compare_t *c)
{
  assert (c != NULL);
  const char *const equality = c->invert ? "match" : "not match";

  assert (c->actual != NULL);
  const char *actual = *(const char **)c->actual;

  assert (c->expected != NULL);
  const char *expected = *(const char **)c->expected;

  return ui_test_compare_printf ("strings should %s:\nexpected: %s\n  actual: %s", equality, expected, actual);
}

#pragma endregion
