#pragma once

#include "api.h"
#include "control.h"

/**
 * @brief Type-casts a pointer to a @p uiBox
 * @param this pointer to cast
 */
#define uiBox(this) ((uiBox *)(this))

/**
 * @brief A box-like container that holds a group of controls.
 * @details The contained controls are arranged to be displayed either horizontally or vertically next to each other.
 */
typedef struct uiBox uiBox;

/**
 * @brief Appends a @p uiControl to a @p uiBox
 * @details Stretchy items expand to use the remaining space within the box. In the case of multiple stretchy items the
 * space is shared equally.
 * @param b @p uiBox
 * @param child @p uiControl to append
 * @param stretchy non-zero for true
 */
API void uiBoxAppend (uiBox *b, uiControl *child, int stretchy);

/**
 * @brief Gets the number of @p uiControl instances contained within the box.
 * @param b @p uiBox
 * @returns number of children
 */
API int uiBoxNumChildren (uiBox *b);

/**
 * @brief Removes the @p uiControl from a @p uiBox based on its index
 * @param b @p uiBox
 * @param index of the @p uiControl to be removed
 * @remark The @p uiControl is neither destroyed nor freed.
 */
API void uiBoxDelete (uiBox *b, int index);

/**
 * @brief Checks if @p uiControls in a @p uiBox are padded.
 * @param b uiBox instance.
 * @returns non-zero when true
 * @remark Padding is defined as space between individual controls.
 */
API int uiBoxPadded (uiBox *b);

/**
 * @brief Enables or disables padding for a @p uiBox
 * @param b @p uiBox
 * @param padded non-zero to enable, zero to disable
 * @remark Padding is defined as space between individual controls, and is determined by the system defaults.
 */
API void uiBoxSetPadded (uiBox *b, int padded);

/**
 * @brief Creates a @p uiBox with controls placed next to each other horizontally
 * @returns @p uiBox
 */
API uiBox *uiNewHorizontalBox (void);

/**
 * @brief Creates a @p uiBox with controls placed next to each other vertically
 * @returns @p uiBox
 */
API uiBox *uiNewVerticalBox (void);
