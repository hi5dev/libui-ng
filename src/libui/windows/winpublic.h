#pragma once

#include <windef.h>

/**
 * @brief Used to set the Z-order of a chain of controls.
 * @param hwnd handle to a window
 * @param[out] controlID input is the source control's ID, output is the next control's ID
 * @param[out] insertAfter input is the target window's handle, output is the next window's handle
 */
void uiWindowsEnsureAssignControlIDZOrder (HWND hwnd, LONG_PTR *controlID, HWND *insertAfter);

/**
 * @brief Destroys a window, logging any errors on failure.
 * @param hwnd handle of the window to destroy.
 */
void uiWindowsEnsureDestroyWindow (HWND hwnd);

/**
 * @brief Retrieves the coordinates of a window's client area, logging any errors.
 * @param hwnd handle of a window
 * @param[out] r receives the client coordinates
 */
void uiWindowsEnsureGetClientRect (HWND hwnd, RECT *r);

/**
 * @brief Retrieves the dimensions of the bounding rectangle of the specified window, logging any errors.
 * @param hwnd handle of a window
 * @param[out] r receives the client coordinates
 */
void uiWindowsEnsureGetWindowRect (HWND hwnd, RECT *r);

/**
 * @brief Moves and resizes a window, logging any errors.
 * @param hwnd handle of a window
 * @param x position
 * @param y position
 * @param width size
 * @param height size
 */
void uiWindowsEnsureMoveWindowDuringResize (HWND hwnd, int x, int y, int width, int height);

/**
 * @brief Sets a window's parent, logging any errors.
 * @remark Ignores @p NULL handles.
 * @param hwnd handle of the child window
 * @param parent handle of the parent window
 */
void uiWindowsEnsureSetParentHWND (HWND hwnd, HWND parent);
