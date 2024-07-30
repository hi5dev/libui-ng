#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiEditableCombobox
 * @param this pointer to cast
 */
#define uiEditableCombobox(this) ((uiEditableCombobox *)(this))

/**
 * @brief A @p uiControl to select one item from a predefined list of items or enter ones own.
 *
 * - Predefined items can be selected from a drop down menu.
 * - A customary item can be entered by the user via an editable text field.
 *
 * @see uiCombobox
 */
typedef struct uiEditableCombobox uiEditableCombobox;

/**
 * @brief Callback function for @p uiEditableCombobox events
 * @param sender reference to the instance that triggered the callback
 * @param senderData user data registered with the sender instance
 */
typedef void (uiEditableComboboxCallback) (uiEditableCombobox *sender, void *senderData);

/**
 * @brief Appends an item to an @p uiEditableCombobox
 * @param c @p uiEditableCombobox
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiEditableComboboxAppend (uiEditableCombobox *c, const char *text);

/**
 * @brief Gets the text of an @p uiEditableCombobox
 * @details This text is either the text of one of the predefined list items or the text manually entered by the user.
 * @param c @p uiEditableCombobox
 * @return string
 * @remark the caller is responsible for freeing the returned string
 * @see uiFreeText
 */
API char *uiEditableComboboxText (uiEditableCombobox *c);

/**
 * @brief Sets the text of an @p uiEditableCombobox
 * @param c @p uiEditableCombobox
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiEditableComboboxSetText (uiEditableCombobox *c, const char *text);

/**
 * @brief Registers a callback for when an @p uiEditableCombobox item is selected, or the user modified its text
 * @param c @p uiEditableCombobox
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark The callback is not triggered when calling @p uiEditableComboboxSetText
 * @remark Only one callback can be registered at a time
 */
API void uiEditableComboboxOnChanged (uiEditableCombobox *c, uiEditableComboboxCallback *f, void *data);

/**
 * @brief @p uiEditableCombobox constructor
 * @returns @p uiEditableCombobox
 */
API uiEditableCombobox *uiNewEditableCombobox (void);
