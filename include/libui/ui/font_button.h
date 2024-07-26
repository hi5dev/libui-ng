#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiFontButton
 * @param this pointer to cast
 */
#define uiFontButton(this) ((uiFontButton *)(this))

/**
 * @brief A button-like @p uiControl that opens a font chooser when clicked
 */
typedef struct uiFontButton uiFontButton;

/**
 * @brief Callback for @p uiFontButton events
 * @param sender reference to the instance that triggered the callback
 * @param senderData user-data registered with the sender instance
 */
typedef void (uiFontButtonCallback) (uiFontButton *sender, void *senderData);

/**
 * @brief Gets the selected font
 * @param b @p uiFontButton
 * @param[out] desc font descriptor.
 * @see uiFreeFontButtonFont
 */
API void uiFontButtonFont (uiFontButton *b, uiFontDescriptor *desc);

/**
 * @brief Registers a callback for when the font is changed.
 * @param b @p uiFontButton
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark Only one callback can be registered at a time.
 */
API void uiFontButtonOnChanged (uiFontButton *b, uiFontButtonCallback *f, void *data);

/**
 * @brief @p uiFontButton constructor
 * @return @p uiFontButton
 * @remark the default font is determined by the OS defaults
 */
API uiFontButton *uiNewFontButton (void);

/**
 * @brief Frees a @p uiFontDescriptor constructed by @p uiNewFontButton
 * @param desc @p uiFontDescriptor
 * @remark After calling this function the contents of @p desc should be assumed undefined, however you can safely
 * reuse @p desc.
 * @remark @p Calling this function on a @p uiFontDescriptor not previously filled by @p uiFontButtonFont() iss
 * undefined behavior.
 */
API void uiFreeFontButtonFont (uiFontDescriptor *desc);
