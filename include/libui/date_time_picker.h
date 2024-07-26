#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiDateTimePicker
 * @param this pointer to cast
 */
#define uiDateTimePicker(this) ((uiDateTimePicker *)(this))

/**
 * @see time.h
 */
struct tm;

/**
 * @brief A @p uiControl to enter a date and/or time
 *
 * - All functions operate on @code struct tm@endcode as defined in @code time.h@endcode
 * - All functions assume local time and do NOT perform any time zone conversions.
 *
 * @remark The @code struct tm@endcode members @p tm_wday and @p tm_yday are left undefined
 * @remark The @code struct tm@endcode member @p tm_isdst is ignored on windows and should be set to @p -1
 */
typedef struct uiDateTimePicker uiDateTimePicker;

/**
 * @brief Callback for @p uiDateTimePicker events
 * @param sender reference to the instance that triggered the callback
 * @param senderData user-data registered with the sender instance
 */
typedef void (uiDateTimePickerCallback) (uiDateTimePicker *sender, void *senderData);

/**
 * @brief Gets the date and time stored in a @p uiDateTimePicker
 * @param d @p uiDateTimePicker
 * @param[out] time date and/or time as local time
 * @remark The @code struct tm@endcode members @p tm_wday and @p tm_yday are left undefined
 */
API void uiDateTimePickerTime (uiDateTimePicker *d, struct tm *time);

/**
 * @brief Sets date and time of a @p uiDateTimePicker
 * @param d @p uiDateTimePicker
 * @param time date and/or time as local time
 * @remark The @code struct tm@endcode members @p tm_wday and @p tm_yday are left undefined
 */
API void uiDateTimePickerSetTime (uiDateTimePicker *d, const struct tm *time);

/**
 * @brief Registers a callback for when the @p uiDateTimePicker value is changed by the user
 * @param d @p uiDateTimePicker
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark the callback is not triggered when calling @p uiDateTimePickerSetTime
 * @remark only one callback can be registered at a time
 */
API void uiDateTimePickerOnChanged (uiDateTimePicker *d, uiDateTimePickerCallback *f, void *data);

/**
 * @brief @p uiDateTimePicker constructor
 * @return @p uiDateTimePicker
 */
API uiDateTimePicker *uiNewDateTimePicker (void);

/**
 * @brief Constructs a @p uiDateTimePicker for only selecting a date
 * @returns @p uiDateTimePicker
 */
API uiDateTimePicker *uiNewDatePicker (void);

/**
 * @brief Constructs a @p uiDateTimePicker for only selecting a time
 * @return @p uiDateTimePicker
 */
API uiDateTimePicker *uiNewTimePicker (void);
