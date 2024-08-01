#pragma once

/**
 * @brief Program options.
 * @see getopt(3)
 */
#define UI_TEST_OPTIONS_GETOPT_STR "n:fhsv"

/**
 * @brief Formatted message for the command-line option usage help line.
 */
#define UI_TEST_OPTIONS_GETOPT_FMT "[-n name] [-fhsv]"

/**
 * @brief Command-line options help text.
 */
#define UI_TEST_OPTIONS_HELP_STR                                                                                      \
  "\n"                                                                                                                \
  "Options:\n"                                                                                                        \
  "    -n [name]\n"                                                                                                   \
  "       Runs all the tests that match the given name (exact match).\n"                                              \
  "\n"                                                                                                                \
  "    -v\n"                                                                                                          \
  "       Shows verbose messages, including backtraces for tests that pass.\n"                                        \
  "       Takes presedence over -s\n"                                                                                 \
  "\n"                                                                                                                \
  "   -s\n"                                                                                                           \
  "       Silences backtrace messages - ignored when -v is used.\n"                                                   \
  "\n"                                                                                                                \
  "   -f\n"                                                                                                           \
  "       Forces all skipped tests to run.\n"                                                                         \
  "\n"                                                                                                                \
  "   -h\n"                                                                                                           \
  "       Shows this help message.\n"                                                                                 \
  "\n"

/**
 * @brief Command-line arguments.
 */
struct ui_test_options_t
{
  /**
   * @brief Optionally specified test name.
   * @details When not @p NULL, only tests with this name will run.
   * @remark Command line option: `-n [name]`
   */
  const char *name;

  /**
   * @brief Forces all skipped tests to run when non-zero.
   * @remark Command line option: `-f`
   */
  int force_skipped_tests;

  /**
   * @brief Backtrace messages will be silenced when non-zero.
   * @remark Command line option: `-s`
   */
  int silence_backtraces;

  /**
   * @brief Will include backtraces for all tests, even those that pass.
   * @remark Takes precedence over the @p silence_backtrace option.
   * @remark Command line option: `-v`
   */
  int verbose;
};
