#pragma once

#include <windef.h>

#include <ui/tab.h>
#include <ui_win32.h>

#include <vector>

/**
 * @remark You don't add controls directly to a tab control on Windows; instead you make them siblings and swap between
 * them on a TCN_SELCHANGING/TCN_SELCHANGE notification pair.
 * @remark In addition, you use dialogs because they can be textured properly; other controls cannot (Things will look
 * wrong if the tab background in the current theme is fancy if you just use the tab background by itself.
 */
struct uiTab
{
  uiWindowsControl c;

  /**
   * @brief Handle of the outer container
   */
  HWND hwnd;

  /**
   * @brief Handle of the tab control itself
   */
  HWND tabHWND;

  std::vector<struct tabPage *> *pages;

  HWND parent;
};
