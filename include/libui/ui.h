#pragma once

#include "alignment.h"

#include "api.h"
#include "area.h"
#include "box.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "control.h"
#include "date_time_picker.h"
#include "dialogs.h"
#include "draw.h"
#include "editable_combobox.h"
#include "entry.h"
#include "form.h"
#include "grid.h"
#include "group.h"
#include "image.h"
#include "label.h"
#include "menu.h"
#include "menu_item.h"
#include "multiline_entry.h"
#include "progressbar.h"
#include "radio_buttons.h"
#include "separator.h"
#include "slider.h"
#include "spinbox.h"
#include "tab.h"
#include "table.h"
#include "window.h"

#include <stdint.h>

// uiAttributeItalic() returns the font italic mode stored in a. It is an
// error to call this on a uiAttribute that does not hold a font italic
// mode.
API uiTextItalic uiAttributeItalic (const uiAttribute *a);

// uiTextStretch represents possible stretches (also called "widths")
// of a font.
//
// Note that due to restrictions in early versions of Windows, some
// fonts have "special" stretches be exposed in many programs as
// separate font families. This is perhaps most notable with
// Arial Condensed. libui does not do this, even on Windows (because
// the DirectWrite API libui uses on Windows does not do this); to
// specify Arial Condensed, use family Arial and stretch
// uiTextStretchCondensed.
typedef enum uiTextStretch
{
  uiTextStretchUltraCondensed,
  uiTextStretchExtraCondensed,
  uiTextStretchCondensed,
  uiTextStretchSemiCondensed,
  uiTextStretchNormal,
  uiTextStretchSemiExpanded,
  uiTextStretchExpanded,
  uiTextStretchExtraExpanded,
  uiTextStretchUltraExpanded,
} uiTextStretch;

// uiNewStretchAttribute() creates a new uiAttribute that changes the
// stretch of the text it is applied to. It is an error to specify a strech
// not specified in uiTextStretch.
API uiAttribute *uiNewStretchAttribute (uiTextStretch stretch);

// uiAttributeStretch() returns the font stretch stored in a. It is an
// error to call this on a uiAttribute that does not hold a font stretch.
API uiTextStretch uiAttributeStretch (const uiAttribute *a);

// uiNewColorAttribute() creates a new uiAttribute that changes the
// color of the text it is applied to. It is an error to specify an invalid
// color.
API uiAttribute *uiNewColorAttribute (double r, double g, double b, double a);

// uiAttributeColor() returns the text color stored in a. It is an
// error to call this on a uiAttribute that does not hold a text color.
API void uiAttributeColor (const uiAttribute *a, double *r, double *g, double *b, double *alpha);

// uiNewBackgroundAttribute() creates a new uiAttribute that
// changes the background color of the text it is applied to. It is an
// error to specify an invalid color.
API uiAttribute *uiNewBackgroundAttribute (double r, double g, double b, double a);

// TODO reuse uiAttributeColor() for background colors, or make a new function...

// uiUnderline specifies a type of underline to use on text.
typedef enum uiUnderline
{
  uiUnderlineNone,
  uiUnderlineSingle,
  uiUnderlineDouble,
  uiUnderlineSuggestion, // wavy or dotted underlines used for spelling/grammar checkers
} uiUnderline;

// uiNewUnderlineAttribute() creates a new uiAttribute that changes
// the type of underline on the text it is applied to. It is an error to
// specify an underline type not specified in uiUnderline.
API uiAttribute *uiNewUnderlineAttribute (uiUnderline u);

// uiAttributeUnderline() returns the underline type stored in a. It is
// an error to call this on a uiAttribute that does not hold an underline
// style.
API uiUnderline uiAttributeUnderline (const uiAttribute *a);

// uiUnderlineColor specifies the color of any underline on the text it
// is applied to, regardless of the type of underline. In addition to
// being able to specify a custom color, you can explicitly specify
// platform-specific colors for suggestion underlines; to use them
// correctly, pair them with uiUnderlineSuggestion (though they can
// be used on other types of underline as well).
//
// If an underline type is applied but no underline color is
// specified, the text color is used instead. If an underline color
// is specified without an underline type, the underline color
// attribute is ignored, but not removed from the uiAttributedString.
typedef enum uiUnderlineColor
{
  uiUnderlineColorCustom,
  uiUnderlineColorSpelling,
  uiUnderlineColorGrammar,
  uiUnderlineColorAuxiliary, // for instance, the color used by smart replacements on macOS or in Microsoft Office
} uiUnderlineColor;

