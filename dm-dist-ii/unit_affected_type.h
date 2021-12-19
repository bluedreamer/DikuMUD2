#pragma once
#include <cstdint>

class unit_data;
struct unit_affected_type
{
   int16_t             id;
   uint16_t            beat;     /* Beat in 1/4 of secs, 0 = None */
   int16_t             duration; /* How many beats until end      */

   int                 data[3];

   int16_t             firstf_i;
   int16_t             tickf_i;
   int16_t             lastf_i;
   int16_t             applyf_i;

   unit_data          *owner;
   unit_affected_type *next, *gnext, *gprevious;
};
