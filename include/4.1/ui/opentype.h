#pragma once

#include <stdint.h>

/**
 * @brief Represents a set of OpenType feature tag-value pairs, for applying OpenType features to text.
 *
 * OpenType feature tags are four-character codes defined by OpenType that cover things from design features like
 * small caps and swashes to language-specific glyph shapes and beyond. Each tag may only appear once in any given
 * uiOpenTypeFeatures instance. Each value is a 32-bit integer, often used as a Boolean flag, but sometimes as an
 * index to choose a glyph shape to use.
 *
 * @remark If a font does not support a certain feature, that feature will be ignored.
 * @see https://www.microsoft.com/typography/otspec/featuretags.htm
 */
typedef struct uiOpenTypeFeatures uiOpenTypeFeatures;

/**
 * @brief Function invoked by @p uiOpenTypeFeaturesForEach for every OpenType feature in @p otf.
 * @param otf @p uiOpenTypeFeatures
 * @param a @p char
 * @param b @p char
 * @param c @p char
 * @param d @p char
 * @param value @p uint32_t
 * @param data user-defined data
 * @return @p uiForEach
 * @remark Refer to that function's documentation for more details.
 */
typedef uiForEach (*uiOpenTypeFeaturesForEachFunc) (const uiOpenTypeFeatures *otf, char a, char b, char c, char d,
                                                    uint32_t value, void *data);

/**
 * @brief @p uiOpenTypeFeatures constructor
 * @return @p uiOpenTypeFeatures
 */
API uiOpenTypeFeatures *uiNewOpenTypeFeatures (void);

/**
 * @brief @p uiOpenTypeFeatures destructor
 * @param otf @p uiOpenTypeFeatures
 */
API void uiFreeOpenTypeFeatures (uiOpenTypeFeatures *otf);

/**
 * @brief Clones a @p uiOpenTypeFeatures
 * @param otf @p uiOpenTypeFeatures
 * @return @p uiOpenTypeFeatures
 */
API uiOpenTypeFeatures *uiOpenTypeFeaturesClone (const uiOpenTypeFeatures *otf);

/**
 * @brief Adds the given feature tag and value to @p otf.
 * @details The feature tag is specified by @p a, @p b, @p c, and @p d.
 * @param otf @p uiOpenTypeFeatures
 * @param a @p char
 * @param b @p char
 * @param c @p char
 * @param d @p char
 * @param value @p uint32_t
 * @remark If there is already a value associated with the* specified tag in @p otf, the old value is removed.
 */
API void uiOpenTypeFeaturesAdd (uiOpenTypeFeatures *otf, char a, char b, char c, char d, uint32_t value);

/**
 * @brief Removes the given feature tag and value from @p otf
 * @param otf @p uiOpenTypeFeatures
 * @param a @p char
 * @param b @p char
 * @param c @p char
 * @param d @p char
 * @remark If the tag is not present in @p otf, this function does nothing
 */
API void uiOpenTypeFeaturesRemove (uiOpenTypeFeatures *otf, char a, char b, char c, char d);

/**
 * @brief Determines whether the given feature tag is present in @p otf.
 *
 * This is important: if a feature is not present in a @p uiOpenTypeFeatures, the feature is not treated as if its
 * value was zero anyway. Script-specific font shaping rules and font-specific feature settings may use a different
 * default value for a feature. You should likewise not treat a missing feature as having a value of zero either.
 * Instead, a missing feature should be treated as having some unspecified default value.
 *
 * @param otf @p uiOpenTypeFeatures
 * @param a @p char
 * @param b @p char
 * @param c @p char
 * @param d @p char
 * @param[out] value @p uint32_t
 * @return non-zero when a value is returned, zero otherwise
 */
API int uiOpenTypeFeaturesGet (const uiOpenTypeFeatures *otf, char a, char b, char c, char d, uint32_t *value);

/**
 * @brief Executes @p f for every tag-value pair in @p otf
 * @param otf @p uiOpenTypeFeatures
 * @param f pointer to the for-each function
 * @param data user-defined data to pass to @p f
 * @remark The enumeration order is unspecified. You cannot modify otf while @p uiOpenTypeFeaturesForEach is running
 */
API void uiOpenTypeFeaturesForEach (const uiOpenTypeFeatures *otf, uiOpenTypeFeaturesForEachFunc f, void *data);
