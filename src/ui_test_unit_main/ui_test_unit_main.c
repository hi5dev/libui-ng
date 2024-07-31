#include <ui_test.h>
#include <ui_test_unit_main.h>

#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

void
usage (char *progname)
{
  (void)fprintf (stderr, UI_TEST_MAIN_USAGE_FMT, progname ? progname : UI_TEST_MAIN_DEFAULT_PROGNAME);

  exit (EXIT_FAILURE);
}

int
main (const int argc, char **argv)
{
  int opt;

  struct ui_test_main_options_t options = {};

  opterr = 0;

  while ((opt = getopt (argc, argv, UI_TEST_MAIN_USAGE_FMT)) != -1)
    switch (opt)
      {
      case 'n':
        {
          options.name = optarg;
          break;
        }

      case 'h':
        __attribute__ ((fallthrough));

      default:;
        {
          usage (basename (argv[0]));
          break;
        }
      }

  if (options.name != NULL)
    ui_test_run_by_name (options.name);

  else
    ui_test_run_all ();

  return EXIT_SUCCESS;
}
