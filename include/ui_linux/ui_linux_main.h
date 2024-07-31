#pragma once

#include "ui_linux.h"

/**
 * @brief Linux application runner.
 * @param ui_linux platform-specific application data.
 * @return non-zero when exiting an error.
 */
int ui_linux_main (struct ui_linux_t *ui_linux);
