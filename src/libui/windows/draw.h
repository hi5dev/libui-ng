#pragma once

#include <d2d1.h>

#include <ui/draw.h>

extern HRESULT initDraw ();

extern void uninitDraw ();

extern uiDrawContext *newContext (ID2D1RenderTarget *);

extern void freeContext (uiDrawContext *);

extern ID2D1DCRenderTarget *makeHDCRenderTarget (HDC dc, const RECT *r);

extern ID2D1HwndRenderTarget *makeHWNDRenderTarget (HWND hwnd);
