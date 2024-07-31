#pragma once

/**
 * @brief Linux implementation.
 */
struct ui_linux_t
{
  /**
   * @brief Set to non-zero when the application should quit.
   */
  int quit;
};

/**
 * @brief Dispatches the most recent message.
 * @param ui_linux @p ui_linux_t
 */
void ui_linux_dispatch (struct ui_linux_t *ui_linux);

/**
 * @brief Blocks the calling thread until a new message is ready for dispatch.
 * @param ui_linux @p ui_linux_t
 */
void ui_linux_update (struct ui_linux_t *ui_linux);
