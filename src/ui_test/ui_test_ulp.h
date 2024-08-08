#pragma once

#include <stdint.h>

/**
 * @brief Compares two doubles.
 * @param l left-value
 * @param r right-value
 * @return @p 0 when equal, @p -1 when @code l < r@encode, @p 1 when @code l > r@endcode
 */
int ui_test_ulp_compare_double (float l, float r);

/**
 * @brief Compares two floats.
 * @param l left-value
 * @param r right-value
 * @return @p 0 when equal, @p -1 when @code l < r@encode, @p 1 when @code l > r@endcode
 */
int ui_test_ulp_compare_float (float l, float r);

/**
 * @brief Gets the units of least precision for a 32-bit floating point value.
 * @param value float.
 * @return ULP for the given float.
 */
int32_t ui_test_float_to_ulp (float value);

/**
 * @brief Gets the units of least precision for a 64-bit floating point value.
 * @param value double.
 * @return ULP for the given double.
 */
int64_t ui_test_double_to_ulp (int64_t value);
