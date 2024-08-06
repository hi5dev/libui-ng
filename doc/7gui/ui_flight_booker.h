#pragma once

/**
 * @brief Flight Booker application data.
 */
struct ui_flight_booker_t;

/**
 * @brief Runs the Flight Booker application.
 * @param flight_booker @p ui_flight_booker_t
 * @return non-zero when exiting with an error.
 */
int ui_flight_booker_main (struct ui_flight_booker_t *flight_booker);
