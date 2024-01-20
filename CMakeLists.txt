# Copyright (C) 2017 xent
# Project is distributed under the terms of the MIT License

cmake_minimum_required(VERSION 3.21)
project(osw C)

find_package(xcore QUIET)
if(xcore_FOUND)
    message(STATUS "Found xcore package")
endif()

# Default compiler flags

if(CMAKE_INTERPROCEDURAL_OPTIMIZATION)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILE_OPTIONS_IPO -flto=auto -ffat-lto-objects)
    endif()
endif()

# Collect files for object libraries

add_subdirectory(sources)
add_library(${PROJECT_NAME} $<TARGET_OBJECTS:osw_generic>)
target_link_libraries(${PROJECT_NAME} PUBLIC osw_generic)

# Configure library installation

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
