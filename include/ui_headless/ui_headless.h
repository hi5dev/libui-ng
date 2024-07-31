#pragma once

/**
 * @brief Headless implementation data.
 */
struct ui_headless_t
{
  /**
   * @brief Set to non-zero when the application should quit.
   */
  int quit;
};

/**
 * @brief Dispatches the most recent message.
 * @param ui_headless @p ui_headless_t
 */
void ui_headless_dispatch (struct ui_headless_t *ui_headless);

/**
 * @brief Blocks the calling thread until a new message is ready for dispatch.
 * @param ui_headless @p ui_headless_t
 */
void ui_headless_update (struct ui_headless_t *ui_headless);
