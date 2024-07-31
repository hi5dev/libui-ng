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
  ui_osx_main.c
)

if (TARGET ui_test)
  target_link_libraries (ui_test PUBLIC ui::ui)

  target_sources (
    ui_test

    PRIVATE
    ui_linux_main_test.c
    ui_linux_test.c
  )

  add_test (NAME "ui_osx_main_test" COMMAND ui::test -n ui_osx_main_test)
  add_test (NAME "ui_osx_test" COMMAND ui::test -n ui_osx_test)
endif ()

if (UI_INSTALL)
  install(TARGETS ui_osx)
endif ()
