#pragma once

#include <windef.h>

#define hexd       L"0123456789ABCDEF"
#define nStops     (30)
#define degPerStop (360 / nStops)
#define stopIncr   (1.0 / ((double)nStops))

struct colorDialogRGBA
{
  double r;

  double g;

  double b;

  double a;
};

struct colorDialog
{
  HWND hwnd;

  HWND svChooser;

  HWND hSlider;

  HWND preview;

  HWND opacitySlider;

  HWND editH;

  HWND editS;

  HWND editV;

  HWND editRDouble;

  HWND editRInt;

  HWND editGDouble;

  HWND editGInt;

  HWND editBDouble;

  HWND editBInt;

  HWND editADouble;

  HWND editAInt;

  HWND editHex;

  double h;

  double s;

  double v;

  double a;

  colorDialogRGBA *out;

  BOOL updating;
};

extern BOOL showColorDialog (HWND parent, colorDialogRGBA *c);
