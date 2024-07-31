#pragma once

/**
* @brief OSX implementation data.
 */
struct ui_osx_t
{
  /**
   * @brief Set to non-zero when the application should quit.
   */
  int quit;
};

/**
 * @brief Dispatches the most recent message.
 * @param ui_osx @p ui_osx_t
 */
void ui_osx_dispatch (struct ui_osx_t *ui_osx);

/**
 * @brief Blocks the calling thread until a new message is ready for dispatch.
 * @param ui_osx @p ui_osx_t
 */
void ui_osx_update (struct ui_osx_t *ui_osx);
