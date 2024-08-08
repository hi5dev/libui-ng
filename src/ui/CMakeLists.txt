cmake_minimum_required (VERSION 3.25)

project (
  ui

  DESCRIPTION
  "Platform-independent implementation of LibUI"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui STATIC)

target_link_libraries (
  ui

  PRIVATE
  ui::test

  PUBLIC
  ui::interface
)

target_sources (
  ui

  PRIVATE
  ui_control.c
  ui_event.c
  ui_main.c
  ui_window.c
)

ui_test (
  ui

  TEST_CASES
  ui_control_test_create
  ui_event_test
  ui_window_borderless_test
  ui_window_cdtor_test
  ui_window_client_size_test
  ui_window_focus_test
  ui_window_fullscreen_test
  ui_window_position_test
  ui_window_resizable_test
  ui_window_title_test
  ui_window_visibility_test
)
