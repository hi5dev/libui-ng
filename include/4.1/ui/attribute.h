#pragma once

#include "api.h"
#include "opentype.h"

#include <stddef.h>

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

/**
 * @brief Represents possible stretches (aka widths) of a font.
 * @remark Due to restrictions in early versions of Windows, some fonts have "special" stretches be exposed in many
 * programs as separate font families. This is perhaps most notable with Arial Condensed. libui does not do this, even
 * on Windows (because the DirectWrite API libui uses on Windows does not do this); to specify Arial Condensed, use
 * family Arial and stretch @p uiTextStretchCondensed.
 */
typedef enum uiTextStretch
{
  uiTextStretchUltraCondensed, //!<
  uiTextStretchExtraCondensed, //!<
  uiTextStretchCondensed,      //!<
  uiTextStretchSemiCondensed,  //!<
  uiTextStretchNormal,         //!<
  uiTextStretchSemiExpanded,   //!<
  uiTextStretchExpanded,       //!<
  uiTextStretchExtraExpanded,  //!<
  uiTextStretchUltraExpanded,  //!<
} uiTextStretch;

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
 * @brief Specifies a type of underline to use on text.
 */
typedef enum uiUnderline
{
  uiUnderlineNone,       //!< no underlining
  uiUnderlineSingle,     //!< single-line
  uiUnderlineDouble,     //!< double-line
  uiUnderlineSuggestion, //!< wavy or dotted underlines used for spelling/grammar checkers
} uiUnderline;

/**
 * @brief specifies the color of any underline on the text it is applied to, regardless of the type of underline
 *
 * In addition to being able to specify a custom color, you can explicitly specify platform-specific colors for
 * suggestion underlines; to use them correctly, pair them with @p uiUnderlineSuggestion (though they can be used on
 * other types of underline as well).
 *
 * If an underline type is applied but no underline color is specified, the text color is used instead. If an underline
 * color is specified without an underline type, the underline color attribute is ignored, but not removed from the
 * @p uiAttributedString.
 */
typedef enum uiUnderlineColor
{
  uiUnderlineColorCustom,    //!< custom color
  uiUnderlineColorSpelling,  //!< underline spelling errors
  uiUnderlineColorGrammar,   //!< underline gramatical errors
  uiUnderlineColorAuxiliary, //!< for instance, the color used by smart replacements on macOS or in Microsoft Office
} uiUnderlineColor;

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
 * @brief Gets the text color stored in a @p uiAttribute
 * @param[out] a @p uiAttribute
 * @param[out] r red value
 * @param[out] g green value
 * @param[out] b blue value
 * @param[out] alpha value
 * @remark it is an error to call this on a @p uiAttribute that does not hold a text color
 */
API void uiAttributeColor (const uiAttribute *a, double *r, double *g, double *b, double *alpha);

/**
 * @brief Gets font family of a @p uiAttribute.
 * @remark The returned string is owned by @p a.
 * @remark It is an error to call this on a @p uiAttribute that does not hold a font family.
 */
API const char *uiAttributeFamily (const uiAttribute *a);

/**
 * @brief Gets the OpenType features stored in a @p uiAttribute
 * @remark The returned @p uiOpenTypeFeatures object is owned by @p a.
 * @remark It is an error to call this on a uiAttribute that does not hold OpenType features.
 * @return @p uiOpenTypeFeatures
 */
API const uiOpenTypeFeatures *uiAttributeFeatures (const uiAttribute *a);

/**
 * @brief Gets an attribute's type
 * @param a @p uiAttribute
 * @return @p uiAttributeType
 */
API uiAttributeType uiAttributeGetType (const uiAttribute *a);

/**
 * @brief Gets the font italic mode stored in a @p uiAttribute
 * @param a @p uiAttribute
 * @return @p uiTextItalic
 * @remark it is an error to call this on a uiAttribute that does not hold a font italic mode.
 */
API uiTextItalic uiAttributeItalic (const uiAttribute *a);

/**
 * @brief Gets the font size stored in @p a.
 * @remark It is an error to call this on a @p uiAttribute that does not hold a font size.
 */
API double uiAttributeSize (const uiAttribute *a);

/**
 * @brief Gets the font stretch stored in a @p uiAttribute
 * @param a @p uiAttribute
 * @return @p uiTextStretch
 * @remark it is an error to call this on a @p uiAttribute that does not hold a font stretch
 */
API uiTextStretch uiAttributeStretch (const uiAttribute *a);

