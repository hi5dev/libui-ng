cmake_minimum_required (VERSION 3.25)

project (
  ui_lib

  DESCRIPTION
  "Third-party libraries used by LibUI."

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_subdirectory (tinyexpr)
