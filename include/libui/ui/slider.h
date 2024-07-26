#pragma once

#include "api.h"

#include <slider.h>

/**
 * Type-casts a pointer to a @p uiSlider
 * @param this pointer to cast
 */
#define uiSlider(this) ((uiSlider *)(this))

/**
 * @brief A control to display and modify integer values via a user draggable slider.
 *
 * - Values are guaranteed to be within the specified range.
 * - By default, a @p uiSlider has a tool tip showing the current value when being dragged.
 */
typedef struct uiSlider uiSlider;

/**
 * @brief Callback for @p uiSlider events
 * @param sender Back reference to the instance that triggered the callback.
 * @param senderData User data registered with the sender instance.
 */
typedef void (uiSliderCallback) (uiSlider *sender, void *senderData);

/**
 * @brief Gets the value of a @p uiSlider
 * @param s @p uiSlider
 * @return value
 */
API int uiSliderValue (uiSlider *s);

/**
 * @brief Sets the value of a @p uiSlider
 * @param s @p uiSlider
 * @param value to set
 */
API void uiSliderSetValue (uiSlider *s, int value);

/**
 * @brief Checks if the slider has a tool tip
 * @param s @p uiSlider
 * @returns non-zero when true
 */
API int uiSliderHasToolTip (uiSlider *s);

/**
 * @brief Enables or disables the tool tip of a @p uiSlider
 * @param s @p uiSlider
 * @param hasToolTip non-zero to enable, zero to disable
 */
API void uiSliderSetHasToolTip (uiSlider *s, int hasToolTip);

/**
 * @brief Registers a callback for when the slider value is changed by the user.
 * @param s @p uiSlider
 * @param f pointer to the callback function
 * @param data User data to be passed to the callback.
 * @remark the callback is not triggered when calling @p uiSliderSetValue
 * @remark only one callback can be registered at a time
 * @memberof uiSlider
 */
API void uiSliderOnChanged (uiSlider *s, uiSliderCallback *f, void *data);

/**
 * @brief Registers a callback for when the slider is released from dragging.
 * @param s @p uiSlider
 * @param f pointer to the callback function
 * @param data User data to be passed to the callback.
 * @remark only one callback can be registered at a time
 */
API void uiSliderOnReleased (uiSlider *s, uiSliderCallback *f, void *data);

/**
 * @brief Sets the minimum and maximum values of a @p uiSlider
 * @param s @p uiSlider
 * @param min value
 * @param max value
 */
API void uiSliderSetRange (uiSlider *s, int min, int max);

/**
 * @brief @p uiSlider constructor
 * @remark The default value of a @p uiSlider is its minimum value.
 * @remark In the current implementation @p min and @p max are swapped if @code min>max@endcode
 * @param min value
 * @param max value
 * @return @p uiSlider
 */
API uiSlider *uiNewSlider (int min, int max);
