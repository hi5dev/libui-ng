#pragma once

/**
 * @brief Counter application data
 */
struct ui_counter_t;

/**
 * @brief Call when the user clicks the button.
 * @param counter @p ui_counter_t
 */
void ui_counter_increment (struct ui_counter_t *counter);

/**
 * @brief The counter application's main entry-point.
 * @param counter @p ui_counter_t
 * @return non-zero when exiting with an error.
 */
int ui_counter_main (struct ui_counter_t *counter);
