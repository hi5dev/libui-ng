#pragma once

/**
 * @brief Default duration to use for the timer when the application starts.
 */
#define UI_TIMER_DEFAULT_DURATION 10

/**
 * @brief Timer application data.
 */
struct ui_timer_t;

/**
 * @brief Starts a timer.
 * @param timer @p ui_timer_t
 */
void ui_timer_start (struct ui_timer_t *timer);

/**
 * @brief Stops/pauses a timer.
 * @param timer @p ui_timer_t
 */
void ui_timer_stop (struct ui_timer_t *timer);

/**
 * @brief Resets a timer's elapsed time.
 * @param timer @p ui_timer_t
 */
void ui_timer_reset (struct ui_timer_t *timer);

/**
 * @brief Runs the Timer application.
 * @param timer @p ui_timer_t
 */
int ui_timer_main (struct ui_timer_t *timer);
