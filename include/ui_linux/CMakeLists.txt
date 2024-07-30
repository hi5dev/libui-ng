cmake_minimum_required (VERSION 3.25)

add_library (ui_linux_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::linux::interface ALIAS ui_linux_interface)

target_link_libraries (
  ui_linux_interface

  INTERFACE
  ui::interface
)

set_target_properties (
  ui_linux_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_linux_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_linux.h
  ui_linux_main.h
)
