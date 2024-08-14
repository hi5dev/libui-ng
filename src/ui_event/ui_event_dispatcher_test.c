#include <ui_event_dispatcher.h>
#include <ui_event_list.h>
#include <ui_event_producer.h>
#include <ui_test_expect.h>

#include <assert.h>

ui_event_producer (t_producer1);
ui_event_producer (t_producer2);

static struct ui_event_dispatcher_test_t
{
  struct ui_event_dispatcher_t *dispatcher;
  struct ui_event_t            *last_event;
  ui_event_cb                  *callback;
  int                           event_count;
} t = { 0 };

static int
ui_event_dispatcher_subscribe_test_callback (struct ui_event_t *event)
{
  assert (event != NULL);
  assert (t.last_event == NULL);

  t.last_event = event;
  t.event_count++;

  return 0;
}

__attribute__ ((constructor)) static void
t_setup (void)
{
  t.dispatcher  = ui_event_dispatcher_create ();
  t.last_event  = NULL;
  t.event_count = 0;
  t.callback    = ui_event_dispatcher_subscribe_test_callback;
}

__attribute__ ((destructor)) static void
t_teardown (void)
{
  ui_event_dispatcher_destroy (t.dispatcher);
}

static ui_test_case
ui_event_dispatcher_ctor_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_dispatcher_ctor_test);

  ui_expect_not_null (t.dispatcher);

  ui_expect_null (t.dispatcher->consumers);
  ui_expect_null (t.dispatcher->events);
  ui_expect_null (t.dispatcher->producers);
}

static ui_test_case
ui_event_dispatcher_subscribe_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_dispatcher_subscribe_test);

  struct ui_event_dispatcher_t *dispatcher    = ui_event_dispatcher_create ();
  const int                     initial_count = ui_event_list_size (dispatcher->consumers);

  ui_event_dispatcher_subscribe (dispatcher, &t_producer1, t.callback, 0, 0);
  const int count_after_subscribing_once = ui_event_list_size (dispatcher->consumers);

  ui_event_dispatcher_subscribe (dispatcher, &t_producer1, t.callback, 0, 0);
  const int count_after_subscribing_twice = ui_event_list_size (dispatcher->consumers);

  ui_event_dispatcher_destroy (dispatcher);

  ui_expect_cmp (int, initial_count, is, 0);
  ui_expect_cmp (int, count_after_subscribing_once, is, 1);
  ui_expect_cmp (int, count_after_subscribing_twice, is, 2);
}

static ui_test_case
ui_event_dispatcher_dispatch_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_dispatcher_dispatch_test);

  ui_test_skip ("work in progress");
}
