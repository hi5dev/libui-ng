#pragma once

#include <windows.h>

#include <commctrl.h>

#define d2dScratchClass L"libui_d2dScratchClass"

/**
 * @brief The Direct2D scratch window is a utility for libui internal use to do quick things with Direct2D.
 *
 * To use, call @p newD2DScratch passing in a subclass procedure. This subclass procedure should handle the
 * @p msgD2DScratchPaint message, which has the following usage:
 * - @p wParam  - @p 0
 * - @p lParam  - @p ID2D1RenderTarget*
 * - @p lResult - @p 0
 *
 * You can optionally also handle @p msgD2DScratchLButtonDown, which is sent when the left mouse button is either
 * pressed for the first time or held while the mouse is moving.
 * - @p wParam  - position in DIPs, as @p D2D1_POINT_2F*
 * - @p lParam  - size of render target in DIPs, as @p D2D1_SIZE_F*
 * - @p lResult - @p 0
 *
 * Other messages can also be handled here.
 */
extern HWND newD2DScratch (HWND parent, const RECT *rect, HMENU controlID, SUBCLASSPROC subclass,
                           DWORD_PTR subclassData);

extern ATOM registerD2DScratchClass (HICON hDefaultIcon, HCURSOR hDefaultCursor);

extern void unregisterD2DScratchClass ();
