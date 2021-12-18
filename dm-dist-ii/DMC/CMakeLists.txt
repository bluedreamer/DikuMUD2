find_package(FLEX)
find_package(BISON)

set(DMC_SRC dmc.cpp dmc_process.cpp ../utility.cpp ../string.cpp ../structs.cpp ../money.cpp ../db_file.cpp
        ../bytestring.cpp ../namelist.cpp ../memory.cpp ../textutil.cpp ../extra.cpp ../common.cpp ../unit_data.cpp)
set(DMC_H dmc.h dmctok.h)

include_directories(.)

BISON_TARGET(Parser dmcpar.y ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmcpar.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmcpar.h)

FLEX_TARGET(Scanner dmclex.l ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmclex.cpp)

add_compile_definitions(MUD_DEBUG MEMCHECK MEMORY_DEBUG DMC_SRC)

add_executable(dmc ${DMC_SRC} ${DMC_H} ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS})
