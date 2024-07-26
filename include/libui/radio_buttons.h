#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiRadioButtons
 * @param this pointer to cast
 */
#define uiRadioButtons(this) ((uiRadioButtons *)(this))

/**
 * @brief A multiple choice @p uiControl of check buttons from which only one can be selected at a time
 */
typedef struct uiRadioButtons uiRadioButtons;

/**
 * @brief Callback function for @p uiRadioButtons events
 * @param sender Back reference to the instance that triggered the callback
 * @param senderData User data registered with the sender instance
 */
typedef void (uiRadioButtonsCallback) (uiRadioButtons *sender, void *senderData);

/**
 * @brief Appends an option to a @p uiRadioButtons
 * @param r @p uiRadioButtons
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiRadioButtonsAppend (uiRadioButtons *r, const char *text);

/**
 * @brief Gets the index of the item selected
 * @param r @p uiRadioButtons
 * @return index of the selected item, or @p -1 if nothing is selected
 */
API int uiRadioButtonsSelected (uiRadioButtons *r);

/**
 * @brief Selects the item at a given index
 * @param r @p uiRadioButtons
 * @param index of the item to be selected, or @p -1 to clear the selection
 */
API void uiRadioButtonsSetSelected (uiRadioButtons *r, int index);

/**
 * @p Registers a callback for when radio button is selected
 * @param r @p uiRadioButtons
 * @param f Callback function.\n
 * @param data User data to be passed to the callback.
 * @remark the callback is not triggered when calling @p uiRadioButtonsSetSelected
 * @remark only one callback can be registered at a time
 */
API void uiRadioButtonsOnSelected (uiRadioButtons *r, uiRadioButtonsCallback *f, void *data);

/**
 * @brief @p uiRadioButtons constructor
 * @return @p uiRadioButtons
 */
API uiRadioButtons *uiNewRadioButtons (void);
