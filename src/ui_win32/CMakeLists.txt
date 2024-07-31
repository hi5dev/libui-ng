cmake_minimum_required (VERSION 3.25)

add_library (ui_win32 STATIC)

add_library (ui::ui ALIAS ui_win32)

target_link_libraries (
  ui_win32

  PRIVATE
  ui::win32::interface

  PUBLIC
  ui
)

target_sources (
  ui_win32

  PRIVATE
  ui_win32.c
  ui_win32_main.c
)

if (UI_INSTALL)
  install(TARGETS ui_win32)
endif ()
