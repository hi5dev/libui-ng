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
      - [ui_event_list](./ui_event_list.h)
      - [ui_event_producer](./ui_event_producer.h)

### Overview

[Producers](./ui_event_producer.h) generate events, and [consumers](./ui_event_consumer.h) handle them, while
[dispatchers](./ui_event_dispatcher.h) manage the flow of the events, ensuring that producers do not overwhelm the
consumers.

### Example

```c++
```
