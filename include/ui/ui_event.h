#pragma once

/**
 * @brief Event callback function.
 * @param e event data.
 * @param data user-defined data.
 * @return non-zero if the event has been handled and should not be processed any further.
 */
typedef int (ui_event_t) (const struct event *e, void *data);

/**
 * @brief Event callback data.
 */
struct ui_event
{
  /**
   * @brief Callback function.
   */
  ui_event_t *event;

  /**
   * @brief User-defined data.
   */
  void *data;
};

/**
 * @brief Registers an event callback.
 * @param event to register.
 * @param data user-defined data to pass to the callback function.
 */
void ui_event_cb_register (struct ui_event *event, void *data);
