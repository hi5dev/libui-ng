cmake_minimum_required (VERSION 3.25)

project (
  ui_api

  DESCRIPTION
  "Collection of header-only/interface libraries for LibUI's API"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_subdirectory (ui)
add_subdirectory (ui_headless)
add_subdirectory (ui_test)

if (APPLE)
  add_subdirectory (ui_osx)

elseif (LINUX)
  add_subdirectory (ui_linux)

elseif (WIN32)
  add_subdirectory (ui_win32)

else ()
  message (WARNING "No interface for ${CMAKE_SYSTEM_NAME} systems")

endif ()
