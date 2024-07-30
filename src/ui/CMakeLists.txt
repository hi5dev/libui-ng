cmake_minimum_required (VERSION 3.25)

add_library (ui STATIC)

target_link_libraries (
  ui

  PRIVATE
  ui::interface
)

target_sources (
  ui

  PRIVATE
  ui_event.c
  ui_main.c
)
