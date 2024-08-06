#pragma once

/**
 * @brief Circle Drawer application data.
 */
struct ui_circle_drawer_t;

/**
 * @brief Runs the Circle Drawer application.
 * @param circle_drawer @p ui_circle_drawer_t
 * @return non-zero when exiting with an error.
 */
int ui_circle_drawer_create(struct ui_circle_drawer_t *circle_drawer);
