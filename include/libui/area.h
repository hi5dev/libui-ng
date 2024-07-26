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
  void (*Draw) (uiAreaHandler *, uiArea *, uiAreaDrawParams *);

  /**
   * @brief Callback for mouse events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   * @param event @p uiAreaMouseEvent
   */
  void (*MouseEvent) (uiAreaHandler *, uiArea *, uiAreaMouseEvent *);

  /**
   * @brief Callback for mouse drag-over events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   * @param left
   */
  void (*MouseCrossed) (uiAreaHandler *, uiArea *, int);

  /**
   * @brief Callback for mouse drag-out events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   */
  void (*DragBroken) (uiAreaHandler *, uiArea *);

  /**
   * @brief Callback for keyboard events.
   * @param handler @p uiAreaHandler
   * @param area @p uiArea
   * @param event @p uiAreaKeyEvent
   */
  int (*KeyEvent) (uiAreaHandler *, uiArea *, uiAreaKeyEvent *);
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
 * @param x
 * @param y
 * @param width
 * @param height
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
