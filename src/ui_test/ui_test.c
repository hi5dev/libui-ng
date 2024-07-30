#include <ui_test.h>

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Array of unit tests.
 */
static struct ui_test_t ui_test_registry[UI_TEST_REGISTRY_CAPACITY];

struct ui_test_reporter_t *ui_test_get_reporter (void);

/**
 * @brief Total number of unit tests registered.
 */
static int test_count = 0;


void
register_test (const struct test_register_params params)
{
  assert (test_count < MAX_TESTS);

  assert (params.name != NULL);
  assert (params.name[0] != '\0');
  assert (params.f != NULL);

  test_registry[test_count].name     = params.name;
  test_registry[test_count].f        = params.f;
  test_registry[test_count].filename = params.filename;
  test_registry[test_count].line     = params.line;

  test_count++;
}

void
run_all_tests (void)
{
  for (int i = 0; i < test_count; i++)
    run_test (&test_registry[i]);
}

void
run_test (struct test_t *test)
{
  assert (test != NULL);

  (void)fprintf (stdout, TEST_RUN_FMT, test->name);

  test->failure = NULL;

  test->f (test);

  if (test->failure == NULL)
    {
      (void)fprintf (stdout, " %s\n", TEST_PASS);
      return;
    }

  (void)fprintf (stdout, " %s\n", TEST_FAIL);
  (void)fprintf (stderr, "  %s\n\n", test->failure);
}

void
run_tests_by_name (const char *name)
{
  for (int i = 0; i < test_count; i++)
    if (strcmp (name, test_registry[i].name) == 0)
      run_test (&test_registry[i]);
}

void
usage (char *progname)
{
  fprintf (stderr, USAGE_FMT, progname ? progname : DEFAULT_PROGNAME);

  exit (EXIT_FAILURE);
}

void ui_test_set_reporter (struct ui_test_reporter_t *reporter);
