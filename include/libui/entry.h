#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiEntry
 * @param this pointer to cast
 */
#define uiEntry(this) ((uiEntry *)(this))

/**
 * @brief A control with a single line text entry field.
 */
typedef struct uiEntry uiEntry;

/**
 * @brief Callback for @p uiEntry events
 * @param sender Back reference to the instance that initiated the callback.
 * @param senderData User data registered with the sender instance.
 */
typedef void (uiEntryCallback) (uiEntry *sender, void *senderData);

/**
 * @brief Gets the text of a @p uiEntry
 * @param e @p uiEntry
 * @returns string
 * @remark Caller is responsible for freeing the returned text
 * @see uiFreeText
 */
API char *uiEntryText (uiEntry *e);

/**
 * @brief Sets the text of a @p uiEntry
 * @param e @p uiEntry
 * @param text string
 * @remark @p text is copied internally; ownership is not transferred
 */
API void uiEntrySetText (uiEntry *e, const char *text);

/**
 * @brief Registers a callback for when the user changes the entry's text.
 * @param e @p uiEntry
 * @param f pointer to the callback function
 * @param data User data to be passed to the callback.
 * @remark The callback is not triggered when calling uiEntrySetText().
 */
API void uiEntryOnChanged (uiEntry *e, uiEntryCallback *f, void *data);

/**
 * @brief Checks if the text of a @p uiEntry can be modified.
 * @param e @p uiEntry
 * @returns non-zero when true
 */
API int uiEntryReadOnly (uiEntry *e);

/**
 * @brief Makes a @p uiEntry read-only, or writable.
 * @param e @p uiEntry
 * @param readonly non-zero to make read-only, zero to make writable.
 */
API void uiEntrySetReadOnly (uiEntry *e, int readonly);

/**
 * @brief @p uiEntry constructor
 * @return @p uiEntry
 */
API uiEntry *uiNewEntry (void);

/**
 * @brief Constructs a @p uiEntry with text masked by asterisks, suitable for entering a password
 * @return @p uiEntry
 */
API uiEntry *uiNewPasswordEntry (void);

/**
 * @brief Constructs a @p uiEntry suitable for search fields
 * @return @p uiEntry
 * @remark Some systems will deliberately delay the @p uiEntryOnChanged callback for a more natural feel
 */
API uiEntry *uiNewSearchEntry (void);
