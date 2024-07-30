#pragma once

#include <windef.h>

#include <ui_win32.h>

#include <vector>

#define radiobuttonHeight                      10
#define radiobuttonXFromLeftOfBoxToLeftOfLabel 12

struct uiRadioButtons
{
  uiWindowsControl c;

  /**
   * @brief Handle of the container.
   */
  HWND hwnd;

  /**
   * @brief Handle of the buttons.
   */
  std::vector<HWND> *hwnds;

  void (*onSelected) (uiRadioButtons *, void *);

  void *onSelectedData;
};
