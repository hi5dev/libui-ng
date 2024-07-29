#pragma once

#include <windef.h>

#include <d2d1.h>

#include <type_traits>

/**
 * @brief Adjusts the given width and height to include window decorations.
 * @param hwnd handle to the window
 * @param[out] width size
 * @param[out] height size
 * @param hasMenubar if the window has a menu bar
 */
extern void clientSizeToWindowSize (HWND hwnd, int *width, int *height, BOOL hasMenubar);

/**
 * @brief Retrieves a handle to a control in the specified dialog box.
 * @param hwnd handle to the dialog box that contains the control
 * @param id identifer of the control to be retrieved
 * @return handle of the specified control or @p NULL if not found
 */
extern HWND getDlgItem (HWND hwnd, int id);

/**
 * @brief Retrieves the extended window styles.
 * @param hwnd handle to the window and, indirectly, the class to which the window belongs
 * @return the window style
 * @return zero if there's an error - use @p GetLastError to retrieve the error
 */
extern DWORD getExStyle (HWND hwnd);

/**
 * @brief Retrieves the window styles.
 * @param hwnd handle to the window and, indirectly, the class to which the window belongs
 * @return the window style
 * @return zero if there's an error - use @p GetLastError to retrieve the error
 */
extern DWORD getStyle (HWND hwnd);

/**
 * @brief Adds a rectangle to the specified window's update region.
 * @remark The update region represents the portion of the window's client area that must be redrawn.
 * @param hwnd handle to the window whose update region has changed. If this parameter is @p NULL, the system
 * invalidates and redraws all windows, not just the windows for this application, and sends the @p WM_ERASEBKGND and
 * @p WM_NCPAINT messages before the function returns. Setting this parameter to @p NULL is not recommended.
 * @param r the client coordinates of the rectangle to be added to the update region. If this parameter is @p NULL,
 * the entire client area is added to the update region.
 * @param erase Determines if the background within the update region is to be erased when the update region is
 * processed. When @p TRUE, the background is erased when the @p BeginPaint function is called. When @p FALSE,
 * the background remains unchanged.
 */
extern void invalidateRect (HWND hwnd, const RECT *r, BOOL erase);

/**
 * @brief Wraps the Win32 API function @p MapWindowRect to handle complex error handling.
 * @param from handle to the source window.
 * @param to handle to the target window.
 * @param r The rectangle to be mapped.
 */
extern void mapWindowRect (HWND from, HWND to, RECT *r);

/**
 * @brief Retrieves the parent window. This does not include the owner, as it does with the @p GetParent function.
 * @param child handle to the window whose ancestor is to be retrieved. If this parameter is the desktop window,
 * the function returns @p NULL.
 * @return handle to the ancestor window
 */
extern HWND parentOf (HWND child);

/**
 * @brief Retrieves the root window by walking the chain of parent windows.
 * @param child handle to the window whose ancestor is to be retrieved. If this parameter is the desktop window,
 * the function returns @p NULL.
 * @return handle to the ancestor window
 */
extern HWND parentToplevel (HWND child);

/**
 * @brief Returns the size of the render target.
 * @param rt @p ID2D1RenderTarget
 * @return current size of the render target in device-independent pixels
 */
D2D1_SIZE_F realGetSize (ID2D1RenderTarget *rt);

/**
 * @brief Sets a new extended window style.
 * @param hwnd handle to the window and, indirectly, the class to which the window belongs.
 * @param exstyle zero-based offset to the value to be set
 */
extern void setExStyle (HWND hwnd, DWORD exstyle);

/**
 * @brief Sets a new window style.
 * @param hwnd handle to the window and, indirectly, the class to which the window belongs.
 * @param style zero-based offset to the value to be set
 */
extern void setStyle (HWND hwnd, DWORD style);

/**
 * @brief Changes the Z-order of a window so it comes after another.
 * @param hwnd handle of the source window
 * @param insertAfter handle of the target window
 */
extern void setWindowInsertAfter (HWND hwnd, HWND insertAfter);

/**
 * @brief Ttakes the logic of determining window classes and puts it all in one place.
 *
 * There are a number of places where we need to know what window class an arbitrary handle has theoretically we
 * could use the class atom to avoid a @p _wcsicmp()  however, raymond chen advises against this.
 *
 * @example
 * @code
 * windowClassOf(hwnd, L"class 1", L"class 2", ..., NULL)
 * @endcode
 *
 * @tparam Klass @p WCHAR
 * @tparam Ts @p WCHAR
 * @param hwnd handle of the window
 * @param klass1 the first class name
 * @param klasses @p NULL terminated list of additional class names
 * @return index of the matching class or @p -1 if no match is found
 */
template <typename Klass, typename... Ts, typename std::enable_if<std::is_integral<Klass>::value>::type * = nullptr>
extern int windowClassOf (HWND hwnd, Klass *klass1, Ts... klasses);
