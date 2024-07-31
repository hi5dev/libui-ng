#pragma once

#include "ui_headless.h"

/**
 * @brief Headless application runner.
 * @param ui_headless platform-specific application data.
 * @return non-zero when exiting with an error.
 */
int ui_headless_main (struct ui_headless_t *ui_headless);
