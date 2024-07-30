#pragma once

#include <windows.h>

#include <ui/area.h>

struct scrollParams
{
  int *pos;
  int  pagesize;
  int  length;
  int *wheelCarry;
  UINT wheelSPIAction;
};

extern BOOL areaDoScroll (uiArea *a, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult);

extern void areaScrollOnResize (uiArea *, RECT *);

extern void areaUpdateScroll (uiArea *a);
