cmake_minimum_required(VERSION 3.20)
project(DikuMUD2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#include_directories(dm-dist-ii)

add_subdirectory(dm-dist-ii)
add_subdirectory(dm-dist-ii/DIL)
add_subdirectory(dm-dist-ii/DMC)
add_subdirectory(dm-dist-ii/Mplex)
add_subdirectory(dm-dist-ii/PP)
add_subdirectory(dm-dist-ii/Utility)

add_subdirectory(unit_tests)
