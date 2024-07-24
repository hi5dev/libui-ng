cmake_minimum_required (VERSION 3.25)

set (FETCHCONTENT_UPDATES_DISCONNECTED ON)

cmake_policy (SET CMP0077 NEW)
set (BUILD_TESTING OFF CACHE BOOL "Disable CTest for CMocka" FORCE)
set (WITH_STATIC_LIB ON CACHE BOOL "CMocka WITH_STATIC_LIB Option" FORCE)
set (WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "CMocka WITH_CMOCKERY_SUPPORT Option" FORCE)
set (WITH_EXAMPLES OFF CACHE BOOL "CMocka WITH_EXAMPLES Option" FORCE)
set (UNIT_TESTING OFF CACHE BOOL "CMocka UNIT_TESTING Option" FORCE)

include (FetchContent)

fetchcontent_declare (cmocka EXCLUDE_FROM_ALL URL "https://cmocka.org/files/1.1/cmocka-1.1.5.tar.xz")

message (STATUS "Synchronizing cmocka dependency")

fetchcontent_makeavailable (cmocka)
