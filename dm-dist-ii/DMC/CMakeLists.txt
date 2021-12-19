find_package(FLEX)
find_package(BISON)

set(DMC_SRC
        ../CByteBuffer.cpp
        ../bytestring.cpp
        ../cNamelist.cpp
        ../cNamelist.cpp
        ../cQueue.cpp
        ../cQueueElem.cpp
        ../cStringConstant.cpp
        ../cStringInstance.cpp
        ../cHashSystem.cpp
        ../char_data.cpp
        ../common.cpp
        ../db_file.cpp
        ../dilargstype.cpp
        ../dilargtype.cpp
        ../extra.cpp
        ../memory.cpp
        ../money.cpp
        ../npc_data.cpp
        ../obj_data.cpp
        ../pc_data.cpp
        ../room_data.cpp
        ../room_direction_data.cpp
        ../string.cpp
        ../structs.cpp
        ../textutil.cpp
        ../unit_data.cpp
        ../utility.cpp
        dmc.cpp
        dmc_process.cpp
        )

set(DMC_H dmc.h dmctok.h)

include_directories(.)

BISON_TARGET(Parser dmcpar.y ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmcpar.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmcpar.h)

FLEX_TARGET(Scanner dmclex.l ${CMAKE_CURRENT_BINARY_DIR}/tmp_dmclex.cpp)

add_compile_definitions(MUD_DEBUG MEMCHECK MEMORY_DEBUG DMC_SRC)

add_executable(dmc ${DMC_SRC} ${DMC_H} ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS})
