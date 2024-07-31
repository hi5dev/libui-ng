cmake_minimum_required (VERSION 3.25)

add_library (ui_osx STATIC)

add_library (ui::ui ALIAS ui_osx)

target_link_libraries (
  ui_osx

  PRIVATE
  ui::osx::interface
)

target_sources (
  ui_osx

  PRIVATE
  ui_osx.c
)

if (UI_INSTALL)
  install(TARGETS ui_osx)
endif ()
