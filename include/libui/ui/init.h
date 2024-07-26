#pragma once

#include "api.h"

/**
 * @brief LibUI initialization options.
 */
typedef struct uiInitOptions
{
  size_t Size; //!< struct size
} uiInitOptions;

/**
 * @brief Initializes the LibUI library.
 * @param options @p uiInitOptions
 * @return @p NULL when successful, an error message on failure
 */
API const char *uiInit (uiInitOptions *options);

/**
 * @brief Frees all resources used by the LibUI library.
 */
API void uiUninit (void);

/**
 * @brief Frees errors returned by @p uiInit
 * @param err string
 */
API void uiFreeInitError (const char *err);
