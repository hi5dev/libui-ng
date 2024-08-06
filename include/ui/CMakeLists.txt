cmake_minimum_required (VERSION 3.25)

project (
  ui_interface

  DESCRIPTION
  "A header-only/interface library for LibUI's platform-independent API."

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::interface ALIAS ui_interface)

set_target_properties (
  ui_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_control.h
  ui_event.h
  ui_main.h
  ui_window.h
)

if (UI_INSTALL)
  install(TARGETS ui_interface FILE_SET HEADERS)
endif ()
