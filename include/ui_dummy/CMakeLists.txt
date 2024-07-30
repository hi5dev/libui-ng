cmake_minimum_required (VERSION 3.25)

add_library (ui_dummy_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::dummy::interface ALIAS ui_dummy_interface)

target_link_libraries (
  ui_dummy_interface

  INTERFACE
  ui::interface
)

set_target_properties (
  ui_dummy_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_dummy_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_dummy.h
)
