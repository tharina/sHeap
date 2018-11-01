cmake_minimum_required(VERSION 3.5)
project(sheap)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

if (UNIX AND NOT APPLE)
    SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lrt")
endif (UNIX AND NOT APPLE)

add_library(sheap SHARED sheap.c sheap.h sheap.hpp)

include_directories(.)

add_subdirectory(tests)