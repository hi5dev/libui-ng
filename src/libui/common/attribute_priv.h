#pragma once

#include "attribute.h"

/**
 * @param a @p uiAttribute
 * @return @p uiAttribute
 */
API uiAttribute *uiprivAttributeRetain (uiAttribute *a);

/**
 * @param a @p uiAttribute
 */
API void uiprivAttributeRelease (uiAttribute *a);

/**
 * @brief Compares two @p uiAttribute instances
 * @param a @p uiAttribute
 * @param b @p uiAttribute
 * @return zero when equal
 */
API int uiprivAttributeEqual (const uiAttribute *a, const uiAttribute *b);
