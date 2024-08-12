#pragma once

#include <sys/time.h>

/**
 * @brief Event data.
 */
struct ui_event_t
{
  /// @brief ID of the event's type as defined by its producer.
  unsigned long id;

  /// @brief When the event was created.
  struct timeval timestamp;

  /// @brief User-defined data.
  intptr_t data;
};

/**
 * @brief Event callback.
 * @param event @p ui_event_t
 * @param data user-defined data.
 * @return non-zero to prevent any more callbacks from processing the event.
 */
typedef int (ui_event_cb) (struct ui_event_t *event, intptr_t data);

/**
 * @brief Initializes a stack-allocated @p ui_event_t instance.
 * @param id of the event.
 * @param data to send to the event callbacks.
 * @return @p ui_event_t
 */
struct ui_event_t ui_event_init (unsigned long id, intptr_t data);
