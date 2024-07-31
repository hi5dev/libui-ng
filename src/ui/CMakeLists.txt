cmake_minimum_required (VERSION 3.25)

add_library (ui STATIC)

target_link_libraries (
  ui

  PRIVATE
  ui::interface
)

target_sources (
  ui

  PRIVATE
  ui_event.c
  ui_main.c
)

if (TARGET ui_test)
  target_link_libraries (ui_test PRIVATE ui)

  target_sources (
    ui_test

    PRIVATE
    ui_event_test.c
    ui_main_test.c
  )

  add_test (NAME "ui_event_test" COMMAND ui::test -n ui_event_test)
  add_test (NAME "ui_main_test" COMMAND ui::test -n ui_main_test)
endif ()
