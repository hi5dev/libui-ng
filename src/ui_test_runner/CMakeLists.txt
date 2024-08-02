cmake_minimum_required (VERSION 3.25)

add_executable (ui_test_runner)

add_executable (ui::test::runner ALIAS ui_test_runner)

target_link_libraries (
  ui_test_runner

  PRIVATE
  ui::test
  ui::ui
)

target_link_options (
  ui_test_runner

  PRIVATE
  -Wl,--whole-archive
  "$<TARGET_FILE:ui::ui>"
  "$<TARGET_FILE:ui>"
  -Wl,--no-whole-archive
)

target_sources (
  ui_test_runner
  PRIVATE
  ui_test_runner.c
)
