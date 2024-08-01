#pragma once

#include <stdio.h>

/**
 * @brief Default program name to use when the platform's command-line doesn't pass it in to the argument vector.
 */
#define UI_TEST_MAIN_DEFAULT_PROGNAME "ui_test"

/**
 * @brief Short description of the program shown to the user in the program's help message.
 */
#define UI_TEST_MAIN_DESCRIPTION "Runs one or more tests"

/**
 * @brief Prints the program's usage to the given stream.
 * @param progname name of the executable.
 * @param out stream to write the message to.
 */
void ui_test_main_print_usage (char *progname, FILE *out);

/**
 * @brief Runs the tests.
 * @param argc Command-line argument count.
 * @param argv Command-line argument vector.
 * @return non-zero on failure.
 */
int ui_test_main (int argc, char **argv);
