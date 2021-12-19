set(MPLEX_SRC
        mplex.cpp
        translate.cpp
        network.cpp
        ../select.cpp
        ../queue.cpp
        ../utility.cpp
        ../textutil.cpp
        ../memory.cpp
        ../protocol.cpp
        ../cHook.cpp
        ../bytestring.cpp
        ../cCaptainHook.cpp
        ../common.cpp)

set(MPLEX_H mplex.h network.h)

add_compile_definitions(MPLEX MUD_DEBUG MEMCHECK MEMORY_DEBUG)

add_executable(mplex ${MPLEX_SRC} ${MPLEX_H})
