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
  ui::osx::interface
  ui::test
)

target_sources (
  ui_osx

  PRIVATE
  ui_osx.c
  ui_osx_main.c
)

if (TARGET ui::test::runner)
  ui_test_runner (LINK_LIBRARY ui::ui)

  add_test (NAME "ui_osx/test" COMMAND ui::test::runner -n ui_osx_test)
  add_test (NAME "ui_osx/test/main" COMMAND ui::test::runner -n ui_osx_main_test)
endif ()

if (UI_INSTALL)
  install(TARGETS ui_osx)
endif ()
