#pragma once

#include "ui_osx.h"

/**
 * @brief OSX application runner.
 * @param ui_osx platform-specific application data.
 * @return non-zero when exiting with an error.
 */
int ui_osx_main (struct ui_osx_t *ui_osx);
