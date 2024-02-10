cmake_minimum_required(VERSION 3.20)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(static_key)

add_executable(test_static_key_c static_key.c)
add_executable(test_static_key_cpp static_key.cpp)

add_test(NAME test_static_key_c COMMAND ${CMAKE_CURRENT_LIST_DIR}/tests/test.sh ${STATIC_KEY_RUNNER} ./test_static_key_c)
add_test(NAME test_static_key_cpp COMMAND ${CMAKE_CURRENT_LIST_DIR}/tests/test.sh ${STATIC_KEY_RUNNER} ./test_static_key_cpp)
