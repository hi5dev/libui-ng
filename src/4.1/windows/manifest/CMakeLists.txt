cmake_minimum_required (VERSION 3.25)

set (
  LIBUI_MANIFESTS_SOURCE_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}"

  CACHE PATH
  "Full path to the Windows manifest and resource templates"
)

# Usage:
#   libui_generate_manifest (
#     ASSEMBLY_NAME
#     <orginization.division.appname>
#
#     ASSEMBLY_DESCRIPTION
#     <application description>
#
#     ASSEMBLY_VERSION
#     <major.minor.patch.build
#
#     MANIFEST_OUTPUT
#     <app.exe.manifest>
#
#     RESOURCE_OUTPUT
#     <app.rc>
#   )
function (libui_generate_manifest)
  set (
    single_value_args
    "ASSEMBLY_NAME"
    "ASSEMBLY_DESCRIPTION"
    "ASSEMBLY_VERSION"
    "MANIFEST_OUTPUT"
    "RESOURCE_OUTPUT"
  )

  cmake_parse_arguments (M "" "${single_value_args}" "" ${ARGN})

  set (ASSEMBLY_NAME "${M_ASSEMBLY_NAME}")
  set (ASSEMBLY_DESCRIPTION "${M_ASSEMBLY_DESCRIPTION}")
  set (ASSEMBLY_VERSION "${M_ASSEMBLY_VERSION}")

  get_filename_component (MANIFEST_NAME "${M_MANIFEST_OUTPUT}" NAME)

  configure_file ("${LIBUI_MANIFESTS_SOURCE_DIR}/manifest.xml.in" "${M_MANIFEST_OUTPUT}" @ONLY)

  configure_file ("${LIBUI_MANIFESTS_SOURCE_DIR}/resource.rc.in" "${M_RESOURCE_OUTPUT}")
endfunction ()