/**
 * @brief Gets the underline type stored in a @p uiAttribute
 * @param a @p uiAttribute
 * @return @p uiUnderline
 * @remark it is an error to call this on a @p uiAttribute that does not hold an underline style
 */
API uiUnderline uiAttributeUnderline (const uiAttribute *a);

/**
 * @brief Gets the underline color stored in an @p uiAttribute
 * @param a @p uiAttribute
 * @param[out] u @p uiUnderlineColor
 * @param[out] r red value
 * @param[out] g green value
 * @param[out] b blue value
 * @param[out] alpha value
 */
API void uiAttributeUnderlineColor (const uiAttribute *a, uiUnderlineColor *u, double *r, double *g, double *b,
                                    double *alpha);

/**
 * @brief Gets the font weight stored in @p a.
 * @remark It is an error to call this on a @p uiAttribute that does not hold a font weight.
 */
API uiTextWeight uiAttributeWeight (const uiAttribute *a);

/**
 * @brief @p uiAttribute destructor
 * @remarks Typically called automatically by @p uiAttributedString
 */
API void uiFreeAttribute (uiAttribute *a);

/**
 * @brief Creates a new @p uiAttribute that changes the background color of the text it is applied to
 * @param r red value
 * @param g green value
 * @param b blue value
 * @param a alpha value
 * @remark It is an error to specify an invalid color.
 * @return @p uiAttribute
 */
API uiAttribute *uiNewBackgroundAttribute (double r, double g, double b, double a);

/**
 * @brief Creates a new @p uiAttribute that changes the foreground color of the text it is applied to
 * @param r red value
 * @param g green value
 * @param b blue value
 * @param a alpha value
 * @remark It is an error to specify an invalid color.
 * @return @p uiAttribute
 */
API uiAttribute *uiNewColorAttribute (double r, double g, double b, double a);

/**
 * @brief creates a new @p uiAttribute that changes the font family of the text it is applied to
 * @remark @p family is copied; you do not need to keep it alive after @p uiNewFamilyAttribute returns.
 * @remark Font family names are case-insensitive.
 */
API uiAttribute *uiNewFamilyAttribute (const char *family);

/**
 * Creates a new @p uiAttribute that changes the font family of the text it is applied to
 * @remark @p otf is copied; you may free it after @p uiNewFeaturesAttribute returns.
 * @return @p uiAttribute
 */
API uiAttribute *uiNewFeaturesAttribute (const uiOpenTypeFeatures *otf);

/**
 * @brief creates a new uiAttribute that changes the italic mode of the text it is applied to
 * @remark It is an error to specify an italic mode not specified in @p uiTextItalic.
 */
API uiAttribute *uiNewItalicAttribute (uiTextItalic italic);

/**
 * @brief creates a new @p uiAttribute that changes the size of the text it is applied to, in typographical points
 */
API uiAttribute *uiNewSizeAttribute (double size);

/**
 * @brief Creates a new @p uiAttribute that changes the stretch of the text it is applied to
 * @remark it is an error to specify a strech not specified in @p uiTextStretch
 * @return @p uiAttribute
 */
API uiAttribute *uiNewStretchAttribute (uiTextStretch stretch);

/**
 * @brief Creates a new @p uiAttribute that changes the type of underline on the text it is applied to
 * @remark it is an error to specify an underline type not specified in @p uiUnderline.
 * @return @p uiAttribute
 */
API uiAttribute *uiNewUnderlineAttribute (uiUnderline u);

/**
 * @brief creates a new uiAttribute that changes the color of the underline on the text it is applied to
 * @param u @p uiUnderlineColor
 * @param r red value
 * @param g green value
 * @param b blue value
 * @param a alpha value
 * @return @p uiAttribute
 * @remark it is an error to specify an underline color not specified in @p uiUnderlineColor
 * @remark if the specified color type is @p uiUnderlineColorCustom, it is an error to specify an invalid color value,
 * otherwise the color values are ignored and should be specified as zero.
 */
API uiAttribute *uiNewUnderlineColorAttribute (uiUnderlineColor u, double r, double g, double b, double a);

/**
 * @brief Creates a new uiAttribute that changes the weight of the text it is applied to.
 * @param weight @p uiTextWeight
 * @remark It is an error to specify a weight outside the range
 * @code [uiTextWeightMinimum, uiTextWeightMaximum]@endcode
 */
API uiAttribute *uiNewWeightAttribute (uiTextWeight weight);
