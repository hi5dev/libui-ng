cmake_minimum_required (VERSION 3.25)

add_library (ui_test_unit_main STATIC)

add_library (ui::test::unit::main ALIAS ui_test_unit_main)

target_link_libraries (
  ui_test_unit_main

  PRIVATE
  ui::test::unit::main::interface
)

target_sources (
  ui_test_unit_main

  PUBLIC
  ui_test_unit_main.c
)
