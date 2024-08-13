cmake_minimum_required (VERSION 3.25)

project (
  ui_event_interface

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  DESCRIPTION
  "A publisher/subscriber event notification system."

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui_event_interface INTERFACE)

add_library (ui::event::interface ALIAS ui_event_interface)

target_sources (
  ui_event_interface

  PUBLIC FILE_SET HEADERS

  FILES
  ui_event.h
  ui_event_consumer.h
  ui_event_dispatcher.h
  ui_event_list.h
  ui_event_producer.h
  ui_event_type.h
)
