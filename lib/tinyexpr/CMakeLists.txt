cmake_minimum_required (VERSION 3.25)

project (
  tinyexpr

  DESCRIPTION
  "Tiny recursive descent expression parser, compiler, and evaluation engine for math expressions."

  HOMEPAGE_URL
  "https://github.com/codeplea/tinyexpr"

  LANGUAGES
  C
)

add_library (tinyexpr STATIC)

target_sources (tinyexpr PUBLIC FILE_SET HEADERS FILES tinyexpr.h)

target_sources (tinyexpr PRIVATE tinyexpr.c)

if (UI_ENABLE_TESTING)
  add_executable (tinyexpr_test)
  target_link_libraries (tinyexpr_test PRIVATE tinyexpr)
  target_sources (tinyexpr_test PRIVATE tinyexpr_test.c)
  add_test (NAME "tinyexpr/test" COMMAND tinyexpr_test)
endif ()
