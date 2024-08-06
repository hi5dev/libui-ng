#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <tinyexpr.h>

/**
 * @brief Delegates to @p _assert_formula using the file and line of the caller.
 * @param formula to evaluate
 * @param expected value
 */
#define assert_formula(formula, expected) _assert_formula (formula, expected, __FILE__, __LINE__)

/**
 * @brief Tests the output of the given formula against an expected value.
 * @param formula to evaluate
 * @param expected value
 * @param file with the test
 * @param line number
 */
static void
_assert_formula (const char *formula, const double expected, const char *file, const int line)
{
  const double actual         = te_interp (formula, 0);
  const int    actual_class   = fpclassify (actual);
  const int    expected_class = fpclassify (expected);

  if (actual_class == expected_class)
    switch (expected_class)
      {
      case FP_INFINITE: // both values are infinite
      case FP_NAN:      // both values are not a number
      case FP_ZERO:     // both values are zero
        return;

      case FP_NORMAL:
      case FP_SUBNORMAL:
        // not an optimal floating point test, but it works for this simple test case
        if (fabs (expected - actual) <= DBL_EPSILON)
          return;
        break;

      default:
        (void)fprintf (stderr, "unhandled fpclass: %d\n", actual_class);
        abort ();
      }

  (void)fprintf (stderr, "%s:%d: %s\n  expected %f, got %f\n", file, line, formula, expected, actual);

  abort ();
}

int
main (void)
{
  assert_formula ("sqrt(5^2+7^2+11^2+(8-2)^2)", sqrt (pow (5, 2) + pow (7, 2) + pow (11, 2) + pow (8 - 2, 2)));

  assert_formula ("3 * 3 + 7 * 7", 3 * 3 + 7 * 7);

  // this program aborts on failure
  return EXIT_SUCCESS;
}
