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
  ui::interface
  ui::test
)

target_sources (
  ui

  PRIVATE
  ui_control.c
  ui_event.c
  ui_main.c
)

if (TARGET ui::test::runner)
  ui_test_runner (LINK_LIBRARY ui)

  add_test (NAME "ui/test/control" COMMAND ui::test::runner -n ui_control_test_create)
  add_test (NAME "ui/test/event" COMMAND ui::test::runner -n ui_event_test)
  add_test (NAME "ui/test/main" COMMAND ui::test::runner -n ui_main_test)
endif ()
