#pragma once

#ifndef UI_TEST_MAIN_DEFAULT_PROGNAME
/**
 * @brief Default program name to use when the platform's command-line doesn't pass it in to the argument vector.
 */
#define UI_TEST_MAIN_DEFAULT_PROGNAME "ui_test"
#endif

/**
 * @brief Program options.
 * @see getopt(3)
 */
#define UI_TEST_MAIN_OPTSTR "n:h"

/**
 * @brief Usage formatted message.
 */
#define UI_TEST_MAIN_USAGE_FMT "%s [-n name] [-h]"

/**
 * @brief Test runner formatted message.
 */
#define TEST_RUN_FMT "Running %s ..."

/**
 * @brief Message to print when the test passes.
 */
#define TEST_PASS "[PASS]"

/**
 * @brief Message to print when the test fails.
 */
#define TEST_FAIL "[FAIL]"

/**
 * @brief Command-line arguments.
 */
struct ui_test_main_options_t
{
  /**
   * @brief Optionally specified test name.
   * @details When not @p NULL, only tests with this name will run.
   */
  const char *name;
};

/**
 * @see errno(3)
 */
extern int errno;

/**
 * @see getopt(3)
 */
extern char *optarg;

/**
 * @see getopt(3)
 */
extern int opterr;

/**
 * @see getopt(3)
 */
extern int optind;

/**
 * @brief Prints the program's usage and exits with a failure.
 * @param progname name of the executable.
 */
void ui_test_unit_main_print_usage (char *progname);

/**
 * @brief The unit test runner's main-entrypoint.
 * @param argc Command-line argument count.
 * @param argv Command-line argument vector.
 * @return non-zero on failure.
 */
int ui_test_unit_main (int argc, char **argv);
