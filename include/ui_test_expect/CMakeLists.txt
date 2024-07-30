cmake_minimum_required (VERSION 3.25)

add_library (ui_test_expect_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::test::expect::interface ALIAS ui_test_expect_interface)

target_link_libraries (
  ui_test_expect_interface

  INTERFACE
  ui::test::interface
)

set_target_properties (
  ui_test_expect_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_test_expect_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_test_expect.h
)
