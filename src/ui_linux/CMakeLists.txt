cmake_minimum_required (VERSION 3.25)

add_library (ui_linux STATIC)

add_library (ui::ui ALIAS ui_linux)

target_link_libraries (
  ui_linux

  PRIVATE
  ui::linux::interface
)

target_sources (
  ui_linux

  PRIVATE
  ui_linux.c
)
