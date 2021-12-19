#pragma once
#include "file_index_type.h"

class room_direction_data
{
public:
   room_direction_data();
   ~room_direction_data();

   class cNamelist  open_name; /* For Open & Enter                  */

   file_index_type *key;
   unit_data       *to_room;

   uint8_t          exit_info; /* Door info flags                   */
};
