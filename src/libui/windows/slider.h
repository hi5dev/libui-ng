#pragma once

#include <windef.h>

#include <ui/slider.h>

#include <ui_win32.h>

#define sliderWidth  107
#define sliderHeight 15

struct uiSlider
{
  uiWindowsControl c;

  HWND hwnd;

  void (*onChanged) (uiSlider *, void *);

  void *onChangedData;

  void (*onReleased) (uiSlider *, void *);

  void *onReleasedData;

  HWND hwndToolTip;
};
