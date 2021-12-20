find_package(FLEX)
find_package(BISON)
include_directories(..)

BISON_TARGET(Parser calcpar.y ${CMAKE_CURRENT_BINARY_DIR}/tmp_calcpar.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_calcpar.h)

FLEX_TARGET(Scanner calclex.l ${CMAKE_CURRENT_BINARY_DIR}/tmp_calclex.cpp)

include_directories(.)

add_executable(analyse analyse.cpp)
add_executable(blktest blktest.cpp ../bytestring.cpp ../blkfile.cpp ../textutil.cpp ../utility.cpp ../files.cpp ../common.cpp)
add_executable(calc calctok.h ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})
add_executable(conanalyze conanalyze.cpp)
add_executable(csv2 csv2guild.cpp)
add_executable(header header.cpp)
add_executable(tuborg tuborg.cpp)
add_executable(verify verify.cpp)
