cmake_minimum_required (VERSION 3.25)

add_executable (ui_test)

add_executable (ui::test ALIAS ui_test)

target_link_libraries (
  ui_test

  PRIVATE
  ui::test::interface
)

target_sources (
  ui_test

  PRIVATE
  ui_test.c
  ui_test_expect.c
  ui_test_main.c
  ui_test_report.c
)
