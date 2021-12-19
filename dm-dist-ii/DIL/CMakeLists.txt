find_package(FLEX)
find_package(BISON)

set(DIL_SRC
        ../CByteBuffer.cpp
        ../bytestring.cpp
        ../cHashSystem.cpp
        ../cNamelist.cpp
        ../cStringConstant.cpp
        ../cStringInstance.cpp
        ../common.cpp
        ../constants.cpp
        ../common.h
        ../db_file.cpp
        ../extra.cpp
        ../files.cpp
        ../memory.cpp
        ../room_data.cpp
        ../room_data.h
        ../room_direction_data.cpp
        ../room_direction_data.h
        ../string.cpp
        ../structs.cpp
        ../textutil.cpp
        ../utility.cpp
        dilpar.h
        diltok.h
        )

include_directories(.)

BISON_TARGET(Parser dilpar.y ${CMAKE_CURRENT_BINARY_DIR}/tmp_dilpar.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp_dilpar.h)

FLEX_TARGET(Scanner dillex.l ${CMAKE_CURRENT_BINARY_DIR}/tmp_dillex.cpp)

add_compile_definitions(MUD_DEBUG MEMCHECK MEMORY_DEBUG DIL DMC)

add_executable(dil
        ${DIL_SRC}
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS}
        )
