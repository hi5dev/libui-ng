cmake_minimum_required (VERSION 3.25)

add_library (ui_headless_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::headless::interface ALIAS ui_headless_interface)

target_link_libraries (
  ui_headless_interface

  INTERFACE
  ui::interface
)

set_target_properties (
  ui_headless_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_headless_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_headless_main.h
)

if (TARGET ui_test_interface)
  target_link_libraries (ui_test_interface INTERFACE ui::headless::interface)
endif ()
