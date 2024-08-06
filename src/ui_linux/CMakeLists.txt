cmake_minimum_required (VERSION 3.25)

project (
  ui_linux

  DESCRIPTION
  "Linux implementation of LibUI"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui_linux STATIC)

add_library (ui::ui ALIAS ui_linux)

target_link_libraries (
  ui_linux

  PRIVATE
  ui::test

  PUBLIC
  ui::linux::interface
  ui
)

target_sources (
  ui_linux

  PRIVATE
  ui_linux.c
  ui_linux_main.c
  ui_linux_window.c
)

if (TARGET ui::test::runner)
  ui_test_runner (LINK_LIBRARY ui::ui)

  add_test (NAME "ui_linux/test" COMMAND ui::test::runner -n ui_linux_test)
  add_test (NAME "ui_linux/test/main" COMMAND ui::test::runner -n ui_linux_main_test)
endif ()

if (UI_INSTALL)
  install (TARGETS ui_linux)
endif ()
