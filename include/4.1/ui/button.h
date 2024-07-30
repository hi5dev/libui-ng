#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiButton
 * @param this pointer to cast
 */
#define uiButton(this) ((uiButton *)(this))

/**
 * @brief A control that visually represents a button to be clicked by the user to trigger an action.
 */
typedef struct uiButton uiButton;

/**
 * @brief Callback for @p uiBUtton events
 * @param sender Back reference to the instance that triggered the callback.
 * @param senderData User data registered with the sender instance.
 */
typedef void (uiButtonCallback) (uiButton *sender, void *senderData);

/**
 * @brief Gets a @p uiButton label's text
 * @param b @p uiButton
 * @returns string
 * @remark Caller is responsible for freeing the data.
 * @see @p uiFreeText
 */
API char *uiButtonText (uiButton *b);

/**
 * @brief Sets a @p uiButton label's text
 * @param b @p uiButton
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiButtonSetText (uiButton *b, const char *text);

/**
 * @brief Registers a callback for when a @p uiButton is clicked.
 * @param b @p uiButton
 * @param f pointer to the callback function
 * @param data User data to be passed to the callback.
 * @remark Only one callback can be registered at a time.
 */
API void uiButtonOnClicked (uiButton *b, uiButtonCallback *f, void *data);

/**
 * @brief @p uiButton constructor
 * @param text of the button's label
 * @remark @p text is copied internally; ownership is not transferred
 * @returns @p uiButton
 */
API uiButton *uiNewButton (const char *text);
