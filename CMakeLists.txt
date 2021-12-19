cmake_minimum_required(VERSION 3.20)
project(DikuMUD2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fpermissive -fdiagnostics-color=always)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fpermissive)

include_directories(dm-dist-ii)

add_subdirectory(dm-dist-ii)
add_subdirectory(dm-dist-ii/DIL)
add_subdirectory(dm-dist-ii/DMC)
add_subdirectory(dm-dist-ii/Mplex)
add_subdirectory(dm-dist-ii/PP)
add_subdirectory(dm-dist-ii/Utility)
