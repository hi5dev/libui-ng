cmake_minimum_required (VERSION 3.25)

add_library (ui_test STATIC)

add_library (ui::test ALIAS ui_test)

target_link_libraries (
  ui_test

  PRIVATE
  ui::test::interface
)

target_sources (
  ui_test

  PRIVATE
  ui_test.c
)
