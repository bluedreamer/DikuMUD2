find_package(FLEX)
find_package(BISON)

set(DMC_SRC
        ../bytestring.cpp
        ../common.cpp
        ../db_file.cpp
        ../extra.cpp
        ../memory.cpp
        ../money.cpp
        ../namelist.cpp
        ../string.cpp
        ../structs.cpp
        ../textutil.cpp
        ../utility.cpp
        dmc.cpp
        dmc_process.cpp
        dmclex.l
        dmcpar.y
        ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmclex.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmcpar.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmcpar.h
        )

set(DMC_H dmc.h dmctok.h)

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})

BISON_TARGET(Parser dmcpar.y ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmcpar.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmcpar.h)

FLEX_TARGET(Scanner dmclex.l ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmclex.cpp)

add_compile_definitions(MUD_DEBUG MEMCHECK MEMORY_DEBUG DMC_SRC)

add_executable(dmc ${DMC_SRC} ${DMC_H} ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS})
