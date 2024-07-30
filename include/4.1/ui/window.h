#pragma once

#include <ui/control.h>

#define uiWindow(this) ((uiWindow *)(this))

/**
 * @brief A control that represents a top-level window.
 * @details A window contains exactly one child control that occupied the entire window.
 * @remark Many of the @p uiWindow methods should be regarded as mere hints. The underlying system may override these
 * or even choose to ignore them completely. This is especially true for many Unix systems.
 * @remark A @p uiWindow can NOT be a child of another uiControl.
 */
typedef struct uiWindow uiWindow;

/**
 * @brief Window resize edges.
 */
typedef enum uiWindowResizeEdge
{
  uiWindowResizeEdgeLeft,
  uiWindowResizeEdgeTop,
  uiWindowResizeEdgeRight,
  uiWindowResizeEdgeBottom,
  uiWindowResizeEdgeTopLeft,
  uiWindowResizeEdgeTopRight,
  uiWindowResizeEdgeBottomLeft,
  uiWindowResizeEdgeBottomRight,
} uiWindowResizeEdge;

/**
 * @brief @p uiWindow callback function.
 * @param sender Back reference to the instance that triggered the callback.
 * @param senderData User data registered with the sender instance.
 */
typedef void (uiWindowCallback) (uiWindow *sender, void *senderData);

/**
 * @brief @p uiWindow callback for actions that can be canceled.
 * @param sender Back reference to the instance that triggered the callback.
 * @param senderData User data registered with the sender instance.
 * @retval true abort and keep the window alive and visible
 * @retval false destroy the window,
 */
typedef int (uiWindowCancelableCallback) (uiWindow *sender, void *senderData);

/**
 * @brief Gets the title of the given @p uiWindow instance.
 * @param w @p uiWindow instance.
 * @returns string
 * @remark Caller is responsible for freeing the data with @p uiFreeText
 */
API char *uiWindowTitle (uiWindow *w);

/**
 * @p Sets the title of a @p uiWindow
 * @param w @p uiWindow instance.
 * @param title string
 * @remark Data is copied internally. Ownership is not transferred.
 * @remark This method is merely a hint and may be ignored on unix platforms.
 */
API void uiWindowSetTitle (uiWindow *w, const char *title);

/**
 * @brief Gets the position of a @p uiWindow
 * @param w @p uiWindow instance.
 * @param[out] x Number of pixels from the left of the screen.
 * @param[out] y Number of pixels from the top of the screen.
 * @remark This method may return inaccurate or dummy values on Unix platforms.
 */
API void uiWindowPosition (uiWindow *w, int *x, int *y);

/**
 * @brief Moves the window to the specified position.
 * @param w @p uiWindow instance.
 * @param x Number of pixels from the left of the screen.
 * @param y Number of pixels from the top of the screen.
 * @remark This method is merely a hint and may be ignored on Unix platforms.
 */
API void uiWindowSetPosition (uiWindow *w, int x, int y);

/**
 * @brief Registers a callback for when a @p uiWindow is moved.
 * @param w @p uiWindow instance.
 * @param f Pointer to a function with these parameters:
 *        - @p sender Back reference to the instance that triggered the callback.
 *        - @p senderData User data registered with the sender instance.
 * @param data User data to be passed to the callback.
 * @remark Only one callback can be registered at a time.
 * @remark The callback is not triggered when calling @p uiWindowSetPosition
 */
API void uiWindowOnPositionChanged (uiWindow *w, void (*f) (uiWindow *sender, void *senderData), void *data);

/**
 * @brief Gets the content size of a @p uiWindow
 * @param w @p uiWindow instance.
 * @param[out] width Content width.
 * @param[out] height Content height.
 * @remark The content size is the area inside the window without any menus, title bars, resize grips, etc.
 */
API void uiWindowContentSize (uiWindow *w, int *width, int *height);

/**
 * @brief Sets the content size of a @p uiWindow
 * @param w uiWindow instance.
 * @param width Window content width to set.
 * @param height Window content height to set.
 * @remark The content size is the area inside the window without any menus, title bars, resize grips, etc.
 * @remark This method is merely a hint and may be ignored by the system.
 */
API void uiWindowSetContentSize (uiWindow *w, int width, int height);

