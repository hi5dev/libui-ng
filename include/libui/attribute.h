#pragma once

#include "api.h"
#include "ui.h"

/**
 * @brief Stores information about an attribute in a @p uiAttributedString.
 *
 * - @p uiAttributes are created using the specialized constructor functions. For every Unicode codepoint in the
 * - @p uiAttributedString, at most one value of each attribute type can be applied.
 * - @p uiAttributes are immutable and the @p uiAttributedString takes ownership of the @p uiAttribute object once
 * assigned, copying its contents as necessary.
 */
typedef struct uiAttribute uiAttribute;

/**
 * @brief Represents possible text weights.
 *
 * These roughly map to the OS/2 text weight field of TrueType and OpenType fonts, or to CSS weight numbers. The
 * named constants are nominal values; the actual values may vary by font and by OS, though this isn't particularly
 * likely. Any value between uiTextWeightMinimum and uiTextWeightMaximum, inclusive, is allowed.
 *
 * Note that due to restrictions in early versions of Windows, some fonts have "special" weights be exposed in many
 * programs as separate font families. This is perhaps most notable with Arial Black. libui does not do this, even on
 * Windows (because the DirectWrite API libui uses on Windows does not do this); to specify Arial Black, use family
 * Arial and weight uiTextWeightBlack.
 */
typedef enum uiTextWeight
{
  uiTextWeightMinimum    = 0,
  uiTextWeightThin       = 100,
  uiTextWeightUltraLight = 200,
  uiTextWeightLight      = 300,
  uiTextWeightBook       = 350,
  uiTextWeightNormal     = 400,
  uiTextWeightMedium     = 500,
  uiTextWeightSemiBold   = 600,
  uiTextWeightBold       = 700,
  uiTextWeightUltraBold  = 800,
  uiTextWeightHeavy      = 900,
  uiTextWeightUltraHeavy = 950,
  uiTextWeightMaximum    = 1000,
} uiTextWeight;

/**
 * @brief holds the possible uiAttribute types that may be returned by @p uiAttributeGetType.
 *
 * Refer to the documentation for each type's constructor function for details on each type.
 */
typedef enum uiAttributeType
{
  uiAttributeTypeFamily,         //!< font-family
  uiAttributeTypeSize,           //!< font-size
  uiAttributeTypeWeight,         //!< font-weight
  uiAttributeTypeItalic,         //!< font-italics
  uiAttributeTypeStretch,        //!< font-stretch-to-fit
  uiAttributeTypeColor,          //!< text foreground color
  uiAttributeTypeBackground,     //!< text background color
  uiAttributeTypeUnderline,      //!< text underlining
  uiAttributeTypeUnderlineColor, //!< text underlining color
  uiAttributeTypeFeatures,       //!< OpenFont features
} uiAttributeType;

/**
 * @brief Represents possible italic modes for a font.
 *
 * Italic represents "true" italics where the slanted glyphs have custom shapes, whereas oblique represents italics
 * that are merely slanted versions of the normal glyphs. Most fonts usually have one or the other.
 */
typedef enum uiTextItalic
{
  uiTextItalicNormal,
  uiTextItalicOblique,
  uiTextItalicItalic,
} uiTextItalic;

struct uiAttribute
{
  /**
   * @brief Non-zero when owned by the user
   */
  int ownedByUser;

  /**
   * @brief Reference counting.
   */
  size_t refcount;

  /**
   * @brief The attribute's type.
   */
  uiAttributeType type;

  /**
   * @brief The attribute's value.
   */
  union
  {
    /// @brief Font-family
    char *family;

    /// @brief Font size
    double size;

    /// @brief Font weight
    uiTextWeight weight;

    /// @brief Font italics
    uiTextItalic italic;

    /// @brief Stretch-to-fit
    uiTextStretch stretch;

    struct
    {
      /// @brief Red
      double r;

      /// @breif Green
      double g;

      /// @breif Blue
      double b;

      /// @breif Alpha
      double a;

      /// @breif Color of the line under text
      uiUnderlineColor underlineColor;
    } color;

    /// @brief Font underlining
    uiUnderline underline;

    /// @brief OpenType font features
    uiOpenTypeFeatures *features;
  } u;
};

/**
 * @brief Gets font family of a @p uiAttribute.
 * @remark The returned string is owned by @p a.
 * @remark It is an error to call this on a @p uiAttribute that does not hold a font family.
 */
API const char *uiAttributeFamily (const uiAttribute *a);

API const uiOpenTypeFeatures *uiAttributeFeatures (const uiAttribute *a);

/**
 * @brief Gets the font size stored in @p a.
 * @remark It is an error to call this on a @p uiAttribute that does not hold a font size.
 */
API double uiAttributeSize (const uiAttribute *a);

API int uiprivAttributeEqual (const uiAttribute *a, const uiAttribute *b);

API uiAttribute *uiNewBackgroundAttribute (double r, double g, double b, double a);

API uiAttribute *uiNewColorAttribute (double r, double g, double b, double a);

/**
 * @brief creates a new @p uiAttribute that changes the font family of the text it is applied to.
 * @remark @p family is copied; you do not need to keep it alive after @p uiNewFamilyAttribute returns.
 * @remark Font family names are case-insensitive.
 */
API uiAttribute *uiNewFamilyAttribute (const char *family);

API uiAttribute *uiNewFeaturesAttribute (const uiOpenTypeFeatures *otf);

/**
 * @brief creates a new uiAttribute that changes the italic mode of the text it is applied to.
 * @remark It is an error to specify an italic mode not specified in @p uiTextItalic.
 */
API uiAttribute *uiNewItalicAttribute (uiTextItalic italic);

/**
 * @brief creates a new @p uiAttribute that changes the size of the text it is applied to, in typographical points.
 */
API uiAttribute *uiNewSizeAttribute (double size);

API uiAttribute *uiNewStretchAttribute (uiTextStretch stretch);

API uiAttribute *uiNewUnderlineAttribute (uiUnderline u);

API uiAttribute *uiNewUnderlineColorAttribute (uiUnderlineColor u, double r, double g, double b, double a);

/**
 * @brief Creates a new uiAttribute that changes the weight of the text it is applied to.
 * @remark It is an error to specify a weight outside the range
 * @code [uiTextWeightMinimum, uiTextWeightMaximum]@endcode
 */
API uiAttribute *uiNewWeightAttribute (uiTextWeight weight);

API uiAttribute *uiprivAttributeRetain (uiAttribute *a);

/**
 * @brief Gets an attribute's type
 */
API uiAttributeType uiAttributeGetType (const uiAttribute *a);

API uiTextItalic uiAttributeItalic (const uiAttribute *a);

API uiTextStretch uiAttributeStretch (const uiAttribute *a);

/**
 * @brief Gets the font weight stored in @p a.
 * @remark It is an error to call this on a @p uiAttribute that does not hold a font weight.
 */
API uiTextWeight uiAttributeWeight (const uiAttribute *a);

API uiUnderline uiAttributeUnderline (const uiAttribute *a);

API void uiAttributeColor (const uiAttribute *a, double *r, double *g, double *b, double *alpha);

API void uiAttributeUnderlineColor (const uiAttribute *a, uiUnderlineColor *u, double *r, double *g, double *b,
                                    double *alpha);

/**
 * @brief @p uiAttribute destructor
 * @remarks Typically called automatically by @p uiAttributedString
 */
API void uiFreeAttribute (uiAttribute *a);

API void uiprivAttributeRelease (uiAttribute *a);
