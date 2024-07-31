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
  ui_linux_main.c
)

if (TARGET ui_test)
  target_link_libraries (ui_test PUBLIC ui::ui)

  target_sources (
    ui_test

    PRIVATE
    ui_linux_test.c
    ui_linux_main_test.c
  )

  add_test (NAME "ui_linux_test" COMMAND ui::test -n ui_linux_test)
  add_test (NAME "ui_linux_main_test" COMMAND ui::test -n ui_linux_main_test)
endif ()

if (UI_INSTALL)
  install(TARGETS ui_linux)
endif ()
