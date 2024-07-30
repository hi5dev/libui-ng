cmake_minimum_required (VERSION 3.25)

add_library (ui_test_unit_main_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::test::unit::main::interface ALIAS ui_test_unit_main_interface)

target_link_libraries (
  ui_test_unit_main_interface

  INTERFACE
  ui::test::unit::interface
)

set_target_properties (
  ui_test_unit_main_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_test_unit_main_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_test_unit_main.h
)
