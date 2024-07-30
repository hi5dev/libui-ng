#include "fontmatch.h"

static uiTextItalic
uiprivDWriteFontStyleToUiItalic (const DWRITE_FONT_STYLE font_style)
{
  switch (font_style)
    {
    default:
    case DWRITE_FONT_STYLE_NORMAL:
      return uiTextItalicNormal;
    case DWRITE_FONT_STYLE_OBLIQUE:
      return uiTextItalicOblique;
    case DWRITE_FONT_STYLE_ITALIC:
      return uiTextItalicItalic;
    }
}

static uiTextStretch
uiprivDWriteFontStretchToUiStretch (const DWRITE_FONT_STRETCH stretch)
{
  switch (stretch)
    {
    default:
    case DWRITE_FONT_STRETCH_NORMAL:
      return uiTextStretchNormal;
    case DWRITE_FONT_STRETCH_ULTRA_CONDENSED:
      return uiTextStretchUltraCondensed;
    case DWRITE_FONT_STRETCH_EXTRA_CONDENSED:
      return uiTextStretchExtraCondensed;
    case DWRITE_FONT_STRETCH_CONDENSED:
      return uiTextStretchCondensed;
    case DWRITE_FONT_STRETCH_SEMI_CONDENSED:
      return uiTextStretchSemiCondensed;
    case DWRITE_FONT_STRETCH_SEMI_EXPANDED:
      return uiTextStretchSemiExpanded;
    case DWRITE_FONT_STRETCH_EXPANDED:
      return uiTextStretchExpanded;
    case DWRITE_FONT_STRETCH_EXTRA_EXPANDED:
      return uiTextStretchExtraExpanded;
    case DWRITE_FONT_STRETCH_ULTRA_EXPANDED:
      return uiTextStretchUltraExpanded;
    }
}

DWRITE_FONT_WEIGHT
uiprivWeightToDWriteWeight (uiTextWeight w) { return static_cast<DWRITE_FONT_WEIGHT> (w); }

DWRITE_FONT_STYLE
uiprivItalicToDWriteStyle (const uiTextItalic i)
{
  switch (i)
    {
    default:
    case uiTextItalicNormal:
      return DWRITE_FONT_STYLE_NORMAL;
    case uiTextItalicOblique:
      return DWRITE_FONT_STYLE_OBLIQUE;
    case uiTextItalicItalic:
      return DWRITE_FONT_STYLE_ITALIC;
    }
}

DWRITE_FONT_STRETCH
uiprivStretchToDWriteStretch (const uiTextStretch s)
{
  switch (s)
    {
    default:
    case uiTextStretchNormal:
      return DWRITE_FONT_STRETCH_NORMAL;
    case uiTextStretchUltraCondensed:
      return DWRITE_FONT_STRETCH_ULTRA_CONDENSED;
    case uiTextStretchExtraCondensed:
      return DWRITE_FONT_STRETCH_EXTRA_CONDENSED;
    case uiTextStretchCondensed:
      return DWRITE_FONT_STRETCH_CONDENSED;
    case uiTextStretchSemiCondensed:
      return DWRITE_FONT_STRETCH_SEMI_CONDENSED;
    case uiTextStretchSemiExpanded:
      return DWRITE_FONT_STRETCH_SEMI_EXPANDED;
    case uiTextStretchExpanded:
      return DWRITE_FONT_STRETCH_EXPANDED;
    case uiTextStretchExtraExpanded:
      return DWRITE_FONT_STRETCH_EXTRA_EXPANDED;
    case uiTextStretchUltraExpanded:
      return DWRITE_FONT_STRETCH_ULTRA_EXPANDED;
    }
}

void
uiprivFontDescriptorFromIDWriteFont (IDWriteFont *font, uiFontDescriptor *uidesc)
{
  uidesc->Weight  = static_cast<uiTextWeight> (font->GetWeight ());
  uidesc->Italic  = uiprivDWriteFontStyleToUiItalic (font->GetStyle ());
  uidesc->Stretch = uiprivDWriteFontStretchToUiStretch (font->GetStretch ());
}
