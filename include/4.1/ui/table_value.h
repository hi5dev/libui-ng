#pragma once

#include "api.h"
#include "image.h"

/**
 * @brief Container to store values used in container related methods.
 * @remark @p uiTableValue objects are immutable.
 * @remark @p uiTable and @p uiTableModel methods take ownership of the @p uiTableValue objects when passed as
 * parameter. Exception: @p uiNewTableValueImage.
 * @remark @p uiTable and @p uiTableModel methods retain ownership when returning @p uiTableValue objects. Exception:
 * @p uiTableValueImage.
 */
typedef struct uiTableValue uiTableValue;

/**
 * @brief @p uiTableValue types
 */
typedef enum uiTableValueType
{
  uiTableValueTypeString, //!< string
  uiTableValueTypeImage,  //!< image
  uiTableValueTypeInt,    //!< integer
  uiTableValueTypeColor,  //!< color
} uiTableValueType;

/**
 * @brief @p uiTableValue destructor
 * @param v Table value to free.
 * @remark This function is to be used only on @p uiTableValue objects that have NOT been passed to @p uiTable or
 * @p uiTableModel - as these take ownership of the object.
 * @remark Use this for freeing erroneously created values or when directly calling @p uiTableModelHandler without
 * transferring ownership to @p uiTable or @p uiTableModel.
 */
API void uiFreeTableValue (uiTableValue *v);

/**
 * @brief Gets the @p uiTableValue type
 * @param v @p uiTableValue
 * @return @p uiTableValueType
 */
API uiTableValueType uiTableValueGetType (const uiTableValue *v);

/**
 * @brief Constructs a @p uiTableValue for string storage
 * @param str string
 * @remark @p str is copied internally; ownership is not transferred
 * @return @p uiTableValue
 */
API uiTableValue *uiNewTableValueString (const char *str);

/**
 * @brief Gets the string value held internally.
 * @param v Table value.
 * @return string
 * @remark data remains owned by @p v, do not call @p uiFreeText
 * @remark To be used only on @p uiTableValue objects of type @p uiTableValueTypeString.
 */
API const char *uiTableValueString (const uiTableValue *v);

/**
 * @brief Constructs a new @p uiTableValue for image storage
 *
 * Unlike other @p uiTableValue constructors, @p uiNewTableValueImage does not copy the image to save time and space.
 * Make sure the image data stays valid while in use by the library.
 *
 * As a general rule: if the constructor is called via the @p uiTableModelHandler, the image is safe to free once
 * execution returnsto any of your code.
 *
 * @param img @p uiImage
 * @return @p uiTableValue
 * @remark @p img is not copied and needs to kept alive
 */
API uiTableValue *uiNewTableValueImage (uiImage *img);

/**
 * @brief Gets a reference to the image contained in a @p uiTableValue of type @p uiTableValueTypeImage
 * @param v @p uiTableValue
 * @return @p uiImage
 * @remark data is owned by the caller
 */
API uiImage *uiTableValueImage (const uiTableValue *v);

/**
 * @brief Constructs a @p uiTableValue for integer storage
 *
 * This value type can be used in conjunction with properties like column editable boolean or controls, like progress
 * bars and checkboxes. For these, consult @p uiProgressBar and @p uiCheckbox for the allowed integer ranges.
 *
 * @param i value
 * @return @p uiTableValue
 */
API uiTableValue *uiNewTableValueInt (int i);

/**
 * @brief Gets the integer value held internally for a @p uiTableValueTypeInt type @p uiTablevalue
 * @param v @p uiTableValue
 * @return value
 */
API int uiTableValueInt (const uiTableValue *v);

/**
 * @brief Constructs a new @p uiTableValue for color storage
 * @param r Red. Double in range of [0, 1.0].
 * @param g Green. Double in range of [0, 1.0].
 * @param b Blue. Double in range of [0, 1.0].
 * @param a Alpha. Double in range of [0, 1.0].
 * @return @p uiTableValue
 */
API uiTableValue *uiNewTableValueColor (double r, double g, double b, double a);

/**
 * @brief Gets the color value held internally in a @p uiTableValueTypeColor type @p uiTableValue
 * @param v @p uiTableValue
 * @param[out] r Red. Double in range of [0, 1.0].
 * @param[out] g Green. Double in range of [0, 1.0].
 * @param[out] b Blue. Double in range of [0, 1.0].
 * @param[out] a Alpha. Double in range of [0, 1.0].
 */
API void uiTableValueColor (const uiTableValue *v, double *r, double *g, double *b, double *a);
