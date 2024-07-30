#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiColorButton
 * @param this pointer to cast
 */
#define uiColorButton(this) ((uiColorButton *)(this))

/**
 * @brief A @p uiControl with a color indicator that opens a color chooser when clicked.
 *
 * - The control visually represents a button with a color field representing the selected color.
 * - Clicking on the button opens up a color chooser in form of a color palette.
 */
typedef struct uiColorButton uiColorButton;

/**
 * @brief Callback for @p uiColorButton events
 * @param sender reference to the instance that triggered the callback
 * @param senderData user-defined data to be passed to the callback
 */
typedef void (uiColorButtonCallback) (uiColorButton *sender, void *senderData);

/**
 * @brief Gets the color button color.
 * @param b @p uiColorButton
 * @param[out] r Red. Double in range of [0, 1.0].
 * @param[out] g Green. Double in range of [0, 1.0].
 * @param[out] bl Blue. Double in range of [0, 1.0].
 * @param[out] a Alpha. Double in range of [0, 1.0].
 */
API void uiColorButtonColor (uiColorButton *b, double *r, double *g, double *bl, double *a);

/**
 * @brief Sets the color button color.
 * @param b @p uiColorButton
 * @param r Red. Double in range of [0, 1.0].
 * @param g Green. Double in range of [0, 1.0].
 * @param bl Blue. Double in range of [0, 1.0].
 * @param a Alpha. Double in range of [0, 1.0].
 */
API void uiColorButtonSetColor (uiColorButton *b, double r, double g, double bl, double a);

/**
 * @brief Registers a callback for when the color is changed.
 * @param b @p uiColorButton
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark the callback is not triggered when calling @p uiColorButtonSetColor
 * @remark only one callback can be registered at a time
 */
API void uiColorButtonOnChanged (uiColorButton *b, uiColorButtonCallback *f, void *data);

/**
 * @brief @p uiColorButton constructor
 * @return @p uiColorButton
 */
API uiColorButton *uiNewColorButton (void);
