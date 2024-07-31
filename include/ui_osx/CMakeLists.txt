cmake_minimum_required (VERSION 3.25)

add_library (ui_osx_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::osx::interface ALIAS ui_osx_interface)

target_link_libraries (
  ui_osx_interface

  INTERFACE
  ui::interface
)

set_target_properties (
  ui_osx_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_osx_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_osx.h
  ui_osx_main.h
)

if (TARGET ui_test_interface)
  target_link_libraries (ui_test_interface INTERFACE ui::osx::interface)
endif ()

if (UI_INSTALL)
  install(TARGETS ui_osx_interface FILE_SET HEADERS)
endif ()
