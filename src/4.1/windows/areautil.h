#pragma once

#include <windows.h>

#include <d2d1.h>

#include "area.h"

extern void loadAreaSize (const uiArea *a, ID2D1RenderTarget *rt, double *width, double *height);

extern void pixelsToDIP (const uiArea *a, double *x, double *y);

extern void dipToPixels (const uiArea *a, double *x, double *y);
