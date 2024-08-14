cmake_minimum_required (VERSION 3.25)

project (
  ui_event

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  DESCRIPTION
  "Default implementation of ui_event."

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui_event STATIC)

add_library (ui::event ALIAS ui_event)

target_link_libraries (ui_event PRIVATE ui::event::interface ui::test)

target_sources (
  ui_event

  PRIVATE FILE_SET HEADERS

  FILES
  ui_event_error.h
  ui_event_thread.h
)

target_sources (
  ui_event

  PRIVATE
  ui_event.c
  ui_event_consumer.c
  ui_event_dispatcher.c
  ui_event_error.c
  ui_event_list.c
  ui_event_producer.c
  ui_event_test.c
  ui_event_thread.c
)

ui_test (
  ui_event

  TEST_CASES

  # ui_event_t
  ui_event_cdtor_test
  ui_event_test
  ui_event_update_timestamp_test

  # ui_event_dispatcher_t
  ui_event_dispatcher_cdtor_test

  # ui_event_list_t
  ui_event_list_test
)
