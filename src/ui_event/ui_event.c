#include <ui_event.h>
#include <ui_test_expect.h>

#include "ui_event_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ui_event_t
ui_event_init (const unsigned long id, const intptr_t data)
{
  struct timeval timestamp = { 0, 0 };

  if (gettimeofday (&timestamp, NULL) != 0)
    ui_event_perror (errno);

  return (struct ui_event_t){ id, timestamp, data };
}

static ui_test_case
ui_event_init_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_init_test);

  const unsigned long id   = 99;
  const char         *data = "Event Test";

  const struct ui_event_t event = ui_event_init (id, (intptr_t)data);

  ui_expect_cmp (long, event.id, is, id);
  ui_expect_cmp (long, event.timestamp.tv_sec, is_not, 0);
  ui_expect_cmp (long, event.timestamp.tv_usec, is_not, 0);
  ui_expect_cmp (str, (const char *)event.data, is, data);
}
