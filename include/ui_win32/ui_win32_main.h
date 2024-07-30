#pragma once

#include <windef.h>

/**
 * @brief Win32 entry-point.
 * @param hInstance is the handle to an instance or handle to a module. The operating system uses this value to
 * identify the executable or EXE when it's loaded in memory. Certain Windows functions need the instance handle,
 * for example to load icons or bitmaps.
 * @param hPrevInstance has no meaning. It was used in 16-bit Windows, but is now always zero.
 * @param lpCmdLine contains the command-line arguments as a Unicode string.
 * @param nShowCmd is a flag that indicates whether the main application window is minimized, maximized, or shown
 * normally.
 * @return non-zero when the application exits with a failure.
 */
int WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd);
