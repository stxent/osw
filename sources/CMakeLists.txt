# Copyright (C) 2017 xent
# Project is distributed under the terms of the MIT License

string(TOLOWER ${CMAKE_SYSTEM_NAME} OS_TYPE)

file(GLOB_RECURSE SOURCE_FILES "${OS_TYPE}/*.c")
add_library(osw_generic OBJECT ${SOURCE_FILES})
target_compile_definitions(osw_generic PUBLIC -DOS_TYPE=${OS_TYPE})
target_include_directories(osw_generic PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(osw_generic PUBLIC xcore)
install(TARGETS osw_generic EXPORT osw-targets)
include("${OS_TYPE}/CMakeLists.txt" OPTIONAL)
