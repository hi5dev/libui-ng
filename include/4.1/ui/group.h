#pragma once

/**
 * @brief Type-casts a pointer to a @p uiGroup
 * @param this pointer to cast
 */
#define uiGroup(this) ((uiGroup *)(this))

/**
 * @brief A control container that adds a label to the contained child control.
 *
 * This control is a great way of grouping related controls in combination with @p uiBox.
 *
 * A visual box will or will not be drawn around the child control dependent on the underlying OS implementation.
 */
typedef struct uiGroup uiGroup;

/**
 * @brief Gets the title of a @p uiGroup
 * @param g @p uiGroup
 * @return string
 * @remark caller is responsible for freeing the returned string
 * @see uiFreeText
 */
API char *uiGroupTitle (uiGroup *g);

/**
 * @brief Sets the title of a @p uiGroup
 * @param g @p uiGroup
 * @param title text
 * @remark @p title is copied internally; ownership is not transferred
 */
API void uiGroupSetTitle (uiGroup *g, const char *title);

/**
 * @brief Sets the child @p uiControl of a @p uiGroup
 * @param g @p uiGroup
 * @param c @p uiControl or @p NULL to remove the existing child
 */
API void uiGroupSetChild (uiGroup *g, uiControl *c);

/**
 * @brief Checks if a @p uiGgroup has a margin
 * @param g @p uiGroup
 * @return non-zero when true
 */
API int uiGroupMargined (uiGroup *g);

/**
 * @brief Enables or disables the margin of a @p uiGroup
 * @param g @p uiGroup
 * @param margined non-zero to enable, zero to disable
 * @remark margin size is determined by the system
 */
API void uiGroupSetMargined (uiGroup *g, int margined);

/**
 * @brief @p uiGroup constructor
 * @param title of the group (string)
 * @remark @p title is copied internally; ownership is not transferred
 * @return @p uiGroup
 */
API uiGroup *uiNewGroup (const char *title);
