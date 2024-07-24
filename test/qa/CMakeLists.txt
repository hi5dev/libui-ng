cmake_minimum_required (VERSION 3.25)

project (
  libui_test_qa

  DESCRIPTION
  "LibUI acceptance tests."

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

add_executable (${PROJECT_NAME})

add_executable (libui::test::qa ALIAS ${PROJECT_NAME})

if (WIN32)
  set_target_properties (libui_test_qa PROPERTIES WIN32_EXECUTABLE ON)

  libui_generate_manifest (
    ASSEMBLY_NAME
    "github.hi5dev.${PROJECT_NAME}"

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
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe.manifest"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc"
  )
endif ()

target_link_libraries (${PROJECT_NAME} PRIVATE libui::libui)

target_sources (
  ${PROJECT_NAME}

  PRIVATE
  button.c
  checkbox.c
  entry.c
  label.c
  main.c
  qa.c
  radiobuttons.c
  separator.c
  spinbox.c
  window.c
)
