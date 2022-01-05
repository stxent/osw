# Copyright (C) 2017 xent
# Project is distributed under the terms of the MIT License

target_link_libraries(osw_generic PUBLIC pthread)
target_compile_definitions(osw_generic PUBLIC _POSIX_C_SOURCE=200809L)
