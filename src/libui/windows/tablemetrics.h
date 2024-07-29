#pragma once

#include <windef.h>

#include <ui/table.h>

typedef struct uiprivTableMetrics uiprivTableMetrics;
struct uiprivTableMetrics
{
  BOOL hasText;

  BOOL hasImage;

  BOOL focused;

  BOOL selected;

  RECT itemBounds;

  RECT itemIcon;

  RECT itemLabel;

  RECT subitemBounds;

  RECT subitemIcon;

  RECT subitemLabel;

  LRESULT bitmapMargin;

  int cxIcon;

  int cyIcon;

  RECT realTextBackground;

  RECT realTextRect;
};
extern HRESULT uiprivTableGetMetrics (uiTable *t, int iItem, int iSubItem, uiprivTableMetrics **mout);
