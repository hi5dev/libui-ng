cmake_minimum_required (VERSION 3.25)

project (
  libui

  DESCRIPTION
  "A portable GUI library for C."

  HOMEPAGE_URL
  "https://github.com/hi5dev/libui-ng"

  VERSION
  4.1.0

  LANGUAGES
  C
  CXX
)

set (PROJECT_VERSION_TWEAK "-alpha+git.hi5dev.main")

set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_STANDARD 11)

set (CMAKE_STATIC_LIBRARY_PREFIX "")

option (BUILD_EXAMPLES "Build example programs" ${PROJECT_IS_TOP_LEVEL})
option (BUILD_DOCS "Build documentation" ${PROJECT_IS_TOP_LEVEL})
option (BUILD_TESTS "Build test programs" ${PROJECT_IS_TOP_LEVEL})
option (INSTALL_TARGETS "Generate installation targets" ${PROJECT_IS_TOP_LEVEL})

add_subdirectory (include)
add_subdirectory (src)

if (BUILD_DOCS)
  add_subdirectory (doc)
endif ()

if (BUILD_TESTS)
  enable_testing ()
  add_subdirectory (test)
endif ()

if (INSTALL_TARGETS)
  include (GNUInstallDirs)
endif ()
