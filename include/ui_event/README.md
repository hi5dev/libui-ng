# Event API

API for a publisher/subscriber event notification system.

## [Index](../../README.md)
- [APIs](../README.md)
  - **[Event API](./README.md)**
    - [Overview](#overview)
    - [Example](#example)
    - **API**
      - [ui_event](./ui_event.h)
      - [ui_event_consumer](./ui_event_consumer.h)
      - [ui_event_dispatcher](./ui_event_dispatcher.h)
      - [ui_event_producer](./ui_event_producer.h)

### Overview

[Producers](./ui_event_producer.h) generate events, and [consumers](./ui_event_consumer.h) handle them, while
[dispatchers](./ui_event_dispatcher.h) manage the flow of the events, ensuring that producers do not overwhelm the
consumers.

### Example

```c++
enum example_producers
{
  BUTTON,
  WINDOW,
};

enum example_events
{
  CLICK,
  FOCUS,
  MOVE,
};

void on_click (ui_event_t *event, intptr_t data);
void on_drag (ui_event_t *event, intptr_t data);
void on_focus (ui_event_t *event, intptr_t data);

void
example (void)
{
  struct ui_event_dispatcher_t *dispatcher = ui_event_dispatcher_create ();

  // register button events
  ui_event_register (dispatcher, BUTTON, CLICK);
  ui_event_register (dispatcher, BUTTON, FOCUS);

  // register window events - note that the same event IDs can be used for any producer
  ui_event_register (dispatcher, WINDOW, CLICK);
  ui_event_register (dispatcher, WINDOW, FOCUS);
  ui_event_register (dispatcher, WINDOW, MOVE);

  // subscribe to button events
  ui_event_subscribe (dispatcher, BUTTON, CLICK, on_click);
  ui_event_subscribe (dispatcher, BUTTON, FOCUS, on_focus);

  // subscribe to window events - note that the same callbacks can be used for any producer
  ui_event_subscribe (dispatcher, WINDOW, CLICK, on_click);
  ui_event_subscribe (dispatcher, WINDOW, FOCUS, on_focus);
  ui_event_subscribe (dispatcher, WINDOW, MOVE, on_move);

  //
  ui_struct_window_t *window = ui_window_create ("Example", 854, 480);
  ui_event_notify (dispatcher, WINDOW, FOCUS, main_window);

  // this is just an example, don't actually do this
  // dispatch the events in the main loop
  while (running)
    ui_event_dispatch (dispatcher);

  ui_event_dispatcher_destroy (dispatcher);
}

```

### Consumer Example
