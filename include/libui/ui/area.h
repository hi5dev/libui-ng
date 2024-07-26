#pragma once

#include "api.h"
#include "draw.h"
#include "window.h"

/**
 * @brief Type-casts a pointer to a @p uiArea
 * @param this pointer to cast
 */
#define uiArea(this) ((uiArea *)(this))

/**
 * @brief Represents a drawable area of a @p uiWindow
 */
typedef struct uiArea uiArea;

/**
 * @brief Area callbacks.
 */
typedef struct uiAreaHandler uiAreaHandler;

/**
 * @brief Drawing parameters.
 */
typedef struct uiAreaDrawParams uiAreaDrawParams;

/**
 * @brief Mouse events.
 */
typedef struct uiAreaMouseEvent uiAreaMouseEvent;

/**
 * @brief Keyboard events.
 */
typedef struct uiAreaKeyEvent uiAreaKeyEvent;

/**
 * Keyboard modifier keys.
 *
 * Usable as bitmasks.
 */
typedef enum uiModifiers
{
  uiModifierCtrl  = 1 << 0, //!< Control key.
  uiModifierAlt   = 1 << 1, //!< Alternate/Option key.
  uiModifierShift = 1 << 2, //!< Shift key.
  uiModifierSuper = 1 << 3, //!< Super/Command/Windows key.
} uiModifiers;

/**
 * @brief Extended keys
 */
typedef enum uiExtKey
{
  uiExtKeyEscape = 1, //!< escape
  uiExtKeyInsert,     //!< equivalent to "Help" on Apple keyboards
  uiExtKeyDelete,     //!< delete
  uiExtKeyHome,       //!< home
  uiExtKeyEnd,        //!< end
  uiExtKeyPageUp,     //!< page-up
  uiExtKeyPageDown,   //!< page-down
  uiExtKeyUp,         //!< up arrow
  uiExtKeyDown,       //!< down arrow
  uiExtKeyLeft,       //!< left arrow
  uiExtKeyRight,      //!< right arrow
  uiExtKeyF1,         //!< F1
  uiExtKeyF2,         //!< F2
  uiExtKeyF3,         //!< F3
  uiExtKeyF4,         //!< F4
  uiExtKeyF5,         //!< F5
  uiExtKeyF6,         //!< F6
  uiExtKeyF7,         //!< F7
  uiExtKeyF8,         //!< F8
  uiExtKeyF9,         //!< F9
  uiExtKeyF10,        //!< F10
  uiExtKeyF11,        //!< F11
  uiExtKeyF12,        //!< F12
  uiExtKeyN0,         //!< numpad 0
  uiExtKeyN1,         //!< numpad 1
  uiExtKeyN2,         //!< numpad 2
  uiExtKeyN3,         //!< numpad 3
  uiExtKeyN4,         //!< numpad 4
  uiExtKeyN5,         //!< numpad 5
  uiExtKeyN6,         //!< numpad 6
  uiExtKeyN7,         //!< numpad 7
  uiExtKeyN8,         //!< numpad 8
  uiExtKeyN9,         //!< numpad 9
  uiExtKeyNDot,       //!< numpad period
  uiExtKeyNEnter,     //!< numpad enter key
  uiExtKeyNAdd,       //!< numpad plus
  uiExtKeyNSubtract,  //!< numpad minus
  uiExtKeyNMultiply,  //!< numpad asterisk
  uiExtKeyNDivide,    //!< numpad forward-slash
} uiExtKey;

struct uiAreaKeyEvent
{
  char        Key;       //!< key
  uiExtKey    ExtKey;    //!< @p uiExtKey
  uiModifiers Modifier;  //!< @p uiModifiers
  uiModifiers Modifiers; //!< @p uiModifiers
  int         Up;        //!< non-zero when up
};

struct uiAreaMouseEvent
{
  double      X;          //!< position
  double      Y;          //!< position
  double      AreaWidth;  //!< size
  double      AreaHeight; //!< size
  int         Down;       //!< non-zero when the button is held down
  int         Up;         //!< non-zero when the button is released
  int         Count;      //!<
  uiModifiers Modifiers;  //!< @p uiModifiers
  uint64_t    Held1To64;  //!<
};

struct uiAreaDrawParams
{
  /**
   * @brief @p uiDrawContext
   */
  uiDrawContext *Context;

  /**
   * @brief Non-scrollable width of the drawing.
   */
  double AreaWidth;

  /**
   * @brief Non-scrollable height of the drawing.
   */
  double AreaHeight;

  /**
   * @brief Left-most start position.
   */
  double ClipX;

  /**
   * @brief Top-most start position.
   */
  double ClipY;

  /**
   * @brief Width of the clipped area.
   */
  double ClipWidth;

  /**
   * @brief Height of the clipped area.
   */
  double ClipHeight;
};

struct uiAreaHandler
{
  /**
   * @brief Callback for draw events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   * @param params @p uiAreaDrawParams
   */
  void (*Draw) (uiAreaHandler *handler, uiArea *area, uiAreaDrawParams *params);

  /**
   * @brief Callback for mouse events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   * @param event @p uiAreaMouseEvent
   */
  void (*MouseEvent) (uiAreaHandler *handler, uiArea *area, uiAreaMouseEvent *event);

  /**
   * @brief Callback for mouse drag-over events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   * @param left
   */
  void (*MouseCrossed) (uiAreaHandler *handler, uiArea *area, int left);

  /**
   * @brief Callback for mouse drag-out events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   */
  void (*DragBroken) (uiAreaHandler *handler, uiArea *area);

  /**
   * @brief Callback for keyboard events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   * @param event @p uiAreaKeyEvent
   */
  int (*KeyEvent) (uiAreaHandler *handler, uiArea *area, uiAreaKeyEvent *event);
};

/**
 * @brief Sets the size of a @p uiArea
 * @param a @p uiArea
 * @param width
 * @param height
 */
API void uiAreaSetSize (uiArea *a, int width, int height);

/**
 * @brief Queues a @p uiArea for redrawing
 * @param a @p uiArea
 */
API void uiAreaQueueRedrawAll (uiArea *a);

/**
 * @brief Scrolls a @p uiArea to the given bounds
 * @param a @p uiArea
 * @param x position
 * @param y position
 * @param width size
 * @param height size
 */
API void uiAreaScrollTo (uiArea *a, double x, double y, double width, double height);

/**
 * @brief Signals that the user is moving the window of a @p uiArea
 * @param a @p uiArea
 */
API void uiAreaBeginUserWindowMove (uiArea *a);

/**
 * @brief Signals that the user is resizing the window of a @p uiArea
 * @param a @p uiArea
 * @param edge @p uiWindowResizeEdge
 */
API void uiAreaBeginUserWindowResize (uiArea *a, uiWindowResizeEdge edge);

/**
 * @brief @p uiArea constructor
 * @param ah @p uiAreaHandler
 * @return @p uiArea
 */
API uiArea *uiNewArea (uiAreaHandler *ah);

/**
 * @brief Constructs a scrollable @p uiArea
 * @param ah @p uiAreaHandler
 * @param width
 * @param height
 * @return @p uiArea
 */
API uiArea *uiNewScrollingArea (uiAreaHandler *ah, int width, int height);
