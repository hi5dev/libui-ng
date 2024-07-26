#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiMultilineEntry
 * @param this pointer to cast
 */
#define uiMultilineEntry(this) ((uiMultilineEntry *)(this))

/**
 * @brief A @p uiControl with a multi line text entry field
 */
typedef struct uiMultilineEntry uiMultilineEntry;

/**
 * @brief Callback function for @p uiMultilineEntry events
 * @param sender back reference to the instance that initiated the callback
 * @param senderData user data registered with the sender instance
 */
typedef void (uiMultilineEntryCallback) (uiMultilineEntry *sender, void *senderData);

/**
 * @brief Gets the text of a @p uiMultilineEntry
 * @param e @p uiMultilineEntry
 * @return string
 * @remark caller is responsible for freeing the returned text
 * @see uiFreeText
 */
API char *uiMultilineEntryText (uiMultilineEntry *e);

/**
 * @brief Sets the text of a @p uiMultilineEntry
 * @param e @p uiMultilineEntry
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiMultilineEntrySetText (uiMultilineEntry *e, const char *text);

/**
 * @brief Appends text to a @p uiMultilineEntry
 * @param e @p uiMultilineEntry
 * @param text string
 * @remark @param text is copied internally; ownership is not transferred
 */
API void uiMultilineEntryAppend (uiMultilineEntry *e, const char *text);

/**
 * @brief Registers a callback for when the user modifies the content of a @p uiMultilineEntry
 * @param e @p uiMultilineEntry
 * @param f pointer to the callback function
 *          @p
 *          @p
 * @param data User data to be passed to the callback.
 *
 * @note The callback is not triggered when calling uiMultilineEntrySetText()
 *       or uiMultilineEntryAppend().
 * @note Only one callback can be registered at a time.
 * @memberof uiMultilineEntry
 */
API void uiMultilineEntryOnChanged (uiMultilineEntry *e, uiMultilineEntryCallback *f, void *data);

/**
 * @brief Checks if the text of a @p uiMultilineEntry can be modified
 * @param e @p uiMultilineEntry
 * @return non-zero when true
 */
API int uiMultilineEntryReadOnly (uiMultilineEntry *e);

/**
 * @brief Sets the writability of a @p uiMultilineEntry
 * @param e @p uiMultilineEntry
 * @param readonly non-zero to set to read-only, zero to set to writable
 */
API void uiMultilineEntrySetReadOnly (uiMultilineEntry *e, int readonly);

/**
 * @brief @uiMultilineEntry constructor
 * @return @p uiMultilineEntry
 */
API uiMultilineEntry *uiNewMultilineEntry (void);

/**
 * @brief Constructs a @uiMultilineEntry that automatically scrolls horizontally as text overflows
 * @returns @p uiMultilineEntry
 * @remark Windows does not allow for this style to be changed after creation
 */
API uiMultilineEntry *uiNewNonWrappingMultilineEntry (void);
