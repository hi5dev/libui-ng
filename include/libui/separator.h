#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiSeparator
 * @param this pointer to cast
 */
#define uiSeparator(this) ((uiSeparator *)(this))

/**
 * @brief A @p uiControl to visually separate controls, horizontally or vertically
 */
typedef struct uiSeparator uiSeparator;

/**
 * @brief Constructs a horizontal @p uiSeparator
 * @return @p uiSeparator
 */
API uiSeparator *uiNewHorizontalSeparator (void);

/**
 * @brief Constructs a vertical @p uiSeparator
 * @return @p uiSeparator
 */
API uiSeparator *uiNewVerticalSeparator (void);
