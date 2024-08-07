#pragma once

/**
 * @brief A control that represents a top-level window.
 * @details A window contains exactly one child control that occupied the entire window.
 * @remark Many of a window's methods should be regarded as mere hints. The underlying system may override these, or
 * even choose to ignore them completely. This is especially true for many Unix systems.
 * @remark A window can only be the child of another window.
 */
struct ui_window_t;

/**
 * @brief @p ui_window_t constructor.
 * @param title of the window.
 * @param width in pixels.
 * @param height in pixels.
 * @param has_menu non-zero if the window has a menu bar.
 * @return @p ui_window_t
 */
struct ui_window_t *ui_window_create (const char *title, int width, int height, int has_menu);

/**
 * @brief @p ui_window_t destructor.
 * @param window @p ui_window_t
 */
void ui_window_destroy (struct ui_window_t *window);

/**
 * @brief Checks if a window has a border.
 * @param window @p ui_window_t
 * @returns Zero if the window has a border.
 */
int ui_window_get_borderless (struct ui_window_t *window);

/**
 * @brief Gets the content size of a window.
 * @param window @p ui_window_t
 * @param[out] width of the window's content.
 * @param[out] height of the window's content.
 * @remark The content size is the area inside the window without any menus, title bars, resize grips, etc.
 */
void ui_window_get_content_size (struct ui_window_t *window, int *width, int *height);

/**
 * @brief Checks if a window has focus.
 * @param window @p ui_window_t
 * @returns non-zero when the window has focus.
 */
int ui_window_get_focused (struct ui_window_t *window);

/**
 * @brief Checks if a window is full screen.
 * @param window @p ui_window_t
 * @returns Non-zero when full screen.
 */
int ui_window_get_fullscreen (struct ui_window_t *window);

/**
 * @brief Gets the position of of a window.
 * @param window @p ui_window_t
 * @param[out] x Number of pixels from the left of the screen.
 * @param[out] y Number of pixels from the top of the screen.
 * @remark This method may return inaccurate or dummy values on Unix platforms.
 */
void ui_window_get_position (struct ui_window_t *window, int *x, int *y);

/**
 * @brief Checks if a window can be resized by the user.
 * @param window @p ui_window_t
 * @returns Non-zero when the window is resizable.
 */
int ui_window_get_resizable (struct ui_window_t *window);

/**
 * @brief Gets the title of the given window.
 * @param window @p ui_window_t
 * @returns The window's title.
 * @remark Caller is responsible for freeing the data with @p uiFreeText
 */
char *ui_window_get_title (struct ui_window_t *window);

/**
 * @brief Hides a window.
 * @param window @p ui_window_t
 */
void ui_window_hide (struct ui_window_t *window);

/**
 * @brief Turns a window's border on or off.
 * @param window *p ui_window_t
 * @param borderless Non-zero to turn the border off.
 * @remark This method is merely a hint and may be ignored by the system.
 */
void ui_window_set_borderless (struct ui_window_t *window, int borderless);

/**
 * @brief Sets the content size of a window.
 * @param window *p ui_window_t
 * @param width of the window's content.
 * @param height of the window's content.
 * @remark The content size is the area inside the window without any menus, title bars, resize grips, etc.
 * @remark This method is merely a hint and may be ignored by the system.
 */
void ui_window_set_content_size (struct ui_window_t *window, int width, int height);

/**
 * @brief Enters or leaves full screen for a window.
 * @param window @p ui_window_t
 * @param fullscreen Non-zero for full-screen, zero to return to normal.
 * @remark This method is merely a hint and may be ignored by the system.
 */
void ui_window_set_fullscreen (struct ui_window_t *window, int fullscreen);

/**
 * @brief Moves the window to the specified position.
 * @param window @p ui_window_t
 * @param x Number of pixels from the left of the screen.
 * @param y Number of pixels from the top of the screen.
 * @remark This method is merely a hint and may be ignored on Unix platforms.
 */
void ui_window_set_position (struct ui_window_t *window, int x, int y);

/**
 * @brief Sets whether or not the window is user resizeable.
 * @param window @p ui_window_t
 * @param resizable non-zero to make resizable.
 * @remark This method is merely a hint and may be ignored by the system.
 */
void ui_window_set_resizable (struct ui_window_t *window, int resizable);

/**
 * @brief Sets a window's title.
 * @param window @p ui_window_t
 * @param title of the window.
 * @remark Data is copied internally. Ownership is not transferred.
 * @remark This method is merely a hint and may be ignored on unix platforms.
 */
void ui_window_set_title (struct ui_window_t *window, const char *title);

/**
 * @brief Shows a window.
 * @param window @p ui_window_t
 */
void ui_window_show (struct ui_window_t *window);
