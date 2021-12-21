find_package(FLEX)
find_package(BISON)

set(DIL_SRC
        ../bytestring.cpp
        ../common.cpp
        ../common.h
        ../db_file.cpp
        ../extra.cpp
        ../files.cpp
        ../memory.cpp
        ../namelist.cpp
        ../string.cpp
        ../structs.cpp
        ../textutil.cpp
        ../utility.cpp
        dillex.l
        dilpar.h
        dilpar.y
        diltok.h
        )

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})

BISON_TARGET(Parser dilpar.y ${CMAKE_CURRENT_BINARY_DIR}/tmp_dilpar.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_dilpar.h)

FLEX_TARGET(Scanner dillex.l ${CMAKE_CURRENT_BINARY_DIR}/tmp_dillex.cpp)

add_compile_definitions(MUD_DEBUG MEMCHECK MEMORY_DEBUG DIL DMC)

add_executable(dil
        ${DIL_SRC}
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS}
        )
