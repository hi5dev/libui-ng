#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiCheckbox
 * @param this pointer to cast
 */
#define uiCheckbox(this) ((uiCheckbox *)(this))

/**
 * @brief A control with a user checkable box accompanied by a text label.
 */
typedef struct uiCheckbox uiCheckbox;

/**
 * @brief Callback for @p uiCheckbox events
 * @param sender Back reference to the instance that initiated the callback.
 * @param senderData User data registered with the sender instance.
 */
typedef void (uiCheckboxCallback) (uiCheckbox *sender, void *senderData);

/**
 * @brief Gets the a @p uiCheckbox label's text
 * @param c @p uiCheckbox
 * @returns string
 * @remark Caller is responsible for freeing the data
 * @see uiFreeText
 */
API char *uiCheckboxText (uiCheckbox *c);

/**
 * @brief Sets a @p uiCheckbox label's text
 * @param c @p uiCheckbox
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiCheckboxSetText (uiCheckbox *c, const char *text);

/**
 * @brief Registers a callback for when the checkbox is toggled by the user.
 * @param c @p uiCheckbox
 * @param f Pointer to the callback function
 * @param data User data to be passed to the callback.
 * @remark The callback is not triggered when calling @p uiCheckboxSetChecked
 * @remark Only one callback can be registered at a time.
 */
API void uiCheckboxOnToggled (uiCheckbox *c, uiCheckboxCallback *f, void *data);

/**
 * @brief Checks if a @p uiCheckbox is checked
 * @param c @p uiCheckbox
 * @return non-zero when checked
 */
API int uiCheckboxChecked (uiCheckbox *c);

/**
 * @brief Checks or unchecks a @p uiCheckbox
 * @param c @p uiCheckbox
 * @param checked non-zero to check, zero to uncheck
 */
API void uiCheckboxSetChecked (uiCheckbox *c, int checked);

/**
 * @brief @p uiCheckbox constructor
 * @param text Label text (string)
 * @remark @p text is copied internally; ownership is not transferred
 * @return @p uiCheckbox
 */
API uiCheckbox *uiNewCheckbox (const char *text);
