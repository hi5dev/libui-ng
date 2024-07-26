#include "unit.h"

#include <ui/button.h>

#define uiButtonPtrFromState(s) uiControlPtrFromState (uiButton, s)
#define buttonUnitTest(f)       cmocka_unit_test_setup_teardown ((f), unitTestSetup, unitTestTeardown)

static void
buttonNew (void **state)
{
  uiButton **b = uiButtonPtrFromState (state);

  *b = uiNewButton ("Text");
}

static void
buttonTextDefault (void **state)
{
  uiButton  **b    = uiButtonPtrFromState (state);
  const char *text = "Text";

  *b       = uiNewButton (text);
  char *rv = uiButtonText (*b);
  assert_string_equal (rv, text);
  uiFreeText (rv);
}

static void
buttonSetText (void **state)
{
  uiButton  **b       = uiButtonPtrFromState (state);
  const char *text    = "Text";
  const char *setText = "setText";

  *b = uiNewButton (text);
  uiButtonSetText (*b, setText);
  char *rv = uiButtonText (*b);
  assert_string_equal (rv, setText);
  uiFreeText (rv);
}

int
buttonRunUnitTests (void)
{
  const struct CMUnitTest tests[] = {
    buttonUnitTest (buttonNew),
    buttonUnitTest (buttonTextDefault),
    buttonUnitTest (buttonSetText),
  };

  return cmocka_run_group_tests_name ("uiButton", tests, unitTestsSetup, unitTestsTeardown);
}
