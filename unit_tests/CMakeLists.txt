set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

# 'Boost_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with tests
add_executable (test_runner main.cpp time_info_data_test.cpp)
target_link_directories(test_runner PUBLIC ${CMAKE_BINARY_DIR}/dm-dist-ii)
target_link_libraries (test_runner ${Boost_LIBRARIES} dmserver_lib crypt)
