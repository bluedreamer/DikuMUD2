#pragma once
#include <cstdint>
#include <ctime>

struct pc_time_data
{
   time_t   creation; /* This represents time when the pc was created.     */
   time_t   connect;  /* This is the last time that the pc connected.      */
   time_t   birth;    /* This represents the characters age                */
   uint32_t played;   /* This is the total accumulated time played in secs */
};
