cmake_minimum_required (VERSION 3.25)

project (
  ui_headless_interface

  DESCRIPTION
  "Minimal implementation of LibUI's API that can be used for testing and debugging purposes."

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

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
  ui_headless.h
)
