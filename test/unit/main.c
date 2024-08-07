#include "unit.h"

#include <ui/init.h>
#include <ui/main.h>
#include <ui/window.h>

#include <stdio.h>
#include <ui/control.h>

int
unitWindowOnClosingQuit (uiWindow *, void *)
{
  uiQuit ();
  return 1;
}

int
unitTestsSetup (void **state)
{
  *state = malloc (sizeof (struct state));
  assert_non_null (*state);
  return 0;
}

int
unitTestsTeardown (void **state)
{
  free (*state);
  return 0;
}

int
unitTestSetup (void **_state)
{
  struct state *state = *_state;
  uiInitOptions o     = { 0 };

  assert_no_error (uiInit (&o));
  state->w = uiNewWindow ("Unit Test", UNIT_TEST_WINDOW_WIDTH, UNIT_TEST_WINDOW_HEIGHT, 0);
  uiWindowOnClosing (state->w, unitWindowOnClosingQuit, NULL);
  return 0;
}

int
unitTestTeardown (void **_state)
{
  struct state *state = *_state;

  uiWindowSetChild (state->w, uiControl (state->c));
  uiControlShow (uiControl (state->w));

  uiMainSteps ();
  uiMainStep (1);
  uiControlDestroy (uiControl (state->w));
  uiUninit ();
  return 0;
}

struct unitTest
{
  int (*fn) (void);
};

int
main (void)
{
  int                   failedTests      = 0;
  int                   failedComponents = 0;
  const struct unitTest unitTests[]      = {
    { initRunUnitTests },         { menuRunUnitTests },   { sliderRunUnitTests },      { spinboxRunUnitTests },
    { labelRunUnitTests },        { buttonRunUnitTests }, { comboboxRunUnitTests },    { checkboxRunUnitTests },
    { radioButtonsRunUnitTests }, { entryRunUnitTests },  { progressBarRunUnitTests }, { drawMatrixRunUnitTests },
  };

  for (size_t i = 0; i < sizeof (unitTests) / sizeof (*unitTests); ++i)
    {
      const int fails = (unitTests[i].fn) ();
      failedTests += fails;
      if (fails > 0)
        failedComponents++;
    }

  puts ("[==========]");
  if (failedTests == 0)
    puts ("[  PASSED  ] All test(s) in all component(s).");
  else
    printf ("[  FAILED  ] %d test(s) in %d component(s), see above.\n", failedTests, failedComponents);

  return failedTests;
}
