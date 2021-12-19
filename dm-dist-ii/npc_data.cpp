#include "npc_data.h"

#include <cstring>
extern int world_nonpc;

npc_data::npc_data()
{
   world_nonpc++;

   memset(weapons, 0, sizeof(weapons));
   memset(spells, 0, sizeof(spells));
   default_pos = POSITION_STANDING;
   flags       = 0;
}

npc_data::~npc_data()
{
   world_nonpc--;
}
