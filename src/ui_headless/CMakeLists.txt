cmake_minimum_required (VERSION 3.25)

add_library (ui_headless STATIC)

add_library (ui::headless ALIAS ui_headless)

target_link_libraries (
  ui_headless

  PRIVATE
  ui::headless::interface
  ui::test
)

target_sources (
  ui_headless

  PRIVATE
  ui_headless.c
  ui_headless_main.c
)

if (UI_CTEST_TARGETS)
  add_test (NAME "ui_headless_main_test" COMMAND ui::test -n ui_headless_main_test)
  add_test (NAME "ui_headless_test" COMMAND ui::test -n ui_headless_test)
endif ()