// uiNewUnderlineColorAttribute() creates a new uiAttribute that
// changes the color of the underline on the text it is applied to.
// It is an error to specify an underline color not specified in
// uiUnderlineColor.
//
// If the specified color type is uiUnderlineColorCustom, it is an
// error to specify an invalid color value. Otherwise, the color values
// are ignored and should be specified as zero.
API uiAttribute *uiNewUnderlineColorAttribute (uiUnderlineColor u, double r, double g, double b, double a);

// uiAttributeUnderlineColor() returns the underline color stored in
// a. It is an error to call this on a uiAttribute that does not hold an
// underline color.
API void uiAttributeUnderlineColor (const uiAttribute *a, uiUnderlineColor *u, double *r, double *g, double *b,
                                    double *alpha);

// uiOpenTypeFeatures represents a set of OpenType feature
// tag-value pairs, for applying OpenType features to text.
// OpenType feature tags are four-character codes defined by
// OpenType that cover things from design features like small
// caps and swashes to language-specific glyph shapes and
// beyond. Each tag may only appear once in any given
// uiOpenTypeFeatures instance. Each value is a 32-bit integer,
// often used as a Boolean flag, but sometimes as an index to choose
// a glyph shape to use.
//
// If a font does not support a certain feature, that feature will be
// ignored.
//
// See the OpenType specification at
// https://www.microsoft.com/typography/otspec/featuretags.htm
// for the complete list of available features, information on specific
// features, and how to use them.
typedef struct uiOpenTypeFeatures uiOpenTypeFeatures;

// uiOpenTypeFeaturesForEachFunc is the type of the function
// invoked by uiOpenTypeFeaturesForEach() for every OpenType
// feature in otf. Refer to that function's documentation for more
// details.
typedef uiForEach (*uiOpenTypeFeaturesForEachFunc) (const uiOpenTypeFeatures *otf, char a, char b, char c, char d,
                                                    uint32_t value, void *data);

// @role uiOpenTypeFeatures constructor
// uiNewOpenTypeFeatures() returns a new uiOpenTypeFeatures
// instance, with no tags yet added.
API uiOpenTypeFeatures *uiNewOpenTypeFeatures (void);

// @role uiOpenTypeFeatures destructor
// uiFreeOpenTypeFeatures() frees otf.
API void uiFreeOpenTypeFeatures (uiOpenTypeFeatures *otf);

// uiOpenTypeFeaturesClone() makes a copy of otf and returns it.
// Changing one will not affect the other.
API uiOpenTypeFeatures *uiOpenTypeFeaturesClone (const uiOpenTypeFeatures *otf);

// uiOpenTypeFeaturesAdd() adds the given feature tag and value
// to otf. The feature tag is specified by a, b, c, and d. If there is
// already a value associated with the specified tag in otf, the old
// value is removed.
API void uiOpenTypeFeaturesAdd (uiOpenTypeFeatures *otf, char a, char b, char c, char d, uint32_t value);

// uiOpenTypeFeaturesRemove() removes the given feature tag
// and value from otf. If the tag is not present in otf,
// uiOpenTypeFeaturesRemove() does nothing.
API void uiOpenTypeFeaturesRemove (uiOpenTypeFeatures *otf, char a, char b, char c, char d);

// uiOpenTypeFeaturesGet() determines whether the given feature
// tag is present in otf. If it is, *value is set to the tag's value and
// nonzero is returned. Otherwise, zero is returned.
//
// Note that if uiOpenTypeFeaturesGet() returns zero, value isn't
// changed. This is important: if a feature is not present in a
// uiOpenTypeFeatures, the feature is NOT treated as if its
// value was zero anyway. Script-specific font shaping rules and
// font-specific feature settings may use a different default value
// for a feature. You should likewise not treat a missing feature as
// having a value of zero either. Instead, a missing feature should
// be treated as having some unspecified default value.
API int uiOpenTypeFeaturesGet (const uiOpenTypeFeatures *otf, char a, char b, char c, char d, uint32_t *value);

// uiOpenTypeFeaturesForEach() executes f for every tag-value
// pair in otf. The enumeration order is unspecified. You cannot
// modify otf while uiOpenTypeFeaturesForEach() is running.
API void uiOpenTypeFeaturesForEach (const uiOpenTypeFeatures *otf, uiOpenTypeFeaturesForEachFunc f, void *data);

// uiNewFeaturesAttribute() creates a new uiAttribute that changes
// the font family of the text it is applied to. otf is copied; you may
// free it after uiNewFeaturesAttribute() returns.
API uiAttribute *uiNewFeaturesAttribute (const uiOpenTypeFeatures *otf);

