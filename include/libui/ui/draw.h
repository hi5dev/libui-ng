#pragma once

#include "api.h"
#include "attributed_string.h"
#include "font_descriptor.h"

/**
 * @brief The default for both Cairo and Direct2D (in the latter case, from the C++ helper functions).
 *
 * Core Graphics doesn't explicitly specify a default, but NSBezierPath allows you to choose one, and this is the
 * initial value so we're good to use it too.
 */
#define uiDrawDefaultMiterLimit 10.0

/**
 * @brief Drawing context - i.e. a canvas
 */
typedef struct uiDrawContext uiDrawContext;

/**
 * @brief Attributes for path-style drawing
 */
typedef struct uiDrawPath uiDrawPath;

/**
 * @brief Attributes for brush-style drawing
 */
typedef struct uiDrawBrush uiDrawBrush;

/**
 * @brief Attributes for stroke-style drawing
 */
typedef struct uiDrawStrokeParams uiDrawStrokeParams;

/**
 * @brief Attributes for matrix-style drawing
 */
typedef struct uiDrawMatrix uiDrawMatrix;

/**
 * @brief Attributes for gradient brush-style drawing
 */
typedef struct uiDrawBrushGradientStop uiDrawBrushGradientStop;

/**
 * @brief A concrete representation of a @p uiAttributedString that can be displayed in a uiDrawContext.
 *
 * This includes information important for the drawing of a block of text, including the bounding box to wrap the text
 * within, the alignment of lines of text within that box, areas to mark as being selected, and other things.
 *
 * @remark Unlike @p uiAttributedString, the content of a @p uiDrawTextLayout is immutable once it has been created.
 */
typedef struct uiDrawTextLayout uiDrawTextLayout;

/**
 * @brief Describes a @p uiDrawTextLayout
 */
typedef struct uiDrawTextLayoutParams uiDrawTextLayoutParams;

/**
 * @brief Brush types.
 */
typedef enum uiDrawBrushType
{
  uiDrawBrushTypeSolid,          //!< Solid brush.
  uiDrawBrushTypeLinearGradient, //!< Progressive transition between two or more colors along a straight line.
  uiDrawBrushTypeRadialGradient, //!< Progressive transition between two or more colors that radiate from an origin.
  uiDrawBrushTypeImage,          //!< Image brush.
} uiDrawBrushType;

/**
 * @brief Fill modes.
 */
typedef enum uiDrawFillMode
{
  uiDrawFillModeWinding,   //!< Fills any region with a non-zero winding value.
  uiDrawFillModeAlternate, //!< Fills by a straight line from its ending point to its starting point.
} uiDrawFillMode;

/**
 * @brief Line cap styles.
 */
typedef enum uiDrawLineCap
{
  uiDrawLineCapFlat,   //!<
  uiDrawLineCapRound,  //!<
  uiDrawLineCapSquare, //!<
} uiDrawLineCap;

/**
 * @brief Line join styles.
 */
typedef enum uiDrawLineJoin
{
  uiDrawLineJoinMiter, //!<
  uiDrawLineJoinRound, //!<
  uiDrawLineJoinBevel, //!<
} uiDrawLineJoin;

/**
 * @brief Specifies the alignment of lines of text in a @p uiDrawTextLayout.
 */
typedef enum uiDrawTextAlign
{
  uiDrawTextAlignLeft,   //!< left-align
  uiDrawTextAlignCenter, //!< center-align
  uiDrawTextAlignRight,  //!< right-align
} uiDrawTextAlign;

struct uiDrawMatrix
{
  double M11; //!<
  double M12; //!<
  double M21; //!<
  double M22; //!<
  double M31; //!<
  double M32; //!<
};

struct uiDrawBrush
{
  uiDrawBrushType Type;

  // solid brushes
  double R; //!< red channel
  double G; //!< green channel
  double B; //!< blue channel
  double A; //!< alpha channel

  // gradient brushes
  double                   X0;          //!< linear: start X, radial: start X
  double                   Y0;          //!< linear: start Y, radial: start Y
  double                   X1;          //!< linear: end X, radial: outer circle center X
  double                   Y1;          //!< linear: end Y, radial: outer circle center Y
  double                   OuterRadius; //!< radial gradients only
  uiDrawBrushGradientStop *Stops;       //!< @p uiDrawBrushGradientStop
  size_t                   NumStops;    //!< number of stops
};

