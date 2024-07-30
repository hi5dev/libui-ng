cmake_minimum_required (VERSION 3.25)

add_library (ui_test_unit STATIC)

add_library (ui::test::unit ALIAS ui_test_unit)

target_link_libraries (
  ui_test_unit

  PRIVATE
  ui::test::unit::interface
)

target_sources (
  ui_test_unit

  PRIVATE
  ui_test_unit.c
)
