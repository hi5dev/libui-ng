#pragma once

#include "ui_win32.h"

/**
 * @brief Win32 application runner.
 * @param ui_win32 platform-specific application data.
 * @return non-zero when exiting an error.
 */
int ui_win32_main (struct ui_win32_t *ui_win32);