struct uiDrawBrushGradientStop
{
  double Pos; //!< position
  double R;   //!< red channel
  double G;   //!< green channel
  double B;   //!< blue channel
  double A;   //!< alpha channel
};

struct uiDrawStrokeParams
{
  uiDrawLineCap  Cap;        //!< @p uiDrawLineCap
  uiDrawLineJoin Join;       //!< @p uiDrawLineJoin
  double         Thickness;  //!< line thickness
  double         MiterLimit; //!< miter limit
  double        *Dashes;     //!< dashes
  size_t         NumDashes;  //!< number of dashes
  double         DashPhase;  //!< dash phase
};

struct uiDrawTextLayoutParams
{
  uiAttributedString *String;      //!< string to draw
  uiFontDescriptor   *DefaultFont; //!< used to render any text that is not attributed sufficiently in @p String.
  double              Width;       //!< width of the bounding box of the text (height is automatic)
  uiDrawTextAlign     Align;       //!< text-alignment
};

/**
 * @brief @p uiDrawPath constructor
 * @param fillMode
 * @return @p uiDrawPath
 */
API uiDrawPath *uiDrawNewPath (uiDrawFillMode fillMode);

/**
 * @brief @p uiDrawPath destructor
 * @param p @p uiDrawPath
 */
API void uiDrawFreePath (uiDrawPath *p);

/**
 * @brief Creates a new figure for a @p uiDrawPath
 * @param p @p uiDrawPath
 * @param x position
 * @param y position
 */
API void uiDrawPathNewFigure (uiDrawPath *p, double x, double y);

/**
 * @brief Starts a new arc figure
 * @param p @p uiDrawPath
 * @param xCenter position
 * @param yCenter position
 * @param radius of the arc
 * @param startAngle in degrees
 * @param sweep value of the arc
 * @param negative non-zero for true
 */
API void uiDrawPathNewFigureWithArc (uiDrawPath *p, double xCenter, double yCenter, double radius, double startAngle,
                                     double sweep, int negative);

/**
 * @brief Draws a line from the current to given position
 * @param p @p uiDrawPath
 * @param x position
 * @param y position
 */
API void uiDrawPathLineTo (uiDrawPath *p, double x, double y);

/**
 * @brief Draws an arc from the current to given position
 * @param p @p uiDrawPath
 * @param xCenter position
 * @param yCenter position
 * @param radius of the arc
 * @param startAngle in degrees
 * @param sweep of the arc
 * @param negative non-zero for true
 */
API void uiDrawPathArcTo (uiDrawPath *p, double xCenter, double yCenter, double radius, double startAngle,
                          double sweep, int negative);

/**
 * @brief Draws a bezier path from the current to given coordinates
 * @param p @p uiDrawPath
 * @param c1x
 * @param c1y
 * @param c2x
 * @param c2y
 * @param endX
 * @param endY
 */
API void uiDrawPathBezierTo (uiDrawPath *p, double c1x, double c1y, double c2x, double c2y, double endX, double endY);

/**
 * @brief Closes the current figure
 * @param p @p uiDrawPath
 */
API void uiDrawPathCloseFigure (uiDrawPath *p);

/**
 * @brief Drraws a rectangle at the given coordinates
 * @param p @p uiDrawPath
 * @param x position
 * @param y position
 * @param width of the rectangle
 * @param height of the rectangle
 */
API void uiDrawPathAddRectangle (uiDrawPath *p, double x, double y, double width, double height);

/**
 * @brief Checks if the path has ended
 * @param p @p uiDrawPath
 * @return non-zero when true
 */
API int uiDrawPathEnded (uiDrawPath *p);

/**
 * @brief Ends a @p uiDrawPath
 * @param p @p uiDrawPath
 */
API void uiDrawPathEnd (uiDrawPath *p);

/**
 * @brief Draws a brush stroke
 * @param c @p uiDrawContext
 * @param path @p uiDrawPath
 * @param b @p uiDrawBrush
 * @param p @p uiDrawStrokeParams
 */
API void uiDrawStroke (uiDrawContext *c, uiDrawPath *path, uiDrawBrush *b, uiDrawStrokeParams *p);

/**
 * @brief Draws a filled in path
 * @param c @p uiDrawContext
 * @param path @p uiDrawPath
 * @param b @p uiDrawBrush
 */
API void uiDrawFill (uiDrawContext *c, uiDrawPath *path, uiDrawBrush *b);

