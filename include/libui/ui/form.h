#pragma once

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiForm
 * @param this pointer to cast
 */
#define uiForm(this) ((uiForm *)(this))

/**
 * @brief A container control to organize contained controls as labeled fields.
 * @details As the name suggests this container is perfect to create ascetically pleasing input forms.
 * - Each control is preceded by it's corresponding label.
 * - Labels and containers are organized into two panes, making both labels and containers align with each other.
 */
typedef struct uiForm uiForm;

/**
 * @brief Appends a @p uiControl to a @p uiForm and creates an associated label
 * @param f @p uiForm
 * @param label text
 * @param c @p uiControl
 * @param stretchy non-zero to stretch the control to the remaining space within the container
 * @remark in the case of multiple stretchy items the space is shared equally
 * @remark @p label text is copied internally; ownership is not transferred
 */
API void uiFormAppend (uiForm *f, const char *label, uiControl *c, int stretchy);

/**
 * @brief Gets the number of controls in a @p uiForm
 * @param f @p uiForm
 * @return count
 */
API int uiFormNumChildren (uiForm *f);

/**
 * @brief Removes a @p uiControl from a @p uiForm by its index
 * @param f @p uiForm
 * @param index of the @p uiControl to be removed
 * @remark The @p uiControl that was removed is neither destroyed nor freed
 */
API void uiFormDelete (uiForm *f, int index);

/**
 * @brief Checks if controls within the form are padded
 * @param f @p uiForm
 * @returns non-zero when true
 * @remark Padding is defined as space between individual controls.
 */
API int uiFormPadded (uiForm *f);

/**
 * @brief Enables or disables the padding of a @p uiForm
 * @param f uiForm instance.
 * @param padded non-zero to enable padding, zero to disable padding
 * @remark padding is defined as space between individual controls, the size of which is determined by the system
 */
API void uiFormSetPadded (uiForm *f, int padded);

/**
 * @p uiForm constructor
 * @returns @p uiForm
 */
API uiForm *uiNewForm (void);
