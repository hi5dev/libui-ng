#include "unit.h"

#include <ui/init.h>

#include <assert.h>

static void
initUninit (void **)
{
  uiInitOptions o = { 0 };
  assert_no_error (uiInit (&o));
  uiUninit ();
}

static void
initUninitTwice (void **)
{
  uiInitOptions o = { 0 };

  assert_no_error (uiInit (&o));
  uiUninit ();

  assert_no_error (uiInit (&o));
  uiUninit ();
}

int
initRunUnitTests (void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (initUninit),
    cmocka_unit_test (initUninitTwice),
  };

  return cmocka_run_group_tests_name ("uiInit", tests, NULL, NULL);
}
