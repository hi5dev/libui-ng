#pragma once

#include "api.h"
#include "attribute.h"

/**
 * @brief Represents a string of UTF-8 text that can optionally be embellished with formatting attributes.
 *
 * LibUI provides the list of formatting attributes, which cover common formatting traits like boldface and color as
 * well as advanced typographical features provided by OpenType like superscripts and small caps. These attributes can
 * be combined in a variety of ways.
 *
 * - Attributes are applied to runs of Unicode codepoints in the string.
 * - Zero-length runs are elided.
 * - Consecutive runs that have the same attribute type and value are merged.
 * - Each attribute is independent of each other attribute.
 * - Overlapping attributes of different types do not split each other apart, but different values of the same
 * attribute type do.
 *
 * The empty string can also be represented by @p uiAttributedString, but because of the no-zero-length-attribute
 * rule, it will not have attributes.
 *
 * A @p uiAttributedString takes ownership of all attributes given to it, as it may need to duplicate or delete
 * @p uiAttribute objects at any time. By extension, when you free a uiAttributedString, all uiAttributes within will
 * also be freed. Each method will describe its own rules in more details.
 *
 * In addition,@p uiAttributedString provides facilities for moving between grapheme clusters, which represent a
 * character from the point of view of the end user. The cursor of a text editor is always placed on a grapheme
 * boundary, so you can use these features to move the cursor left or right by one "character".
 *
 * @p uiAttributedString does not provide enough information to be able to draw itself onto a @p uiDrawContext or
 * respond to user actions. In order to do that, you'll need to use a @p uiDrawTextLayout, which is built from the
 * combination of a @p uiAttributedString and a set of layout-specific properties.
 */
typedef struct uiAttributedString uiAttributedString;

/**
 * @brief Function invoked by @p uiAttributedStringForEachAttribute for every attribute in @p s
 * @details Refer to each function's documentation for more details.
 * @param s @p uiAttributedString
 * @param a @p uiAttribute
 * @param start @p size_t
 * @param end @p size_t
 * @param data user-defined data
 * @return @p uiForEach
 */
typedef uiForEach (*uiAttributedStringForEachAttributeFunc) (const uiAttributedString *s, const uiAttribute *a,
                                                             size_t start, size_t end, void *data);

/**
 * @brief @p uiAttributedString constructor
 * @details Creates a new @p uiAttributedString from initialString. The string will be entirely unattributed.
 * @param initialString string
 * @return @p uiAttributedString
 */
API uiAttributedString *uiNewAttributedString (const char *initialString);

/**
 * @brief @p uiAttributedString destructor
 * @param s @p uiAttributedString
 * @remarks also frees all @p uiAttributes within
 */
API void uiFreeAttributedString (uiAttributedString *s);

/**
 * @brief Gets the textual content of a @p uiAttributedString
 * @param s @p uiAttributedString
 * @return string
 * @remark The returned pointer is valid until the next change to the textual content of @p s.
 */
API const char *uiAttributedStringString (const uiAttributedString *s);

/**
 * @brief Gets the number of UTF-8 bytes in the textual content of @p s, excluding the terminating @p NULL character.
 * @param s @p uiAttributedString
 * @return @p size_t
 */
API size_t uiAttributedStringLen (const uiAttributedString *s);

/**
 * @brief Appends a @p NULL terminator to the given string
 * @details The new substring will be unattributed.
 * @param[out] s @p uiAttributedString
 * @param str string
 */
API void uiAttributedStringAppendUnattributed (uiAttributedString *s, const char *str);

/**
 * @brief Adds a @p NULL terminator to a string at the given byte position
 * @details The new substring will be unattributed; existing attributes will be moved along with their text.
 * @param[out] s @p uiAttributedString
 * @param str string
 * @param at position
 */
API void uiAttributedStringInsertAtUnattributed (uiAttributedString *s, const char *str, size_t at);

/**
 * @brief Deletes the characters and attributes of @p s in the given byte range
 * @param s @p uiAttributedString
 * @param start of the byte range
 * @param end of the byte range
 */
API void uiAttributedStringDelete (uiAttributedString *s, size_t start, size_t end);

/**
 * @brief Sets @p a in the given byte range of @p s.
 * @details Any existing attributes in that byte range of the same type are removed.
 * @param s @p uiAttributedString
 * @param a @p uiAttribute
 * @param start of the byte range
 * @param end of the byte range
 * @remark @p s takes ownership of @p a
 */
API void uiAttributedStringSetAttribute (const uiAttributedString *s, uiAttribute *a, size_t start, size_t end);

/**
 * @brief Enumerates all the @p uiAttributes in @p s.
 * @param s @p uiAttributedString
 * @param f pointer to the callback function
 * @param data user-defined data
 * @remark It is an error to modify @p s in @p f.
 * @remark Within @p f, @p s still owns the attribute; you can neither free it nor save it for later use.
 */
API void uiAttributedStringForEachAttribute (const uiAttributedString *s, uiAttributedStringForEachAttributeFunc f,
                                             void *data);

/**
 * @brief Gets the number of graphemes in @p s
 * @param s @p uiAttributedString
 * @return number of graphemes
 */
API size_t uiAttributedStringNumGraphemes (uiAttributedString *s);

/**
 * @brief Gets the grapheme at the given byte index
 * @param s @p uiAttributedString
 * @param pos @p size_t
 * @return grapheme
 */
API size_t uiAttributedStringByteIndexToGrapheme (uiAttributedString *s, size_t pos);

/**
 * @brief Gets the byte index of the given grapheme
 * @param s @p uiAttributedString
 * @param pos @p size_t
 * @return byte index
 */
API size_t uiAttributedStringGraphemeToByteIndex (uiAttributedString *s, size_t pos);
