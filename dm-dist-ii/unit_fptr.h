#pragma once
#include <cstdint>
struct unit_fptr
{
   uint16_t          index;      /* Index to function pointer array             */
   uint16_t          heart_beat; /* in 1/4 of a sec                             */
   uint16_t          flags;      /* When to override next function (boolean)    */
   void             *data;       /* Pointer to data local for this unit         */
   struct unit_fptr *next;       /* Next in linked list                         */
};
