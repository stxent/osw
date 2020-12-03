# Copyright (C) 2017 xent
# Project is distributed under the terms of the MIT License

project(osw C)
cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0069 NEW)

option(USE_LTO "Enable Link Time Optimization." OFF)

# Default compiler flags
set(FLAGS_DEFAULT "-fdata-sections -ffunction-sections -Wall -Wextra -Wshadow -pedantic")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS_DEFAULT} ${FLAGS_CPU}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

find_package(xcore QUIET)
if(xcore_FOUND)
    message(STATUS "Found xcore package")
endif()

if(${USE_LTO})
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILE_OPTIONS_IPO -flto -ffat-lto-objects)
    endif()
endif()

add_subdirectory(sources)
add_library(${PROJECT_NAME} $<TARGET_OBJECTS:osw_generic>)
target_link_libraries(${PROJECT_NAME} PUBLIC osw_generic)

# Library installation configuration
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/osw
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
)
install(TARGETS ${PROJECT_NAME}
        EXPORT osw-targets
        ARCHIVE DESTINATION lib
)
install(EXPORT osw-targets
        FILE osw-targets.cmake
        DESTINATION lib/osw
)
install(FILES "${PROJECT_SOURCE_DIR}/config.cmake.in"
        DESTINATION lib/osw
        RENAME osw-config.cmake
)