// uiAttributeFeatures() returns the OpenType features stored in a.
// The returned uiOpenTypeFeatures object is owned by a. It is an
// error to call this on a uiAttribute that does not hold OpenType
// features.
API const uiOpenTypeFeatures *uiAttributeFeatures (const uiAttribute *a);

// uiAttributedString represents a string of UTF-8 text that can
// optionally be embellished with formatting attributes. libui
// provides the list of formatting attributes, which cover common
// formatting traits like boldface and color as well as advanced
// typographical features provided by OpenType like superscripts
// and small caps. These attributes can be combined in a variety of
// ways.
//
// Attributes are applied to runs of Unicode codepoints in the string.
// Zero-length runs are elided. Consecutive runs that have the same
// attribute type and value are merged. Each attribute is independent
// of each other attribute; overlapping attributes of different types
// do not split each other apart, but different values of the same
// attribute type do.
//
// The empty string can also be represented by uiAttributedString,
// but because of the no-zero-length-attribute rule, it will not have
// attributes.
//
// A uiAttributedString takes ownership of all attributes given to
// it, as it may need to duplicate or delete uiAttribute objects at
// any time. By extension, when you free a uiAttributedString,
// all uiAttributes within will also be freed. Each method will
// describe its own rules in more details.
//
// In addition, uiAttributedString provides facilities for moving
// between grapheme clusters, which represent a character
// from the point of view of the end user. The cursor of a text editor
// is always placed on a grapheme boundary, so you can use these
// features to move the cursor left or right by one "character".
// TODO does uiAttributedString itself need this
//
// uiAttributedString does not provide enough information to be able
// to draw itself onto a uiDrawContext or respond to user actions.
// In order to do that, you'll need to use a uiDrawTextLayout, which
// is built from the combination of a uiAttributedString and a set of
// layout-specific properties.
typedef struct uiAttributedString uiAttributedString;

// uiAttributedStringForEachAttributeFunc is the type of the function
// invoked by uiAttributedStringForEachAttribute() for every
// attribute in s. Refer to that function's documentation for more
// details.
typedef uiForEach (*uiAttributedStringForEachAttributeFunc) (const uiAttributedString *s, const uiAttribute *a,
                                                             size_t start, size_t end, void *data);

// @role uiAttributedString constructor
// uiNewAttributedString() creates a new uiAttributedString from
// initialString. The string will be entirely unattributed.
API uiAttributedString *uiNewAttributedString (const char *initialString);

// @role uiAttributedString destructor
// uiFreeAttributedString() destroys the uiAttributedString s.
// It will also free all uiAttributes within.
API void uiFreeAttributedString (uiAttributedString *s);

// uiAttributedStringString() returns the textual content of s as a
// '\0'-terminated UTF-8 string. The returned pointer is valid until
// the next change to the textual content of s.
API const char *uiAttributedStringString (const uiAttributedString *s);

// uiAttributedStringLength() returns the number of UTF-8 bytes in
// the textual content of s, excluding the terminating '\0'.
API size_t uiAttributedStringLen (const uiAttributedString *s);

// uiAttributedStringAppendUnattributed() adds the '\0'-terminated
// UTF-8 string str to the end of s. The new substring will be
// unattributed.
API void uiAttributedStringAppendUnattributed (uiAttributedString *s, const char *str);

// uiAttributedStringInsertAtUnattributed() adds the '\0'-terminated
// UTF-8 string str to s at the byte position specified by at. The new
// substring will be unattributed; existing attributes will be moved
// along with their text.
API void uiAttributedStringInsertAtUnattributed (uiAttributedString *s, const char *str, size_t at);

// uiAttributedStringDelete() deletes the characters and attributes of
// s in the byte range [start, end).
API void uiAttributedStringDelete (uiAttributedString *s, size_t start, size_t end);

// uiAttributedStringSetAttribute() sets a in the byte range [start, end)
// of s. Any existing attributes in that byte range of the same type are
// removed. s takes ownership of a; you should not use it after
// uiAttributedStringSetAttribute() returns.
API void uiAttributedStringSetAttribute (const uiAttributedString *s, uiAttribute *a, size_t start, size_t end);

// uiAttributedStringForEachAttribute() enumerates all the
// uiAttributes in s. It is an error to modify s in f. Within f, s still
// owns the attribute; you can neither free it nor save it for later
// use.
API void uiAttributedStringForEachAttribute (const uiAttributedString *s, uiAttributedStringForEachAttributeFunc f,
                                             void *data);

