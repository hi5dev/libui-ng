cmake_minimum_required (VERSION 3.25)

add_library (ui_test_expect STATIC)

add_library (ui::test::expect ALIAS ui_test_expect)

target_link_libraries (
  ui_test_expect

  PRIVATE
  ui::test::expect::interface
)

target_sources (
  ui_test_expect

  PUBLIC
  ui_test_expect.c
)
