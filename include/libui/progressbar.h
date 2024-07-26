#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiProgressBar
 * @param this pointer to cast
 */
#define uiProgressBar(this) ((uiProgressBar *)(this))

/**
 * @brief A @p uiControl that visualizes the progress of a task via the fill level of a horizontal bar
 * @remark Indeterminate values are supported via an animated bar.
 */
typedef struct uiProgressBar uiProgressBar;

/**
 * @brief Gets the value of a @p uiProgressBar
 * @param p @p uiProgressBar
 * @return value
 */
API int uiProgressBarValue (uiProgressBar *p);

/**
 * @brief Sets the value of a @p uiProgressBar
 * @param p @p uiProgressBar
 * @param n value between @p 0 and @p 100, or @p -1 to render an animated progress bar
 */
API void uiProgressBarSetValue (uiProgressBar *p, int n);

/**
 * @brief @p uiProgressBar constructor
 * @return @p uiProgressBar
 */
API uiProgressBar *uiNewProgressBar (void);
