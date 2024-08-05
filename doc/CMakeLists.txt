cmake_minimum_required (VERSION 3.25)

project (
  ui_doc

  DESCRIPTION
  "LibUI documentation"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_subdirectory (7gui)
