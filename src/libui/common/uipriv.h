#pragma once

#include "api.h"

#define uiprivNew(T) ((T *)uiprivAlloc (sizeof (T), #T))

/**
 * @param size
 * @param type
 */
API void *uiprivAlloc (size_t size, const char *type);

/**
 * @brief
 * @param _p
 * @param size
 * @param type
 */
API void *uiprivRealloc (void *_p, size_t size, const char *type);

/**
 * @brief
 * @param _p
 */
API void uiprivFree (void *_p);

/**
 * @brief
 */
API int uiprivShouldQuit (void);

/**
 * @brief
 * @param m
 * @param x
 * @param y
 * @param xamount
 * @param yamount
 */
API void uiprivFallbackSkew (uiDrawMatrix *m, double x, double y, double xamount, double yamount);

/**
 * @brief
 * @param m
 * @param x
 * @param y
 */
API void uiprivFallbackTransformSize (const uiDrawMatrix *m, double *x, double *y);

/**
 * @brief
 * @param a
 * @param b
 */
API int uiprivStricmp (const char *a, const char *b);
