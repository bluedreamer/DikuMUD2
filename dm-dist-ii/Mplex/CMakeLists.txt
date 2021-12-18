set(MPLEX_SRC mplex.cpp network.cpp translate.cpp bytestring.cpp ../select.cpp ../queue.cpp ../utility.cpp ../textutil.cpp ../memory.cpp ../protocol.cpp ../common.cpp)
set(MPLEX_H mplex.h network.h)

add_compile_definitions(MPLEX MUD_DEBUG MEMCHECK MEMORY_DEBUG)

add_executable(mplex ${MPLEX_SRC} ${MPLEX_H})
