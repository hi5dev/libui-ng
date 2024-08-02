cmake_minimum_required (VERSION 3.25)

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

if (UI_CTEST_TARGETS)
  add_test (NAME "ui_control_test" COMMAND ui::test -n ui_control_test)
  add_test (NAME "ui_event_test" COMMAND ui::test -n ui_event_test)
  add_test (NAME "ui_main_test" COMMAND ui::test -n ui_main_test)
endif ()
