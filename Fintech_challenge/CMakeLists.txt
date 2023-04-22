cmake_minimum_required(VERSION 3.0.0)
project(davince VERSION 0.1.0)

add_executable(davince main.cpp order_table.cpp order_operations.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
