#include "unit.h"

#include <ui/control.h>
#include <ui/init.h>
#include <ui/main.h>
#include <ui/menu.h>

#define menuUnitTest(f) cmocka_unit_test_setup_teardown ((f), menuTestSetup, menuTestTeardown)

int menuTestSetup (void **state);
int menuTestTeardown (void **_state);

static void
menuNew (void **)
{
  (void)uiNewMenu ("Menu");
}

static void
menuNewInitTwice (void **state)
{
  menuTestSetup (state);

  (void)uiNewMenu ("Menu 1");
  menuTestTeardown (state);

  menuTestSetup (state);
  (void)uiNewMenu ("Menu 2");
  menuTestTeardown (state);
}

static void
menuNewEmptyString (void **)
{
  (void)uiNewMenu ("");
}

static void
menuAppendItem (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendItem (m, "Item");
}

static void
menuAppendItems (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendItem (m, "Item 1");
  uiMenuAppendItem (m, "Item 2");
}

static void
menuAppendCheckItem (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendCheckItem (m, "Item");
}

static void
menuAppendCheckItems (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendCheckItem (m, "Item 1");
  uiMenuAppendCheckItem (m, "Item 2");
}

static void
menuAppendAboutItem (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendAboutItem (m);
}

static void
menuAppendPreferencesItem (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendPreferencesItem (m);
}

static void
menuAppendQuitItem (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendQuitItem (m);
}

static void
menuAppendSeparator (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendSeparator (m);
}

static void
menuAppendFull (void **)
{
  uiMenu *m = uiNewMenu ("Menu");
  uiMenuAppendItem (m, "Item");
  uiMenuAppendSeparator (m);
  uiMenuAppendCheckItem (m, "Check Item");
  uiMenuAppendAboutItem (m);
  uiMenuAppendPreferencesItem (m);
  uiMenuAppendQuitItem (m);
}

static void
menuItemEnable (void **)
{
  uiMenu     *m = uiNewMenu ("Menu");
  uiMenuItem *i = uiMenuAppendItem (m, "Item");
  uiMenuItemEnable (i);
}

static void
menuItemDisable (void **)
{
  uiMenu     *m = uiNewMenu ("Menu");
  uiMenuItem *i = uiMenuAppendItem (m, "Item");
  uiMenuItemDisable (i);
}

static void
menuItemCheckedDefaultFalse (void **)
{
  uiMenu     *m = uiNewMenu ("Menu");
  uiMenuItem *i = uiMenuAppendCheckItem (m, "Item");
  assert_int_equal (uiMenuItemChecked (i), 0);
}

static void
menuItemSetChecked (void **)
{
  uiMenu     *m = uiNewMenu ("Menu");
  uiMenuItem *i = uiMenuAppendCheckItem (m, "Item");

  uiMenuItemSetChecked (i, 1);
  assert_int_equal (uiMenuItemChecked (i), 1);

  uiMenuItemSetChecked (i, 0);
  assert_int_equal (uiMenuItemChecked (i), 0);
}

static void
onClickedNoCall (uiMenuItem *, uiWindow *, void *)
{
  function_called ();
}

static void
menuItemOnClicked (void **)
{
  uiMenu     *m = uiNewMenu ("Menu");
  uiMenuItem *i = uiMenuAppendItem (m, "Item");

  uiMenuItemOnClicked (i, onClickedNoCall, NULL);
}

int
menuTestSetup (void **)
{
  uiInitOptions o = { 0 };

  assert_no_error (uiInit (&o));
  return 0;
}

int
menuTestTeardown (void **_state)
{
  struct state *state = *_state;

  state->w = uiNewWindow ("Menu Test", UNIT_TEST_WINDOW_WIDTH, UNIT_TEST_WINDOW_HEIGHT, 1);
  uiWindowOnClosing (state->w, unitWindowOnClosingQuit, NULL);

  uiControlShow (uiControl (state->w));
  uiMainSteps ();
  uiMainStep (1);

  uiControlDestroy (uiControl (state->w));
  uiUninit ();

  return 0;
}

int
menuRunUnitTests (void)
{
  const struct CMUnitTest tests[] = {
    menuUnitTest (menuNew),
    cmocka_unit_test (menuNewInitTwice),
    menuUnitTest (menuNewEmptyString),
    menuUnitTest (menuAppendItem),
    menuUnitTest (menuAppendItems),
    menuUnitTest (menuAppendCheckItem),
    menuUnitTest (menuAppendCheckItems),
    menuUnitTest (menuAppendAboutItem),
    menuUnitTest (menuAppendPreferencesItem),
    menuUnitTest (menuAppendQuitItem),
    menuUnitTest (menuAppendSeparator),
    menuUnitTest (menuAppendFull),
    menuUnitTest (menuItemEnable),
    menuUnitTest (menuItemDisable),
    menuUnitTest (menuItemCheckedDefaultFalse),
    menuUnitTest (menuItemSetChecked),
    menuUnitTest (menuItemOnClicked),
  };

  return cmocka_run_group_tests_name ("uiMenu", tests, unitTestsSetup, unitTestsTeardown);
}
