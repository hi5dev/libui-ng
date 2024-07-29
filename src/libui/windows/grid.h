#pragma once

#include <windef.h>

#include <ui/control.h>
#include <ui/grid.h>
#include <ui_win32.h>

#include <map>
#include <vector>

#define xcount(g)      ((g)->xmax - (g)->xmin)
#define ycount(g)      ((g)->ymax - (g)->ymin)
#define toxindex(g, x) ((x) - (g)->xmin)
#define toyindex(g, y) ((y) - (g)->ymin)

struct gridChild
{
  uiControl *c;

  int left;

  int top;

  int xspan;

  int yspan;

  int hexpand;

  uiAlign halign;

  int vexpand;

  uiAlign valign;

  int finalx;

  int finaly;

  int finalwidth;

  int finalheight;

  int minwidth;

  int minheight;
};

struct uiGrid
{
  uiWindowsControl c;

  HWND hwnd;

  std::vector<gridChild *> *children;

  std::map<uiControl *, size_t> *indexof;

  int padded;

  int xmin;

  int ymin;

  int xmax;

  int ymax;
};
