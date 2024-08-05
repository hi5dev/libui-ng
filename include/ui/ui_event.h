#pragma once

/**
 * @brief Event data.
 */
struct ui_event_t
{
  /**
   * @brief Callback function.
   */
  struct ui_event_cb *callback;

  /**
   * @brief User-defined data.
   */
  void *data;
};

/**
 * @brief Event callback function.
 * @param event event data.
 * @param data user-defined data.
 * @return non-zero if the event has been handled and should not be processed any further.
 */
typedef int (ui_event_cb) (const struct ui_event_t *event, void *data);

/**
 * @brief Registers an event callback.
 * @param event to register.
 * @param data user-defined data to pass to the callback function.
 */
void ui_event_cb_register (struct ui_event_t *event, void *data);
