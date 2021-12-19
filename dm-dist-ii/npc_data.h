#pragma once
#include "values.h"

#include <cstdint>

class npc_data
{
public:
   npc_data();
   ~npc_data();

   uint8_t weapons[WPN_GROUP_MAX];
   uint8_t spells[SPL_GROUP_MAX];

   uint8_t default_pos; /* Default position for NPC               */
   uint8_t flags;       /* flags for NPC behavior                 */
};
