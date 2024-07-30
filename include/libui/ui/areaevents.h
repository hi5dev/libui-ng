#pragma once

#include "api.h"

#include <stdint.h>

typedef struct uiprivClickCounter uiprivClickCounter;

/**
 * @remarks Call Reset() to zero-initialize a new instance. It doesn't matter that all the non-count fields are zero.
 * The first click will fail the curButton test straightaway, so it'll return 1 and set the rest of the structure
 * accordingly
 */
struct uiprivClickCounter
{
  int       curButton;
  int       rectX0;
  int       rectY0;
  int       rectX1;
  int       rectY1;
  uintptr_t prevTime;
  int       count;
};

/**
 * @brief Used to detect the number of clicks - double-click, triple, quadruple, etc.
 *
 * Windows and GTK+ have a limit of 2 and 3 clicks, respectively, natively supported. Fortunately, we can simulate the
 * double/triple-click behavior to build higher-order clicks. We can use the same algorithm Windows uses on both.
 * For GTK+, we pull the double-click time and double-click distance, which work the same as the equivalents on Windows
 * (so the distance is in all directions), from the GtkSettings system.
 *
 * On GTK+ this will also allow us to discard the GDK_BUTTON_2PRESS and GDK_BUTTON_3PRESS events, so the button press
 * stream will be just like on other platforms.
 *
 * Thanks to mclasen, garnacho_, halfline, and tristan in irc.gimp.net/#gtk+.
 *
 * @param click_counter @p uiprivClickCounter
 * @param button
 * @param x position
 * @param y position
 * @param time delta-time between clicks
 * @param maxTime max delta-time to consider sequential clicks the same event (e.g. double-click)
 * @param xdist difference in current and previous x position
 * @param ydist difference in current and previous y position
 * @return One for single-clicks, 2 for double clicks, and possibly 3 or more for triple, quadruple, etc.
 */
API int uiprivClickCounterClick (uiprivClickCounter *click_counter, int button, int x, int y, uintptr_t time,
                                 uintptr_t maxTime, int32_t xdist, int32_t ydist);

/**
 * @brief Resets the given click counter.
 * @param click_counter @p uiprivClickCounter
 */
API void uiprivClickCounterReset (uiprivClickCounter *click_counter);

/**
 * @brief Converts system-dependent scancodes to LibUI equivilent key codes, storing the result in the given
 * @p uiAreaKeyEvent.
 *
 * For position independence across international keyboard layouts, typewriter keys are read using scancodes (which
 * are always set to @p 1).
 *
 * Windows provides the scancodes directly in the @p LPARAM. GTK+ provides the scancodes directly from the underlying
 * window system via @p GdkEventKey.hardware_keycode.
 *
 * On X11, this is @code scancode + 8 @endcode (because X11 keyboard codes have a range of [8,255]). Wayland is
 * guaranteed to give the same result. On Linux, where evdev is used instead of polling scancodes directly from the
 * keyboard, evdev's typewriter section key code constants are the same as scancodes anyway, so the rules above apply.
 * Typewriter section scancodes are the same across international keyboards with some exceptions that have been
 * accounted for (see KeyEvent's documentation); see http://www.quadibloc.com/comp/scan.htm for details. Non-typewriter
 * keys can be handled safely using constants provided by the respective backend API.
 *
 * Because GTK+ keysyms may or may not obey Num Lock, we also handle the keys 0-9 and . on the numeric keypad with
 * scancodes (they match too).
 *
 * @param scancode system-dependent scancode.
 * @param key_event LibUI key event to update.
 * @returns @p 1 when the given scancode is properly decoded.
 */
API int uiprivFromScancode (uintptr_t scancode, uiAreaKeyEvent *key_event);
