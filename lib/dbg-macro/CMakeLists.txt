cmake_minimum_required (VERSION 3.25)

project (
  dbg-macro

  DESCRIPTION
  "A set of dbg(…) macros for C."

  HOMEPAGE_URL
  "https://github.com/eerimoq/dbg-macro/tree/0.12.1"

  VERSION
  0.12.1

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "-libui")

add_library (dbg_macro INTERFACE)

set_target_properties (dbg_macro PROPERTIES LINKER_LANGUAGE C)

target_sources (dbg_macro PUBLIC FILE_SET HEADERS FILES dbg.h)
