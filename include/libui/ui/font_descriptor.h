#pragma once

#include "api.h"

/**
 * @brief Provides a complete description of a font where one is needed.
 * @details Currently, this means as the default font of a @p uiDrawTextLayout and as the data returned by
 * @p uiFontButton.
 * @remark all the members operate like the respective @p uiAttributes
 */
typedef struct uiFontDescriptor uiFontDescriptor;

struct uiFontDescriptor
{
  /**
   * @brief Font family
   */
  char *Family;

  /**
   * @brief Font size
   */
  double Size;

  /**
   * @brief Font weight
   */
  uiTextWeight Weight;

  /**
   * @brief Font italics
   */
  uiTextItalic Italic;

  /**
   * @brief Font stretch
   */
  uiTextStretch Stretch;
};

/**
 * @brief @p uiFontDescriptor constructor
 * @param f @p uiFontDescriptor
 */
API void uiLoadControlFont (uiFontDescriptor *f);

/**
 * @brief @p uiFontDescriptor destructor
 * @param desc @p uiFontDescriptor
 */
API void uiFreeFontDescriptor (uiFontDescriptor *desc);
