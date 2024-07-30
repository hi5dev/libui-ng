#include "unit.h"

#include <ui/checkbox.h>

#define uiCheckboxPtrFromState(s) uiControlPtrFromState (uiCheckbox, s)
#define checkboxUnitTest(f)       cmocka_unit_test_setup_teardown ((f), unitTestSetup, unitTestTeardown)

static void
checkboxNew (void **state)
{
  uiCheckbox **c = uiCheckboxPtrFromState (state);

  *c = uiNewCheckbox ("Text");
}

static void
checkboxCheckedDefault (void **state)
{
  uiCheckbox **c = uiCheckboxPtrFromState (state);

  *c = uiNewCheckbox ("Text");
  assert_int_equal (uiCheckboxChecked (*c), 0);
}

static void
checkboxSetChecked (void **state)
{
  uiCheckbox **c = uiCheckboxPtrFromState (state);

  *c = uiNewCheckbox ("Text");
  uiCheckboxSetChecked (*c, 1);
  assert_int_equal (uiCheckboxChecked (*c), 1);
  uiCheckboxSetChecked (*c, 0);
  assert_int_equal (uiCheckboxChecked (*c), 0);
}

static void
checkboxTextDefault (void **state)
{
  uiCheckbox **c    = uiCheckboxPtrFromState (state);
  const char  *text = "Text";

  *c       = uiNewCheckbox (text);
  char *rv = uiCheckboxText (*c);
  assert_string_equal (text, rv);
  uiFreeText (rv);
}

static void
checkboxSetText (void **state)
{
  uiCheckbox **c    = uiCheckboxPtrFromState (state);
  const char  *text = "SetText";

  *c = uiNewCheckbox ("Text");
  uiCheckboxSetText (*c, text);
  char *rv = uiCheckboxText (*c);
  assert_string_equal (text, rv);
  uiFreeText (rv);
}

static void
onToggledNoCall (uiCheckbox *, void *)
{
  function_called ();
}

static void
checkboxSetCheckedNoCallback (void **state)
{
  uiCheckbox **c = uiCheckboxPtrFromState (state);

  *c = uiNewCheckbox ("Text");
  uiCheckboxOnToggled (*c, onToggledNoCall, NULL);
  uiCheckboxSetChecked (*c, 1);
  uiCheckboxSetChecked (*c, 0);
}

int
checkboxRunUnitTests (void)
{
  const struct CMUnitTest tests[] = {
    checkboxUnitTest (checkboxNew),        checkboxUnitTest (checkboxCheckedDefault),
    checkboxUnitTest (checkboxSetChecked), checkboxUnitTest (checkboxTextDefault),
    checkboxUnitTest (checkboxSetText),    checkboxUnitTest (checkboxSetCheckedNoCallback),
  };

  return cmocka_run_group_tests_name ("uiCheckbox", tests, unitTestsSetup, unitTestsTeardown);
}
