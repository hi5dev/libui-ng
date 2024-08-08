#include "ui_test_ulp.h"
#include "ui_test.h"

#include <assert.h>
#include <float.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

int32_t
ui_test_float_to_ulp (const float value)
{
  int32_t ulps = 0;

  static const size_t size = sizeof (float);

  memcpy (&ulps, &value, sizeof (float));

  // positive values are the same in both two's complement and signed magnitude
  if (ulps > 0)
    return ulps;

  // for negative values, remove the sign bit and negate the result (subtract from 0)
  return -(ulps & ~(1 << size * 8 - 1));
}

int
ui_test_ulp_compare_float (const float l, const float r)
{
  // compare +0 to -0
  if (fpclassify (l) == FP_ZERO && fpclassify (r) == FP_ZERO)
    return 0;

  const int64_t ul = ui_test_float_to_ulp (l);
  const int64_t ur = ui_test_float_to_ulp (r);

  // when signs differ
  if (ul < 0 != ur < 0)
    return ul < 0 ? -1 : 1;

  // compare same signed infinities
  if (isinf (l) && isinf (r))
    return 0;

  // near-zero comparison
  if (fabs (l - r) <= FLT_EPSILON)
    return 0;

  return ul == ur ? 0 : ul < ur ? -1 : 1;
}

static ui_test_case
ui_test_ulp_compare_float_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_ulp_compare_float_test);

  assert (ui_test_ulp_compare_float (+0.000, +0.000) == 0);
  assert (ui_test_ulp_compare_float (-0.000, -0.000) == 0);
  assert (ui_test_ulp_compare_float (-0.000, +0.000) == 0);
  assert (ui_test_ulp_compare_float (+1.000, +1.000) == 0);
  assert (ui_test_ulp_compare_float (-1.000, -1.000) == 0);
  assert (ui_test_ulp_compare_float (+1.000, +2.000) == -1);
  assert (ui_test_ulp_compare_float (+3.000, +2.000) == 1);
  assert (ui_test_ulp_compare_float (+0.001, +0.002) == -1);
  assert (ui_test_ulp_compare_float (+0.003, +0.002) == 1);

  const float r1 = rand ();
  const float r2 = r1 * rand ();
  assert (ui_test_ulp_compare_float (r1, r2) != 0);
  assert (ui_test_ulp_compare_float (fabs (r1), -fabs (r2)) == 1);
  assert (ui_test_ulp_compare_float (-fabs (r1), fabs (r2)) == -1);
}

int64_t
ui_test_double_to_ulp (const int64_t value)
{
  int32_t ulps = 0;

  memcpy (&ulps, &value, sizeof (float));

  // positive values are the same in both two's complement and signed magnitude
  if (ulps > 0)
    return ulps;

  // for negative values, remove the sign bit and negate the result (subtract from 0)
  return -(ulps & ~((int64_t)1 << 31));
}

int
ui_test_ulp_compare_double (const float l, const float r)
{
  return ui_test_ulp_compare_float (l, r);
}

static ui_test_case
ui_test_ulp_compare_double_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_test_ulp_compare_double_test);

  assert (ui_test_ulp_compare_double (-0.000, +0.000) == 0);
  assert (ui_test_ulp_compare_double (+1.000, +1.000) == 0);
  assert (ui_test_ulp_compare_double (-1.000, -1.000) == 0);
  assert (ui_test_ulp_compare_double (+1.000, +2.000) == -1);
  assert (ui_test_ulp_compare_double (+3.000, +2.000) == 1);
  assert (ui_test_ulp_compare_double (+0.001, +0.002) == -1);
  assert (ui_test_ulp_compare_double (+0.003, +0.002) == 1);

  const double r1 = rand ();
  const double r2 = r1 * rand ();
  assert (ui_test_ulp_compare_double (r1, r2) != 0);
  assert (ui_test_ulp_compare_double (fabs (r1), -fabs (r2)) == 1);
  assert (ui_test_ulp_compare_double (-fabs (r1), fabs (r2)) == -1);
}