/**
 * @brief Sets the identity of a @p uiDrawMatrix
 * @param m @p uiDrawMatrix
 */
API void uiDrawMatrixSetIdentity (uiDrawMatrix *m);

/**
 * @brief Matrix translation
 * @param m @p uiDrawMatrix
 * @param x position
 * @param y position
 */
API void uiDrawMatrixTranslate (uiDrawMatrix *m, double x, double y);

/**
 * @brief Matrix scaling
 * @param m @p uiDrawMatrix
 * @param xCenter
 * @param yCenter
 * @param x
 * @param y
 */
API void uiDrawMatrixScale (uiDrawMatrix *m, double xCenter, double yCenter, double x, double y);

/**
 * @brief Matrix rotation
 * @param m @p uiDrawMatrix
 * @param x
 * @param y
 * @param amount
 */
API void uiDrawMatrixRotate (uiDrawMatrix *m, double x, double y, double amount);

/**
 * @brief Matrix scewing
 * @param m @p uiDrawMatrix
 * @param x
 * @param y
 * @param xamount
 * @param yamount
 */
API void uiDrawMatrixSkew (uiDrawMatrix *m, double x, double y, double xamount, double yamount);

/**
 * @brief Matrix multiplication
 * @param dest @p uiDrawMatrix
 * @param src @p uiDrawMatrix
 */
API void uiDrawMatrixMultiply (uiDrawMatrix *dest, uiDrawMatrix *src);

/**
 * @brief Inverts a @p uiDrawMatrix
 * @param m @p uiDrawMatrix
 */
API int uiDrawMatrixInvertible (uiDrawMatrix *m);

/**
 * @brief Matrix inversion
 * @param m @p uiDrawMatrix
 */
API int uiDrawMatrixInvert (uiDrawMatrix *m);

/**
 * @brief Matrix point transformation
 * @param m @p uiDrawMatrix
 * @param x[out]
 * @param y[out]
 */
API void uiDrawMatrixTransformPoint (uiDrawMatrix *m, double *x, double *y);

/**
 * @brief Matrix size transformation
 * @param m @p uiDrawMatrix
 * @param x[out]
 * @param y[out]
 */
API void uiDrawMatrixTransformSize (uiDrawMatrix *m, double *x, double *y);

/**
 * @brief Matrix transformation for a @p uiDrawContext
 * @param c @p uiDrawContext
 * @param m @p uiDrawMatrix
 */
API void uiDrawTransform (uiDrawContext *c, uiDrawMatrix *m);

/**
 * @brief Clips a @p uiDrawPath
 * @param c @p uiDrawContext
 * @param path @p uiDrawPath
 */
API void uiDrawClip (uiDrawContext *c, uiDrawPath *path);

/**
 * @brief Saves a @p uiDrawContext
 * @param c @p uiDrawContext
 */
API void uiDrawSave (uiDrawContext *c);

/**
 * @brief Restores a @p uiDrawContext
 * @param c @p uiDrawContext
 */
API void uiDrawRestore (uiDrawContext *c);

/**
 * @brief @p uiDrawTextLayout constructor
 * @param params @p uiDrawTextLayoutParams
 * @return @p uiDrawTextLayout
 */
API uiDrawTextLayout *uiDrawNewTextLayout (uiDrawTextLayoutParams *params);

/**
 * @brief @p uiDrawFreeTextLayout destructor
 * @param tl @p uiDrawTextLayout
 * @remark The underlying @p uiAttributedString is not freed.
 */
API void uiDrawFreeTextLayout (uiDrawTextLayout *tl);

/**
 * @brief Draws @p tl in @p c with the top-left point of @p tl at @code (x, y)@endcode
 * @param c @p uiDrawContext
 * @param tl @p uiDrawTextLayout
 * @param x position
 * @param y position
 */
API void uiDrawText (uiDrawContext *c, uiDrawTextLayout *tl, double x, double y);

/**
 * @brief Gets the width and height of @p tl in @p width and @p height.
 * @param tl @p uiDrawTextLayout
 * @param[out] width size
 * @param[out] height size
 * @remark The returned width may be smaller than the width passed into @p uiDrawNewTextLayout, depending on how the
 * text in @p tl is wrapped. Therefore, you can use this function to get the actual size of the text layout.
 */
API void uiDrawTextLayoutExtents (uiDrawTextLayout *tl, double *width, double *height);
