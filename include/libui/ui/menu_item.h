#pragma once

#include "api.h"
#include "window.h"

/**
 * @brief Type-casts a pointer to a @p uiMenuItem
 * @param this pointer to cast
 */
#define uiMenuItem(this) ((uiMenuItem *)(this))

/**
 * @brief An item for a @p uiMenu
 */
typedef struct uiMenuItem uiMenuItem;

/**
 * @brief Callback for @p uiMenuItem events
 * @param sender the @p uiMenuItem that triggered the callback.
 * @param window the @p uiWindow from which the callback was triggered
 * @param senderData user-data registered with @p sender
 */
typedef void (uiMenuItemCallback) (uiMenuItem *sender, uiWindow *window, void *senderData);

/**
 * @brief Enables a @p uiMenuItem
 * @param m @p uiMenuItem
 */
API void uiMenuItemEnable (uiMenuItem *m);

/**
 * @brief Grays out a @p uiMenuItem, disallowing user-interaction
 * @param m @p uiMenuItem
 */
API void uiMenuItemDisable (uiMenuItem *m);

/**
 * @brief Registers a callback for when a @p uiMenuItem is clicked
 * @param m @p uiMenuItem
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark only one callback can be registered at a time
 */
API void uiMenuItemOnClicked (uiMenuItem *m, uiMenuItemCallback *f, void *data);

/**
 * @brief Checks if the checkbox of a @p uiMenuItem is checked
 * @param m @p uiMenuItem
 * @return non-zero when true
 * @remark To be used only with items created via @p uiMenuAppendCheckItem
 */
API int uiMenuItemChecked (uiMenuItem *m);

/**
 * @brief Sets the state of the checkbox for a @p uiMenuItem
 * @param m @p uiMenuItem
 * @param checked non-zero to set to checked, zero to uncheck
 * @remark to be used only with items created via @p uiMenuAppendCheckItem
 */
API void uiMenuItemSetChecked (uiMenuItem *m, int checked);
