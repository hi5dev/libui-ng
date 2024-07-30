#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiLabel
 * @param this pointer to cast
 */
#define uiLabel(this) ((uiLabel *)(this))

/**
 * @brief A control that displays non interactive text.
 */
typedef struct uiLabel uiLabel;

/**
 * @brief Gets the text of a @p uiLabel
 * @param l @p uiLabel
 * @return string
 * @remark Caller is responsible for freeing the returned text
 * @see uiFreeText
 */
API char *uiLabelText (uiLabel *l);

/**
 * @brief Sets the text of a @p uiLabel
 * @param l @p uiLabel
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiLabelSetText (uiLabel *l, const char *text);

/**
 * @brief @p uiLabel constructor
 * @param text of the label (string)
 * @remark @p text is copied internally; ownership is not transferred
 * @return @p uiLabel
 */
API uiLabel *uiNewLabel (const char *text);
