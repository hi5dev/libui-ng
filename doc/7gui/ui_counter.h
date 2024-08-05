#pragma once

/**
 * @brief Counter data.
 */
struct ui_counter_t;

/**
 * @brief Gets a counter's current value.
 * @param counter @p ui_counter_t
 * @return integer value.
 */
typedef int (ui_counter_get_value_cb) (struct ui_counter_t *counter);

struct ui_counter_t
{
  /**
   * @brief Private implementation.
   */
  void *impl;

  /**
   * @implements ui_counter_get_value_cb
   */
  ui_counter_get_value_cb *get_value;
};

/**
 * @brief @p ui_counter_t constructor
 * @return @p ui_counter_t
 */
struct ui_counter_t *ui_counter_create (void);

/**
 * @brief @p ui_counter_t destructor
 * @param counter @p ui_counter_t
 */
void ui_counter_destroy (struct ui_counter_t *counter);

/**
 * @see ui_counter_get_value_cb
 * @param counter @p ui_counter_t
 * @return integer value.
 */
int ui_counter_get_value (struct ui_counter_t *counter);

/**
 * @brief The counter application's main entry-point.
 * @param argc command-line argument count.
 * @param argv command-line argument vector.
 * @return non-zero when exiting with an error.
 */
int ui_counter_main (int argc, char **argv);
