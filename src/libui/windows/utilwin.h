#pragma once

#include <windef.h>

/**
 * @brief The utility window is a special window that performs certain tasks internal to libui.
 *
 * It is a message-only window, and it is always hidden and disabled.
 *
 * Its roles:
 * - It is the initial parent of all controls.
 * - When a control loses its parent, it also becomes that control's parent.
 * - It handles @p WM_QUERYENDSESSION requests.
 * - It handles @p WM_WININICHANGE and forwards the message to any child windows that request it.
 * - It handles executing functions queued to run by @p uiQueueMain
 */
extern HWND utilWindow;

/**
 * @brief @p utilWindow constructor
 * @param hDefaultIcon handle to the application's default icon
 * @param hDefaultCursor handle to the application's default cursor
 * @return error message or @p NULL when successful
 */
extern const char *initUtilWindow (HICON hDefaultIcon, HCURSOR hDefaultCursor);

/**
 * @brief @p utilWindow destructor
 */
extern void uninitUtilWindow ();
