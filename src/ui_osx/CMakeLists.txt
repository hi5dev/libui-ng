cmake_minimum_required (VERSION 3.25)

project (
  ui_osx

  DESCRIPTION
  "OSX implementation of LibUI"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui_osx STATIC)

add_library (ui::ui ALIAS ui_osx)

target_link_libraries (
  ui_osx

  PRIVATE
  ui::test

  PUBLIC
  ui::osx::interface
  ui::ui
)

target_sources (
  ui_osx

  PRIVATE
  ui_osx.c
  ui_osx_main.c
  ui_osx_window.c
)

ui_test (
  ui_osx

  TEST_CASES
  ui_osx_test
  ui_osx_main_test
)

if (UI_INSTALL)
  install(TARGETS ui_osx)
endif ()
