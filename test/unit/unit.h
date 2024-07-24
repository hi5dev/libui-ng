#ifndef __LIBUI_UNIT_H__
#define __LIBUI_UNIT_H__

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include <cmocka.h>

#include <stdio.h>
#include <stdlib.h>

#include "ui.h"

#define assert_no_error(stmt)                                                                                         \
  do                                                                                                                  \
    {                                                                                                                 \
      const char *error = ({ stmt; });                                                                                \
      if (error != NULL)                                                                                              \
        (void)fprintf (stderr, "%s:%d: %s\n", __FILE_NAME__, __LINE__, error);                                        \
      assert_null (error);                                                                                            \
    }                                                                                                                 \
  while (0)

/**
 * Unit test run functions.
 */
int initRunUnitTests (void);
int sliderRunUnitTests (void);
int spinboxRunUnitTests (void);
int labelRunUnitTests (void);
int buttonRunUnitTests (void);
int comboboxRunUnitTests (void);
int checkboxRunUnitTests (void);
int radioButtonsRunUnitTests (void);
int entryRunUnitTests (void);
int menuRunUnitTests (void);
int progressBarRunUnitTests (void);
int drawMatrixRunUnitTests (void);

/**
 * Helper for general setup/teardown of controls embedded in a window.
 */
struct state
{
  uiWindow  *w;
  uiControl *c;
};

int unitWindowOnClosingQuit (uiWindow *w, void *data);

#define UNIT_TEST_WINDOW_WIDTH  300
#define UNIT_TEST_WINDOW_HEIGHT 200

/**
 * Helper for setting up the state variable used in unit tests.
 */
int unitTestsSetup (void **state);

/**
 * Helper for tearing down unitTestsSetup().
 */
int unitTestsTeardown (void **state);

/**
 * Helper for creating a window used in unit tests and displaying a control.
 *
 * Use #uiControlPtrFromState to assign your test control:
 *
 * uiControl **c uiControlPtrFromState(uiControl, state);
 */
int unitTestSetup (void **state);

/**
 * Helper for tearing down unitTestSetup().
 */
int unitTestTeardown (void **state);

#define uiControlPtrFromState(t, s) (t **)&(((struct state *)*(s))->c)

#endif
