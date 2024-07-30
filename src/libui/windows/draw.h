#pragma once

#include <windows.h>

#include <d2d1.h>

#include <ui/draw.h>

#include <vector>

struct uiDrawContext
{
  ID2D1RenderTarget *rt;

  std::vector<struct drawState> *states;

  ID2D1PathGeometry *currentClip;
};

extern HRESULT initDraw ();

extern void uninitDraw ();

extern uiDrawContext *newContext (ID2D1RenderTarget *);

extern void freeContext (uiDrawContext *);

extern ID2D1DCRenderTarget *makeHDCRenderTarget (HDC dc, const RECT *r);

extern ID2D1HwndRenderTarget *makeHWNDRenderTarget (HWND hwnd);

extern ID2D1Factory *d2dfactory;


// drawmatrix.cpp
extern void m2d (uiDrawMatrix *m, D2D1_MATRIX_3X2_F *d);
