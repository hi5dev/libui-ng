#pragma once

#include "api.h"

/**
 * Type-casts a pointer to a @p uiSpinbox
 * @param this pointer to cast
 */
#define uiSpinbox(this) ((uiSpinbox *)(this))

/**
 * @brief A @p uiControl to display and modify integer values via a text field or +/- buttons.
 *
 * - This is a convenient control for having the user enter integer values.
 * - Values are guaranteed to be within the specified range.
 * - The + button increases the held value by 1.
 * - The - button decreased the held value by 1.
 * - Entering a value out of range will clamp to the nearest value in range.
 */
typedef struct uiSpinbox uiSpinbox;

/**
 * @brief Gets the value of a @p uiSpinbox
 * @param s @p uiSpinbox
 * @return integer
 */
API int uiSpinboxValue (uiSpinbox *s);

/**
 * @brief Sets the value of a @p uiSpinbox
 * @param s @p uiSpinbox
 * @param value to set
 * @remark values are clamped to the range of the @p uiSpinbox
 */
API void uiSpinboxSetValue (uiSpinbox *s, int value);

/**
 * @brief Callback for @p uiSpinbox events
 * @param sender Back reference to the instance that triggered the callback.
 * @param senderData User data registered with the sender instance.
 */
typedef void (uiSpinboxCallback) (uiSpinbox *sender, void *senderData);

/**
 * @brief Registers a callback for when the value of a @p uiSpinbox is changed by the user
 * @param s @p uiSpinbox
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark The callback is not triggered when calling @p uiSpinboxSetValue
 * @remark Only one callback can be registered at a time.
 */
API void uiSpinboxOnChanged (uiSpinbox *s, void (*f) (uiSpinbox *sender, void *senderData), void *data);

/**
 * @brief @p uiSpinbox constructor
 * @remark The initial spinbox value equals the minimum value.
 * @remark In the current implementation @p min and @p max are swapped if @code min>max@endcode
 * @param min value
 * @param max value
 * @return @p uiSpinbox
 */
API uiSpinbox *uiNewSpinbox (int min, int max);
