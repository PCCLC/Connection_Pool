cmake_minimum_required(VERSION 3.0)

project(ConnectionPool)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(src)
