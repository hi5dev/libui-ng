#pragma once

#include "alignment.h"

/**
 * @brief Type-casts a pointer to a @p uiGrid
 * @param this pointer to cast
 */
#define uiGrid(this) ((uiGrid *)(this))

/**
 * @brief A control container to arrange containing controls in a grid.
 *
 * Contained controls are arranged on an imaginary grid of rows and columns. Controls can be placed anywhere on this
 * grid, spanning multiple rows and/or columns.
 *
 * Additionally placed controls can be programmed to expand horizontally and/or vertically, sharing the remaining space
 * among other expanded controls.
 *
 * Alignment options are available via @p uiAlign attributes to determine the controls placement within the reserved
 * area, should the area be bigger than the control itself.
 *
 * Controls can also be placed in relation to other controls using @p uiAt attributes.
 */
typedef struct uiGrid uiGrid;

/**
 * @brief Appends a @p uiControl to a @p uiGrid
 * @param g @p uiGrid
 * @param c @p uiControl to insert
 * @param left Placement as number of columns from the left. Integer in range of `[INT_MIN, INT_MAX]`
 * @param top Placement as number of rows from the top. Integer in range of `[INT_MIN, INT_MAX]`
 * @param xspan Number of columns to span. Integer in range of `[0, INT_MAX]`
 * @param yspan Number of rows to span. Integer in range of `[0, INT_MAX]`
 * @param hexpand non-zero to expand reserved area horizontally
 * @param halign Horizontal alignment of the control within the reserved space
 * @param vexpand non-zero to expand reserved area vertically
 * @param valign Vertical alignment of the control within the reserved space
 */
API void uiGridAppend (uiGrid *g, uiControl *c, int left, int top, int xspan, int yspan, int hexpand, uiAlign halign,
                       int vexpand, uiAlign valign);

/**
 * @brief Inserts a @p uiControl positioned in relation to another control within a @p uiGrid
 * @param g @p uiGrid
 * @param c @p uiControl to insert
 * @param existing The existing control to position relatively to
 * @param at Placement specifier in relation to @p existing control
 * @param xspan Number of columns to span. Integer in range of `[0, INT_MAX]`
 * @param yspan Number of rows to span. Integer in range of `[0, INT_MAX]`
 * @param hexpand non-zero to expand reserved area horizontally
 * @param halign Horizontal alignment of the control within the reserved space
 * @param vexpand non-zero to expand reserved area vertically
 * @param valign Vertical alignment of the control within the reserved space
 * @memberof uiGrid
 */
API void uiGridInsertAt (uiGrid *g, uiControl *c, uiControl *existing, uiAt at, int xspan, int yspan, int hexpand,
                         uiAlign halign, int vexpand, uiAlign valign);

/**
 * @brief Checks if the controls within a @p uiGrid are padded
 * @param g @p uiGrid
 * @return non-zero when true
 * @remark padding is defined as space between individual controls
 */
API int uiGridPadded (uiGrid *g);

/**
 * @brief Enables or disables the padding of a @p uiGrid
 * @param g @p uiGrid
 * @param padded non-zero to enable padding, zero to disable padding
 * @brief padding is defined as space between individual controls, the size of which is determined by the system
 */
API void uiGridSetPadded (uiGrid *g, int padded);

/**
 * @brief uiGrid constructor
 * @return @p uiGrid
 */
API uiGrid *uiNewGrid (void);