/**
 * @brief Checks if a @p uiWindow is full screen.
 * @param w @p uiWindow instance.
 * @returns Non-zero when true.
 */
API int uiWindowFullscreen (uiWindow *w);

/**
 * @brief Enters full-screen mode for a @p uiWindow
 * @param w @p uiWindow instance.
 * @param fullscreen Non-zero for full-screen, zero to return to normal.
 * @remark This method is merely a hint and may be ignored by the system.
 */
API void uiWindowSetFullscreen (uiWindow *w, int fullscreen);

/**
 * @brief Registers a callback for when a @p uiWindow content size is changed.
 * @param w @p uiWindow
 * @param f @p uiWindowCallback
 * @param data User data to be passed to the callback.
 * @remark The callback is not triggered when calling @p uiWindowSetContentSize
 * @remark Only one callback can be registered at a time.
 */
API void uiWindowOnContentSizeChanged (uiWindow *w, uiWindowCallback *f, void *data);

/**
 * @brief Registers a callback for when a @p uiWindow is closing.
 * @param w @p uiWindow
 * @param f @p uiWindowCancelableCallback
 * @param data User data to be passed to the callback.
 * @remark Only one callback can be registered at a time.
 */
API void uiWindowOnClosing (uiWindow *w, uiWindowCancelableCallback *f, void *data);

/**
 * @brief Registers a callback for when a @p uiWindow changes focus.
 * @param w @p uiWindow
 * @param f @p uiWindowCallback
 * @param data User data to be passed to the callback.
 * @remark Only one callback can be registered at a time.
 * @memberof uiWindow
 */
API void uiWindowOnFocusChanged (uiWindow *w, uiWindowCallback *f, void *data);

/**
 * Returns whether or not the window is focused.
 *
 * @param w uiWindow instance.
 * @returns `TRUE` if window is focused, `FALSE` otherwise.
 * @memberof uiWindow
 */
API int uiWindowFocused (uiWindow *w);

/**
 * @brief Checks if a @p uiWindow has a border.
 * @param w uiWindow
 * @returns @p 0 if the window has a border.
 */
API int uiWindowBorderless (uiWindow *w);

/**
 * @brief Turns the border of a @p uiWindow on or off.
 * @param w @p uiWindow
 * @param borderless Non-zero to turn the border off.
 * @remark This method is merely a hint and may be ignored by the system.
 */
API void uiWindowSetBorderless (uiWindow *w, int borderless);

/**
 * @brief Set the child @p uiControl of a @p uiWindow
 * @param w @p uiWindow
 * @param child @p uiControl
 */
API void uiWindowSetChild (uiWindow *w, uiControl *child);

/**
 * @brief Checks if a @p uiWindow has a margin
 * @param w @p uiWindow
 * @returns Non-zero when true.
 */
API int uiWindowMargined (uiWindow *w);

/**
 * @brief Enables or disables the margin of a @p uiWindow
 * @remark A window's margin size is system-dependent.
 * @param w @p uiWindow
 * @param margined Non-zero to turn the margin on, zero to turn it off.
 */
API void uiWindowSetMargined (uiWindow *w, int margined);

/**
 * @brief Checks if a @p uiWindow can be resized by the user
 * @param w @p uiWindow
 * @returns Non-zero when true.
 */
API int uiWindowResizeable (uiWindow *w);

/**
 * Sets whether or not the window is user resizeable.
 *
 * @param w uiWindow instance.
 * @param resizeable `TRUE` to make window resizable, `FALSE` otherwise.
 * @note This method is merely a hint and may be ignored by the system.
 * @memberof uiWindow
 */
API void uiWindowSetResizeable (uiWindow *w, int resizeable);

/**
 * Creates a new uiWindow.
 *
 * @param title Window title text.\n
 *              A valid, `NUL` terminated UTF-8 string.\n
 *              Data is copied internally. Ownership is not transferred.
 * @param width Window width.
 * @param height Window height.
 * @param hasMenubar Whether or not the window should display a menu bar.
 * @returns A new uiWindow instance.
 * @memberof uiWindow @static
 */
API uiWindow *uiNewWindow (const char *title, int width, int height, int hasMenubar);
