cmake_minimum_required (VERSION 3.25)

project (
  libui_test_unit

  DESCRIPTION
  "LibUI unit tests"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

add_executable (${PROJECT_NAME})

add_executable (libui::test::unit ALIAS ${PROJECT_NAME})

target_link_libraries (${PROJECT_NAME} PRIVATE cmocka-static libui::libui)

target_sources (
  ${PROJECT_NAME}

  PRIVATE
  button.c
  checkbox.c
  combobox.c
  drawmatrix.c
  entry.c
  init.c
  label.c
  main.c
  menu.c
  progressbar.c
  radiobuttons.c
  slider.c
  spinbox.c
)

if (WIN32)
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
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc"
  )
endif ()

add_test (NAME "libui/test" COMMAND "libui_test_unit")
