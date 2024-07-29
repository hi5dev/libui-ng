#pragma once

#include <dwrite.h>

#include <ui/attribute.h>
#include <ui/font_descriptor.h>

extern DWRITE_FONT_WEIGHT uiprivWeightToDWriteWeight (uiTextWeight w);

extern DWRITE_FONT_STYLE uiprivItalicToDWriteStyle (uiTextItalic i);

extern DWRITE_FONT_STRETCH uiprivStretchToDWriteStretch (uiTextStretch s);

extern void uiprivFontDescriptorFromIDWriteFont (IDWriteFont *font, uiFontDescriptor *uidesc);
