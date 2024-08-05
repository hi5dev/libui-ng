cmake_minimum_required (VERSION 3.25)

project (
  ui_7gui

  DESCRIPTION
  "LibUI 7GUIs implemenation."

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

function (add_ui_7gui_executable TARGET)
  add_executable ("ui_${TARGET}")

  add_executable ("ui::${TARGET}" ALIAS "ui_${TARGET}")

  target_link_libraries (
    "ui_${TARGET}"

    PRIVATE
    ui::ui
  )

  target_sources (
    "ui_${TARGET}"

    PRIVATE FILE_SET HEADERS

    FILES
    "ui_${TARGET}.h"
  )

  target_sources (
    "ui_${TARGET}"

    PRIVATE
    "ui_${TARGET}.c"
  )

  if (WIN32)
    set_target_properties ("ui_${TARGET}" PROPERTIES WIN32_EXECUTABLE 1)
    target_link_options ("ui_${TARGET}" PUBLIC -municode)
  endif ()
endfunction ()

add_ui_7gui_executable (cells)
add_ui_7gui_executable (circle_drawer)
add_ui_7gui_executable (counter)
add_ui_7gui_executable (crud)
add_ui_7gui_executable (flight_booker)
add_ui_7gui_executable (temperature_converter)
add_ui_7gui_executable (timer)
