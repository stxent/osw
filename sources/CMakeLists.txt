# Copyright (C) 2017 xent
# Project is distributed under the terms of the GNU General Public License v3.0

string(TOLOWER ${CMAKE_SYSTEM_NAME} OS_TYPE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOS_TYPE=${OS_TYPE}")
file(GLOB_RECURSE SOURCE_FILES "${OS_TYPE}/*.c")
add_library(sources OBJECT ${SOURCE_FILES})
include("${OS_TYPE}/CMakeLists.txt" OPTIONAL)
