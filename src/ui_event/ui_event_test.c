#include <ui_test_expect.h>

#include <ui_event.h>
#include <ui_event_consumer.h>
#include <ui_event_dispatcher.h>
#include <ui_event_producer.h>
#include <ui_event_type.h>

#include <stdlib.h>

ui_event_producer (ui_event_test_producer1);
ui_event_producer (ui_event_test_producer2);

ui_event_type (ui_event_test_event_type1);
ui_event_type (ui_event_test_event_type2);

struct ui_event_test_t
{
  struct ui_event_dispatcher_t *dispatcher;
};

static ui_test_case
ui_event_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_test);

  struct ui_event_test_t *event_test = calloc (1, sizeof (*event_test));
  ui_expect_not_null (event_test);

  event_test->dispatcher = ui_event_dispatcher_create ();
  if (event_test->dispatcher == NULL)
    free (event_test);
  ui_expect_not_null (event_test->dispatcher);



  ui_event_dispatcher_destroy (event_test->dispatcher);
  free (event_test);
}
