cmake_minimum_required (VERSION 3.25)

project (
  cpp-multithread

  DESCRIPTION
  "LibUI C++ multithreading example."

  VERSION
  1.0.0

  LANGUAGES
  CXX
)

set (CMAKE_CXX_STANDARD 11)

add_executable (${PROJECT_NAME})

add_executable (libui::examples::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries (${PROJECT_NAME} PRIVATE libui::libui)

target_sources (${PROJECT_NAME} PRIVATE main.cpp)

if (WIN32)
  target_compile_definitions (${PROJECT_NAME} PUBLIC -DUNICODE -D_UNICODE)

  libui_generate_manifest (
    ASSEMBLY_NAME
    "github.hi5dev.libui-${PROJECT_NAME}"

    ASSEMBLY_DESCRIPTION
    "${PROJECT_DESCRIPTION}"

    ASSEMBLY_VERSION
    "${PROJECT_VERSION}.0"

    MANIFEST_OUTPUT
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe.manifest"

    RESOURCE_OUTPUT
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc"
  )

  target_sources (
    ${PROJECT_NAME}

    PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc"
  )

endif ()