API size_t uiAttributedStringNumGraphemes (uiAttributedString *s);

API size_t uiAttributedStringByteIndexToGrapheme (uiAttributedString *s, size_t pos);

API size_t uiAttributedStringGraphemeToByteIndex (uiAttributedString *s, size_t pos);

// uiFontDescriptor provides a complete description of a font where
// one is needed. Currently, this means as the default font of a
// uiDrawTextLayout and as the data returned by uiFontButton.
// All the members operate like the respective uiAttributes.
typedef struct uiFontDescriptor uiFontDescriptor;

struct uiFontDescriptor
{
  // TODO const-correct this or figure out how to deal with this when getting a value
  char         *Family;
  double        Size;
  uiTextWeight  Weight;
  uiTextItalic  Italic;
  uiTextStretch Stretch;
};

API void uiLoadControlFont (uiFontDescriptor *f);

API void uiFreeFontDescriptor (uiFontDescriptor *desc);

// uiDrawTextLayout is a concrete representation of a
// uiAttributedString that can be displayed in a uiDrawContext.
// It includes information important for the drawing of a block of
// text, including the bounding box to wrap the text within, the
// alignment of lines of text within that box, areas to mark as
// being selected, and other things.
//
// Unlike uiAttributedString, the content of a uiDrawTextLayout is
// immutable once it has been created.
typedef struct uiDrawTextLayout uiDrawTextLayout;

// uiDrawTextAlign specifies the alignment of lines of text in a
// uiDrawTextLayout.
typedef enum uiDrawTextAlign
{
  uiDrawTextAlignLeft,
  uiDrawTextAlignCenter,
  uiDrawTextAlignRight,
} uiDrawTextAlign;

// uiDrawTextLayoutParams describes a uiDrawTextLayout.
// DefaultFont is used to render any text that is not attributed
// sufficiently in String. Width determines the width of the bounding
// box of the text; the height is determined automatically.
typedef struct uiDrawTextLayoutParams uiDrawTextLayoutParams;

struct uiDrawTextLayoutParams
{
  uiAttributedString *String;
  uiFontDescriptor   *DefaultFont;
  double              Width;
  uiDrawTextAlign     Align;
};

// @role uiDrawTextLayout constructor
// uiDrawNewTextLayout() creates a new uiDrawTextLayout from
// the given parameters.
API uiDrawTextLayout *uiDrawNewTextLayout (uiDrawTextLayoutParams *params);

// @role uiDrawFreeTextLayout destructor
// uiDrawFreeTextLayout() frees tl. The underlying
// uiAttributedString is not freed.
API void uiDrawFreeTextLayout (uiDrawTextLayout *tl);

// uiDrawText() draws tl in c with the top-left point of tl at (x, y).
API void uiDrawText (uiDrawContext *c, uiDrawTextLayout *tl, double x, double y);

// uiDrawTextLayoutExtents() returns the width and height of tl
// in width and height. The returned width may be smaller than
// the width passed into uiDrawNewTextLayout() depending on
// how the text in tl is wrapped. Therefore, you can use this
// function to get the actual size of the text layout.
API void uiDrawTextLayoutExtents (uiDrawTextLayout *tl, double *width, double *height);

/**
 * A button-like control that opens a font chooser when clicked.
 *
 * @struct uiFontButton
 * @extends uiControl
 * @ingroup button dataEntry
 */
typedef struct uiFontButton uiFontButton;
#define uiFontButton(this) ((uiFontButton *)(this))

/**
 * Returns the selected font.
 *
 * @param b uiFontButton instance.
 * @param[out] desc Font descriptor. [Default: OS-dependent].
 * @note Make sure to call `uiFreeFontButtonFont()` to free all allocated
 *       resources within @p desc.
 * @memberof uiFontButton
 */
API void uiFontButtonFont (uiFontButton *b, uiFontDescriptor *desc);

/**
 *  Registers a callback for when the font is changed.
 *
 * @param b uiFontButton instance.
 * @param f Callback function.\n
 *          @p sender Back reference to the instance that triggered the callback.\n
 *          @p senderData User data registered with the sender instance.
 * @param data User data to be passed to the callback.
 *
 * @note Only one callback can be registered at a time.
 * @memberof uiFontButton
 */
API void uiFontButtonOnChanged (uiFontButton *b, void (*f) (uiFontButton *sender, void *senderData), void *data);

/**
 * Creates a new font button.
 *
 * The default font is determined by the OS defaults.
 *
 * @returns A new uiFontButton instance.
 * @memberof uiFontButton @static
 */
