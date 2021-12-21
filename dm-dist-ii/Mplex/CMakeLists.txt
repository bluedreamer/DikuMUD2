set(MPLEX_SRC
        ../bytestring.cpp
        ../common.cpp
        ../memory.cpp
        ../protocol.cpp
        ../queue.cpp
        ../select.cpp
        ../textutil.cpp
        ../utility.cpp
        mplex.cpp
        mplex.h
        network.cpp
        network.h
        translate.cpp
        )

add_compile_definitions(MPLEX MUD_DEBUG MEMCHECK MEMORY_DEBUG)

add_executable(mplex ${MPLEX_SRC})
