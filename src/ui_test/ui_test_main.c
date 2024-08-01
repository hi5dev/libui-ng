#include <ui_test.h>
#include <ui_test_main.h>
#include <ui_test_options.h>
#include <ui_test_report_stdout.h>

#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef UI_TEST_MAIN

int
main (const int argc, char **argv)
{
  return ui_test_main (argc, argv);
}

#endif

int
ui_test_main (const int argc, char **argv)
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
        break;

      case 'h':
        ui_test_main_print_usage (basename (argv[0]), stdout);
        exit (EXIT_SUCCESS);

      default:;
        ui_test_main_print_usage (basename (argv[0]), stderr);
        exit (EXIT_FAILURE);
      }

  struct ui_test_report_t report = { 0 };
  report.n_tests_registered      = ui_test_n_registered ();
  report.report_cb               = ui_test_report_stdout_cb;

  if (options.name != NULL)
    ui_test_run_by_name (options.name, &report, &options);

  else
    ui_test_run_all (&report, &options);

  ui_test_report_stdout_print_summary (&report);

  if (report.n_tests_run > 0 && report.n_tests_passed + report.n_tests_skipped == report.n_tests_run)
    return EXIT_SUCCESS;

  return EXIT_FAILURE;
}

void
ui_test_main_print_usage (char *progname, FILE *out)
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
