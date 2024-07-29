#pragma once

#include "fontcollection.h"

typedef struct fontDialogParams
{
  IDWriteFont *font;
  double       size;
  WCHAR       *familyName;
  WCHAR       *styleName;
} fontDialogParams;

typedef struct fontDialog
{
  HWND hwnd;
  HWND familyCombobox;
  HWND styleCombobox;
  HWND sizeCombobox;

  fontDialogParams *params;

  struct fontCollection *fc;

  RECT sampleRect;
  HWND sampleBox;

  LRESULT curFamily;
  LRESULT curStyle;
  double  curSize;

  DWRITE_FONT_WEIGHT  weight;
  DWRITE_FONT_STYLE   style;
  DWRITE_FONT_STRETCH stretch;
} fontDialog;

extern BOOL uiprivShowFontDialog (HWND parent, fontDialogParams *params);

extern void uiprivLoadInitialFontDialogParams (fontDialogParams *params);

extern void uiprivDestroyFontDialogParams (const fontDialogParams *params);

extern WCHAR *uiprivFontDialogParamsToString (const fontDialogParams *params);
