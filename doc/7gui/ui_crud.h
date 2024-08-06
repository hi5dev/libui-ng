#pragma once

/**
 * @brief CRUD application data.
 */
struct ui_crud_t;

/**
 * @brief Runs the CRUD application.
 * @param crud @p ui_crud_t
 * @return non-zero when exiting with an error.
 */
int ui_crud_main (struct ui_crud_t *crud);
