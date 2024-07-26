#pragma once

#include "api.h"
#include "menu_item.h"

/**
 * @brief Type-casts a pointer to a @p uiMenu
 * @param this pointer to cast
 */
#define uiMenu(this) ((uiMenu *)(this))

/**
 * @brief An application-level menu bar.
 *
 * The various operating systems impose different requirements on the creation and placement of menu bar items,
 * hence the abstraction of the items `Quit`, `Preferences` and `About`.
 *
 * An exemplary, cross platform menu bar:
 *
 * - File
 *   - New
 *   - Open
 *   - Save
 *   - Quit, _use @p uiMenuAppendQuitItem_
 * - Edit
 *   - Undo
 *   - Redo
 *   - Cut
 *   - Copy
 *   - Paste
 *   - Select All
 *   - Preferences, _use @p uiMenuAppendPreferencesItem_
 * - Help
 *   - About, _use @p uiMenuAppendAboutItem_
 */
typedef struct uiMenu uiMenu;

/**
 * @brief Appends a @p uiMenuItem to a @p uiMenu
 * @param m @p uiMenu
 * @param name string
 * @remark @p name is copied internally; ownership is not transferred
 * @return @p uiMenuItem
 */
API uiMenuItem *uiMenuAppendItem (uiMenu *m, const char *name);

/**
 * @brief Appends a checkable @p uiMenuItem to a @p uiMenu
 * @param m @p uiMenu
 * @param name string
 * @remark @p name is copied internally; ownership is not transferred
 * @return @p uiMenuItem
 */
API uiMenuItem *uiMenuAppendCheckItem (uiMenu *m, const char *name);

/**
 * @brief Appends a new "Quit" @p uiMenuItem to a @p uiMenu
 * @param m @p uiMenu
 * @return @p uiMenuItem
 * @remark only one such menu item may exist per application
 */
API uiMenuItem *uiMenuAppendQuitItem (uiMenu *m);

/**
 * @brief Appends a new "Preferences" @p uiMenuItem to a @p uiMenu
 *
 * @param m uiMenu instance.
 * @returns A new uiMenuItem instance.
 * @warning Only one such menu item may exist per application.
 * @memberof uiMenu
 */
API uiMenuItem *uiMenuAppendPreferencesItem (uiMenu *m);

/**
 * Appends a new "About" @p uiMenuItem to a @p uiMenu
 * @param m @p uiMenu
 * @return @p uiMenuItem
 * @remark only one such menu item may exist per application
 */
API uiMenuItem *uiMenuAppendAboutItem (uiMenu *m);

/**
 * Appends a new separator to a @p uiMenu
 * @param m @p uiMenu
 */
API void uiMenuAppendSeparator (uiMenu *m);

/**
 * @brief @p uiMenu constructor
 * @details Typical values are @p "File", @p "Edit", @p "Help"
 * @param name string
 * @return @p uiMenu
 * @remark @p name is copied internally; ownership is not transferred
 */
API uiMenu *uiNewMenu (const char *name);
