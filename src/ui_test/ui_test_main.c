#include <ui_test.h>
#include <ui_test_main.h>
#include <ui_test_options.h>
#include <ui_test_report.h>

#include "ui_test_console_reporter.h"

#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

extern int   errno;  /// @see errno(3)
extern char *optarg; /// @see getopt(3)
extern int   opterr; /// @see getopt(3)
extern int   optind; /// @see getopt(3)

int
main (const int argc, char **argv)
{
  int opt;

  struct ui_test_options_t options = { 0 };

  opterr = 0;

  while ((opt = getopt (argc, argv, UI_TEST_OPTIONS_GETOPT_STR)) != -1)
    switch (opt)
      {
      case 'f':
        options.force_skipped_tests = 1;
        break;

      case 'n':
        options.name = optarg;
        break;

      case 's':
        options.silence_backtraces = 1;
        break;

      case 'v':
        options.verbose = 1;

      case 'h':
        ui_test_unit_main_print_usage (basename (argv[0]), stdout);
        exit (EXIT_SUCCESS);

      default:;
        ui_test_unit_main_print_usage (basename (argv[0]), stderr);
        exit (EXIT_FAILURE);
      }

  static ui_test_report_cb_t *console_report_cb = ui_test_console_reporter;
  ui_test_report_cb (&console_report_cb);

  if (options.name != NULL)
    ui_test_run_by_name (options.name, &options);

  else
    ui_test_run_all (&options);

  ui_test_console_reporter_print_summary ();

  ui_test_report_cb (NULL);

  return EXIT_SUCCESS;
}

void
ui_test_unit_main_print_usage (char *progname, FILE *out)
{
  if (progname == NULL)
    progname = UI_TEST_MAIN_DEFAULT_PROGNAME;

  (void)fprintf (                 // Example output:
      out,                        //
      "Usage: %s %s\n"            // Usage: ui_test [-h]
      "%s\n"                      // Runs one or more unit tests.
      "\n"                        //
      "Options:\n"                // Options:
      "%s\n",                     //     -h
      progname,                   //        Prints this help message
      UI_TEST_OPTIONS_GETOPT_FMT, //
      UI_TEST_MAIN_DESCRIPTION,   //
      UI_TEST_OPTIONS_HELP_STR    //
  );
}
