#pragma once

#include "api.h"
#include "control.h"

/**
 * @brief Parameter to editable model columns to signify all rows are always editable.
 */
#define uiTableModelColumnAlwaysEditable (-2)

#define uiTab(this) ((uiTab *)(this))

/**
 * @brief A multi page control interface that displays one page at a time.
 *
 * Each page/tab has an associated label that can be selected to switch between pages/tabs.
 */
typedef struct uiTab uiTab;

/**
 * @brief Appends a @p uiControl to a @p uiTab
 * @param t @p uiTab
 * @param name label text (string)
 * @param c @p uiControl to append
 * @remark @p name is copied internally; ownership is not transferred
 */
API void uiTabAppend (uiTab *t, const char *name, uiControl *c);

/**
 * @brief Inserts a @p uiControl to a @p uiTab at a given index
 * @param t @p uiTab
 * @param name label text (string)
 * @param index at which to insert the @p uiControl
 * @param c @p uiControl
 * @remark @p name is copied internally; ownership is not transferred
 */
API void uiTabInsertAt (uiTab *t, const char *name, int index, uiControl *c);

/**
 * @brief Removes the @uiControl at the given index from a @p uiTab
 * @param t @p uiTab
 * @param index of the @p uiControl to be removed
 * @remark the @p uiControl is neither destroyed nor freed
 */
API void uiTabDelete (uiTab *t, int index);

/**
 * @brief Gets the number of pages in a @p uiTab
 * @param t @p uiTab
 * @return count
 */
API int uiTabNumPages (uiTab *t);

/**
 * @brief Checks if the @p uiControl of a @p uiTab at a given index has a margin
 * @param t @p uiTab
 * @param index of the @p uiControl
 * @returns non-zero when true
 */
API int uiTabMargined (uiTab *t, int index);

/**
 * @brief Enables or disables the margin of a @p uiControl in a @p uiTab
 * @param t @p uiTab
 * @param index of the @p uiControl
 * @param margined non-zero to enable, zero to disable
 * @remark The margin size is determined by the OS defaults.
 */
API void uiTabSetMargined (uiTab *t, int index, int margined);

/**
 * @p uiTab constructor
 * @return @p uiTab
 */
API uiTab *uiNewTab (void);
