set(MPLEX_SRC
        mplex.cpp
        translate.cpp
        network.cpp
        ../bytestring.cpp
        ../cCaptainHook.cpp
        ../cHook.cpp
        ../cQueue.cpp
        ../cQueueElem.cpp
        ../common.cpp
        ../constants.cpp

        ../memory.cpp
        ../protocol.cpp
        ../textutil.cpp
        ../utility.cpp
        )

set(MPLEX_H mplex.h network.h)

add_compile_definitions(MPLEX MUD_DEBUG MEMCHECK MEMORY_DEBUG)

add_executable(mplex ${MPLEX_SRC} ${MPLEX_H})