API uiFontButton *uiNewFontButton (void);

/**
 * Frees a uiFontDescriptor previously filled by uiFontButtonFont().
 *
 * After calling this function the contents of @p desc should be assumed undefined,
 * however you can safely reuse @p desc.
 *
 * Calling this function on a uiFontDescriptor not previously filled by
 * uiFontButtonFont() results in undefined behavior.
 *
 * @param desc Font descriptor to free.
 * @memberof uiFontButton
 */
API void uiFreeFontButtonFont (uiFontDescriptor *desc);

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

struct uiAreaMouseEvent
{
  double X;
  double Y;

  double AreaWidth;
  double AreaHeight;

  int Down;
  int Up;

  int Count;

  uiModifiers Modifiers;

  uint64_t Held1To64;
};

typedef enum uiExtKey
{
  uiExtKeyEscape = 1,
  uiExtKeyInsert, // equivalent to "Help" on Apple keyboards
  uiExtKeyDelete,
  uiExtKeyHome,
  uiExtKeyEnd,
  uiExtKeyPageUp,
  uiExtKeyPageDown,
  uiExtKeyUp,
  uiExtKeyDown,
  uiExtKeyLeft,
  uiExtKeyRight,
  uiExtKeyF1, // F1..F12 are guaranteed to be consecutive
  uiExtKeyF2,
  uiExtKeyF3,
  uiExtKeyF4,
  uiExtKeyF5,
  uiExtKeyF6,
  uiExtKeyF7,
  uiExtKeyF8,
  uiExtKeyF9,
  uiExtKeyF10,
  uiExtKeyF11,
  uiExtKeyF12,
  uiExtKeyN0, // numpad keys; independent of Num Lock state
  uiExtKeyN1, // N0..N9 are guaranteed to be consecutive
  uiExtKeyN2,
  uiExtKeyN3,
  uiExtKeyN4,
  uiExtKeyN5,
  uiExtKeyN6,
  uiExtKeyN7,
  uiExtKeyN8,
  uiExtKeyN9,
  uiExtKeyNDot,
  uiExtKeyNEnter,
  uiExtKeyNAdd,
  uiExtKeyNSubtract,
  uiExtKeyNMultiply,
  uiExtKeyNDivide,
} uiExtKey;

struct uiAreaKeyEvent
{
  char        Key;
  uiExtKey    ExtKey;
  uiModifiers Modifier;

  uiModifiers Modifiers;

  int Up;
};

/**
 * A control with a color indicator that opens a color chooser when clicked.
 *
 * The control visually represents a button with a color field representing
 * the selected color.
 *
 * Clicking on the button opens up a color chooser in form of a color palette.
 *
 * @struct uiColorButton
 * @extends uiControl
 * @ingroup dataEntry button
 */
typedef struct uiColorButton uiColorButton;
#define uiColorButton(this) ((uiColorButton *)(this))

/**
 * Returns the color button color.
 *
 * @param b uiColorButton instance.
 * @param[out] r Red. Double in range of [0, 1.0].
 * @param[out] g Green. Double in range of [0, 1.0].
 * @param[out] bl Blue. Double in range of [0, 1.0].
 * @param[out] a Alpha. Double in range of [0, 1.0].
 * @memberof uiColorButton
 */
API void uiColorButtonColor (uiColorButton *b, double *r, double *g, double *bl, double *a);

/**
 * Sets the color button color.
 *
 * @param b uiColorButton instance.
 * @param r Red. Double in range of [0, 1.0].
 * @param g Green. Double in range of [0, 1.0].
 * @param bl Blue. Double in range of [0, 1.0].
 * @param a Alpha. Double in range of [0, 1.0].
 * @memberof uiColorButton
 */
API void uiColorButtonSetColor (uiColorButton *b, double r, double g, double bl, double a);

/** Registers a callback for when the color is changed.
 *
 * @param b uiColorButton instance.
 * @param f Callback function.\n
 *          @p sender Back reference to the instance that triggered the callback.\n
 *          @p senderData User data registered with the sender instance.
 * @param data User data to be passed to the callback.
 *
 * @note The callback is not triggered when calling uiColorButtonSetColor().
 * @note Only one callback can be registered at a time.
 * @memberof uiColorButton
 */
API void uiColorButtonOnChanged (uiColorButton *b, void (*f) (uiColorButton *sender, void *senderData), void *data);

/**
 * Creates a new color button.
 *
 * @returns A new uiColorButton instance.
 * @memberof uiColorButton @static
 */
API uiColorButton *uiNewColorButton (void);
