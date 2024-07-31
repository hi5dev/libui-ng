cmake_minimum_required (VERSION 3.25)

add_library (ui_dummy STATIC)

add_library (ui::dummy ALIAS ui_dummy)

target_link_libraries (
  ui_dummy

  PRIVATE
  ui::dummy::interface
)

target_sources (
  ui_dummy

  PRIVATE
  ui_dummy.c
  ui_dummy_main.c
)
