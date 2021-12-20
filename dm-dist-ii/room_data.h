#pragma once
#include "room_direction_data.h"

#include <cstdint>

class room_data
{
public:
   room_data();
   ~room_data();

   room_direction_data *dir_option[6]; /* Her?? */

   uint8_t flags;         /* Room flags                              */
   uint8_t movement_type; /* The type of movement (city, hills etc.) */
   uint8_t resistance;    /* Magic resistance of the room            */
};
