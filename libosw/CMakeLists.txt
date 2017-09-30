#Copyright (C) 2017 xent
#Project is distributed under the terms of the GNU General Public License v3.0

string(TOLOWER ${CMAKE_SYSTEM_NAME} LIBRARY_SOURCES_DIR)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOS_TYPE=${LIBRARY_SOURCES_DIR}")
file(GLOB_RECURSE LIBRARY_SOURCES "${PROJECT_SOURCE_DIR}/libosw/${LIBRARY_SOURCES_DIR}/*.c")
include("${PROJECT_SOURCE_DIR}/libosw/${LIBRARY_SOURCES_DIR}/CMakeLists.txt" OPTIONAL)
