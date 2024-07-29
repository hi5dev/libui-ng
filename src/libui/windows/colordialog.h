#pragma once

#include <windef.h>

struct colorDialogRGBA
{
  double r;
  double g;
  double b;
  double a;
};

extern BOOL showColorDialog (HWND parent, colorDialogRGBA *c);
