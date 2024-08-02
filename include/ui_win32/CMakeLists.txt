cmake_minimum_required (VERSION 3.25)

add_library (ui_win32_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::win32::interface ALIAS ui_win32_interface)

target_link_libraries (
  ui_win32_interface

  INTERFACE
  ui::interface
)

target_compile_definitions (
  ui_win32_interface

  INTERFACE
  -D UNICODE
  -D _UNICODE
  -D WIN32_LEAN_AND_MEAN
)

set_target_properties (
  ui_win32_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_win32_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_win32.h
  ui_win32_main.h
)

if (UI_INSTALL)
  install(TARGETS ui_win32_interface FILE_SET HEADERS)
endif ()
