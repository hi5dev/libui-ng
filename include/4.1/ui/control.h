#pragma once

#include "api.h"

#include <stdint.h>

/**
 * @brief Type-casts the given value to a @p uiControl
 * @param this pointer to cast
 */
#define uiControl(this) ((uiControl *)(this))

/**
 * @brief Base class for GUI controls providing common methods.
 */
typedef struct uiControl uiControl;

/**
 * @brief Placement specifier to define placement in relation to another control.
 */
typedef enum uiAt
{
  uiAtLeading,  //!< Place before control.
  uiAtTop,      //!< Place above control.
  uiAtTrailing, //!< Place behind control.
  uiAtBottom,   //!< Place below control.
} uiAt;

struct uiControl
{
  /**
   * @brief Internal LibUI signature.
   */
  uint32_t Signature;

  /**
   * @brief Operating-system signature.
   */
  uint32_t OSSignature;

  /**
   * @brief Type signature.
   */
  uint32_t TypeSignature;

  /**
   * @brief @p uiControl destructor
   * @param control @p uiControl
   */
  void (*Destroy) (uiControl *);

  /**
   * @brief Gets the handle of a @p uiControl
   * @param control @p uiControl
   * @return @p uintptr_t
   */
  uintptr_t (*Handle) (uiControl *);

  /**
   * @brief Gets the parent of a @p uiControl
   * @param control @p uiControl
   * @return Parent @p uiControl
   */
  uiControl *(*Parent) (uiControl *);

  /**
   * @brief Sets the parent of a @p uiControl
   * @param parent @p uiControl
   * @param child @p uiControl
   */
  void (*SetParent) (uiControl *, uiControl *);

  /**
   * @brief Checks if a @p uiControl is top-level
   * @param control @p uiControl
   * @return true or false
   */
  int (*Toplevel) (uiControl *);

  /**
   * @brief Checks if a @p uiControl is visible.
   * @param control @p uiControl
   * @return true or false
   */
  int (*Visible) (uiControl *);

  /**
   * @brief Shows a @p uiControl
   * @param control @p uiControl
   */
  void (*Show) (uiControl *);

  /**
   * @brief Hides a @p uiControl
   * @param control @p uiControl
   */
  void (*Hide) (uiControl *);

  /**
   * @brief Checks if a @p uiControl is enabled
   * @param control @p uiControl
   * @return true or false
   */
  int (*Enabled) (uiControl *);

  /**
   * @brief Enables the @p uiControl
   * @param control @p uiControl
   */
  void (*Enable) (uiControl *);

  /**
   * @brief Disables the @p uiControl
   * @param control @p uiControl
   */
  void (*Disable) (uiControl *);
};

/**
 * @brief Dispose and free all allocated resources.
 *
 * The platform specific APIs that actually destroy a control (and its children) are called.
 *
 * @remark Most of the time is needed to be used directly only on the top level windows.
 *
 * @param c @p uiControl instance.
 */
API void uiControlDestroy (uiControl *c);

/**
 * @brief Returns the control's OS-level handle.
 * @param c @p uiControl instance.
 * @returns OS-level handle.
 */
API uintptr_t uiControlHandle (uiControl *c);

/**
 * @brief Gets the parent of a @p uiControl
 * @param c @p uiControl instance.
 * @returns The parent @p uiControl
 * @returns @p NULL if detached
 */
API uiControl *uiControlParent (uiControl *c);

/**
 * @brief Sets the parent of a @p uiControl
 * @param c @p uiControl instance.
 * @param parent The parent @p uiControl, or @p NULL to detach
 */
API void uiControlSetParent (uiControl *c, uiControl *parent);

/**
 * @brief Checks if a @p uiControl is a top-level.
 * @param c @p uiControl instance.
 * @returns Non-zero for top-level controls.
 */
API int uiControlToplevel (uiControl *c);

/**
 * @brief Checks if a @p uiControl is visible.
 * @param c uiControl instance.
 * @returns `TRUE` if visible, `FALSE` otherwise.
 */
API int uiControlVisible (uiControl *c);

/**
 * @brief Shows a @p uiControl
 * @param c @p uiControl instance.
 */
API void uiControlShow (uiControl *c);

/**
 * @brief Hides a @p uiControl
 * @param c @p uiControl instance.
 * @remark Hidden controls do not take up space within the layout.
 */
API void uiControlHide (uiControl *c);

/**
 * @brief Checks if a @p uiControl is enabled.
 * @param c @p uiControl instance.
 * @see uiControlEnabledToUser
 */
API int uiControlEnabled (uiControl *c);

/**
 * @brief Enables a @p uiControl
 * @param c @p uiControl instance.
 */
API void uiControlEnable (uiControl *c);

/**
 * @brief Disables a @p uiControl
 * @param c @p uiControl instance.
 */
API void uiControlDisable (uiControl *c);

/**
 * @brief Allocates memory for a @p uiControl
 * @param n Size of type to allocate.
 * @param OSsig
 * @param typesig
 * @param typenamestr
 */
API uiControl *uiAllocControl (size_t n, uint32_t OSsig, uint32_t typesig, const char *typenamestr);

/**
 * @brief Frees memory previously allocated for a @p uiControl
 * @remark This method is public only for writing custom controls.
 * @param c @p uiControl instance.
 */
API void uiFreeControl (uiControl *c);

/**
 * @brief Ensures the parent of a @p uiControl can be set to the given value.
 * @param c @p uiControl instance.
 * @param parent @p uiControl instance.
 * @remark This will crash the application when false.
 */
API void uiControlVerifySetParent (uiControl *c, uiControl *parent);

/**
 * @brief Checks if a @p uiControl is interactive.
 * @remark A control is interactive when all its parents are enabled.
 * @param c @p uiControl instance.
 * @returns Non-zero when interactive.
 * @see uiControlEnabled
 */
API int uiControlEnabledToUser (uiControl *c);
