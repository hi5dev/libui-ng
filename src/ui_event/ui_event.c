#include <ui_event.h>
#include <ui_test_expect.h>
#include <ui_test_time.h>

#include "ui_event_error.h"

#include <assert.h>
#include <stdlib.h>

struct ui_event_t *
ui_event_create (void)
{
  struct ui_event_t *event = calloc (1, sizeof (*event));

  if (event == NULL)
    ui_event_perrno ();

  else
    ui_event_update_timestamp (event);

  return event;
}

void
ui_event_destroy (struct ui_event_t *event)
{
  free (event);
}

static ui_test_case
ui_event_cdtor_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_cdtor_test);

  struct ui_event_t *event = ui_event_create ();

  ui_expect_not_null (event);

  ui_expect_null (event->data);
  ui_expect_null (event->event_type);
  ui_expect_null (event->object);
  ui_expect_null (event->producer);

  ui_expect (event->timestamp.tv_sec > 0, "timestamp should be positive");
  ui_expect (event->timestamp.tv_usec > 0, "timestamp should be positive");

  ui_event_destroy (event);
}

void
ui_event_update_timestamp (struct ui_event_t *event)
{
  assert (event != NULL);

  if (gettimeofday (&event->timestamp, NULL) != 0)
    ui_event_perrno ();
}

static ui_test_case
ui_event_update_timestamp_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_update_timestamp_test);

  struct ui_event_t *event = ui_event_create ();
  ui_expect_not_null (event);

  const long tv_usec_before = event->timestamp.tv_usec;

  ui_test_msleep (1);
  ui_event_update_timestamp (event);
  const long tv_usec_after = event->timestamp.tv_usec;

  ui_event_destroy (event);

  ui_expect (tv_usec_after > tv_usec_before, "timestamp should update");
}
