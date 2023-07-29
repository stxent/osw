# Copyright (C) 2017 xent
# Project is distributed under the terms of the MIT License

find_package(Threads REQUIRED)
target_link_libraries(osw_generic PUBLIC Threads::Threads)
