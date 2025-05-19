cmake_minimum_required(VERSION 3.28)
project(chx VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_library(chx
    src/channel.cpp
)

target_include_directories(chx
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(chx
    PRIVATE -Wall -Wextra -Wpedantic -Werror
)

option(CHX_ENABLE_TESTS "Build and run tests" ON)

if (CHX_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
