#pragma once

#include "unit_data.h"
#include <cstdint>

struct door_data
{
   unit_data       *thing;
   unit_data       *room;
   unit_data       *reverse; /* Reverse Room or Inside thing */
   file_index_type *key;
   uint8_t          direction; /* For rooms, which direction was picked? */
   uint8_t         *flags;
   uint8_t         *rev_flags;
   const char      *name;
};
