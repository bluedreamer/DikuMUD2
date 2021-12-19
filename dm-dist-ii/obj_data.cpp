#include "obj_data.h"
#include <cstring>

extern int world_noobjects;

obj_data::obj_data()
{
   world_noobjects++;

   memset(value, 0, sizeof(value));
   cost         = 0;
   cost_per_day = 0;
   flags        = 0;
   type         = 0;
   equip_pos    = 0;
   resistance   = 0;
}

obj_data::~obj_data()
{
   world_noobjects--;
}
