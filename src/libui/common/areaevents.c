#include "areaevents.h"

/**
 * @brief Maps system scan-codes to LibUI equivilent key-codes.
 */
static const struct
{
  /**
   * @brief System scan-code.
   */
  uintptr_t scancode;

  /**
   * @brief LibUI equivilent keycode.
   */
  char equiv;
} scancodeKeys[] = {
  { 0x02,   '1'  },
  { 0x03,   '2'  },
  { 0x04,   '3'  },
  { 0x05,   '4'  },
  { 0x06,   '5'  },
  { 0x07,   '6'  },
  { 0x08,   '7'  },
  { 0x09,   '8'  },
  { 0x0A,   '9'  },
  { 0x0B,   '0'  },
  { 0x0C,   '-'  },
  { 0x0D,   '='  },
  { 0x0E,   '\b' },
  { 0x0F,   '\t' },
  { 0x10,   'q'  },
  { 0x11,   'w'  },
  { 0x12,   'e'  },
  { 0x13,   'r'  },
  { 0x14,   't'  },
  { 0x15,   'y'  },
  { 0x16,   'u'  },
  { 0x17,   'i'  },
  { 0x18,   'o'  },
  { 0x19,   'p'  },
  { 0x1A,   '['  },
  { 0x1B,   ']'  },
  { 0x1C,   '\n' },
  { 0x1E,   'a'  },
  { 0x1F,   's'  },
  { 0x20,   'd'  },
  { 0x21,   'f'  },
  { 0x22,   'g'  },
  { 0x23,   'h'  },
  { 0x24,   'j'  },
  { 0x25,   'k'  },
  { 0x26,   'l'  },
  { 0x27,   ';'  },
  { 0x28,   '\'' },
  { 0x29,   '`'  },
  { 0x2B,   '\\' },
  { 0x2C,   'z'  },
  { 0x2D,   'x'  },
  { 0x2E,   'c'  },
  { 0x2F,   'v'  },
  { 0x30,   'b'  },
  { 0x31,   'n'  },
  { 0x32,   'm'  },
  { 0x33,   ','  },
  { 0x34,   '.'  },
  { 0x35,   '/'  },
  { 0x39,   ' '  },
  { 0xFFFF, 0    },
};

/**
 * @brief Maps extended system scan-codes to LibUI.
 */
static const struct
{
  /**
   * @brief System scan-code.
   */
  uintptr_t scancode;

  /**
   * @brief LibUI equivilent keycode.
   */
  uiExtKey equiv;
} scancodeExtKeys[] = {
  { 0x47,   uiExtKeyN7   },
  { 0x48,   uiExtKeyN8   },
  { 0x49,   uiExtKeyN9   },
  { 0x4B,   uiExtKeyN4   },
  { 0x4C,   uiExtKeyN5   },
  { 0x4D,   uiExtKeyN6   },
  { 0x4F,   uiExtKeyN1   },
  { 0x50,   uiExtKeyN2   },
  { 0x51,   uiExtKeyN3   },
  { 0x52,   uiExtKeyN0   },
  { 0x53,   uiExtKeyNDot },
  { 0xFFFF, 0            },
};

int
uiprivClickCounterClick (uiprivClickCounter *click_counter, const int button, const int x, const int y,
                         const uintptr_t time, const uintptr_t maxTime, const int32_t xdist, const int32_t ydist)
{
  // do not count if buttons are different from before
  if (button != click_counter->curButton)
    click_counter->count = 0;

  // do not count when outside the double-click region
  if (x < click_counter->rectX0 || y < click_counter->rectY0 || x >= click_counter->rectX1
      || y >= click_counter->rectY1)
    click_counter->count = 0;

  // ensure time between clicks is fast enough to be considered a double-click
  if (time - click_counter->prevTime > maxTime)
    click_counter->count = 0;

  // accumulate click counts based on stats above and previous tests
  click_counter->count++;

  // update the internal structures for the next test
  click_counter->curButton = button;
  click_counter->prevTime  = time;
  click_counter->rectX0    = x - xdist;
  click_counter->rectY0    = y - ydist;
  click_counter->rectX1    = x + xdist;
  click_counter->rectY1    = y + ydist;

  return click_counter->count;
}

void
uiprivClickCounterReset (uiprivClickCounter *click_counter)
{
  click_counter->curButton = 0;
  click_counter->rectX0    = 0;
  click_counter->rectY0    = 0;
  click_counter->rectX1    = 0;
  click_counter->rectY1    = 0;
  click_counter->prevTime  = 0;
  click_counter->count     = 0;
}

int
uiprivFromScancode (const uintptr_t scancode, uiAreaKeyEvent *key_event)
{
  int i;

  for (i = 0; scancodeKeys[i].scancode != 0xFFFF; i++)
    if (scancodeKeys[i].scancode == scancode)
      {
        key_event->Key = scancodeKeys[i].equiv;
        return 1;
      }

  for (i = 0; scancodeExtKeys[i].scancode != 0xFFFF; i++)
    if (scancodeExtKeys[i].scancode == scancode)
      {
        key_event->ExtKey = scancodeExtKeys[i].equiv;
        return 1;
      }

  return 0;
}
