#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiCombobox
 * @param this pointer to cast
 */
#define uiCombobox(this) ((uiCombobox *)(this))

/**
 * @brief A @p uiControl to select one item from a predefined list of items via a drop down menu.
 * @see uiEditableCombobox.
 */
typedef struct uiCombobox uiCombobox;

/**
 * @brief Callback function for @p uiCombobox events
 * @param sender reference to the instance that triggered the callback
 * @param senderData user-data registered with the sender instance
 */
typedef void (uiComboboxCallback) (uiCombobox *sender, void *senderData);

/**
 * @brief Appends an item to a @p uiCombobox
 * @param c @p uiCombobox
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiComboboxAppend (uiCombobox *c, const char *text);

/**
 * @brief Inserts an item to a @p uiCombox at a given index
 * @param c @p uiCombobox
 * @param index at which to insert the item
 * @param text string
 * @remark @p text is copied internally; wnership is not transferred
 */
API void uiComboboxInsertAt (uiCombobox *c, int index, const char *text);

/**
 * @brief Deletes an item from a @p uiCombobox at a given index
 * @remark Deleting a selected item moves the selection to the next item in the box, or to @p -1 when out of range.
 * @param c @p uiCombobox
 * @param index of the item to be deleted
 */
API void uiComboboxDelete (uiCombobox *c, int index);

/**
 * @brief Deletes all items from a @p uiCombobox
 * @param c @p uiCombobox
 */
API void uiComboboxClear (uiCombobox *c);

/**
 * @brief gets the number of items in a @p uiCombobox
 * @param c @p uiCombobox
 * @returns count
 */
API int uiComboboxNumItems (uiCombobox *c);

/**
 * @brief Gets the index of the currently selected item
 * @param c @p uiCombobox
 * @returns Index of the selected item or @p -1
 */
API int uiComboboxSelected (uiCombobox *c);

/**
 * @brief Sets the current selected item
 * @param c @p uiCombobox
 * @param index of the item or @p -1 to clear the selection
 */
API void uiComboboxSetSelected (uiCombobox *c, int index);

/**
 * @brief Registers a callback for when a @p uiCobobox item is selected
 * @param c @p uiCombobox
 * @param f pointer to the callback function
 * @param data User data to be passed to the callback.
 * @remark the callback is not triggered when any of these functions are called:
 *         - @p uiComboboxSetSelected
 *         - @p uiComboboxInsertAt
 *         - @p uiComboboxDelete
 *         - @p uiComboboxClear
 * @remark only one callback can be registered at a time
 */
API void uiComboboxOnSelected (uiCombobox *c, uiComboboxCallback *f, void *data);

/**
 * @brief @p uiCombobox constructor
 * @returns @p uiCombobox
 */
API uiCombobox *uiNewCombobox (void);
