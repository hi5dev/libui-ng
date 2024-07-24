cmake_minimum_required (VERSION 3.25)

add_subdirectory (cmocka)

project (
  libui_test

  DESCRIPTION
  "LibUI usability tests"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

add_executable (${PROJECT_NAME})

add_executable (libui::test ALIAS ${PROJECT_NAME})

if (WIN32)
  set_target_properties (libui_test PROPERTIES WIN32_EXECUTABLE ON)

  libui_generate_manifest (
    ${PROJECT_NAME}

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
    libui_test

    PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc"
  )
endif ()

target_link_libraries (${PROJECT_NAME} PRIVATE libui::libui)

target_sources (
  ${PROJECT_NAME}

  PRIVATE
  drawtests.c
  images.c
  main.c
  menus.c
  page1.c
  page2.c
  page3.c
  page4.c
  page5.c
  page6.c
  page7.c
  page7a.c
  page7b.c
  page7c.c
#  page8.c
#  page9.c
#  page10.c
  page11.c
  page12.c
  page13.c
  page14.c
  page15.c
  page16.c
  page17.c
  spaced.c
)

add_subdirectory (qa)
add_subdirectory (unit)
