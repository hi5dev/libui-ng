#pragma once

#include "api.h"
#include "window.h"

/**
 * @brief Opens the operating system's native file-chooser dialog window for selecting a single file
 * @param parent @p uiWindow
 * @return path to the file or @p NULL when canceled
 * @remark caller is responsible for freeing the returned text
 * @remark paths are separated by the underlying OS file path separator
 * @see uiFreeText
 */
API char *uiOpenFile (uiWindow *parent);

/**
 * @brief Opens the operating system's native folder-chooser dialog window to select a single folder.
 * @param parent @p uiWindow
 * @return path to the folder or @p NULL when canceled
 * @remark caller is responsible for freeing the returned text
 * @remark paths are separated by the underlying OS file path separator
 * @see uiFreeText
 */
API char *uiOpenFolder (uiWindow *parent);

/**
 * @brief Opens the operating system's native save-file dialog window.
 * @param parent @p uiWindow
 * @returns path to the file or @p NULL when canceled
 * @remark caller is responsible for freeing the returned text
 * @remark The user is asked to confirm overwriting existing files.
 * @remark paths are separated by the underlying OS file path separator
 * @see uiFreeText
 */
API char *uiSaveFile (uiWindow *parent);

/**
 * @brief Opens the operating system's native message box
 * @param parent @p uiWindow
 * @param title of the dialog box (string)
 * @param description inside the dialog box (string)
 * @remark @p title and @p description are copied internally; ownership is not transferred
 */
API void uiMsgBox (uiWindow *parent, const char *title, const char *description);

/**
 * @brief Opens the operating system's native error message box
 * @param parent @p uiWindow
 * @param title Dialog window title text.\n
 * @param title of the dialog box (string)
 * @param description inside the dialog box (string)
 * @remark @p title and @p description are copied internally; ownership is not transferred
 */
API void uiMsgBoxError (uiWindow *parent, const char *title, const char *description);
