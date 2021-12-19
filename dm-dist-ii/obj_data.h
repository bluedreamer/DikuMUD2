#pragma once
#include <cstdint>

class obj_data
{
public:
   obj_data();
   ~obj_data();

   int32_t  value[5];     /* Values of the item (see list)       */
   uint32_t cost;         /* Value when sold (gp.)               */
   uint32_t cost_per_day; /* Cost to keep pr. real day           */

   uint8_t  flags;      /* Various special object flags        */
   uint8_t  type;       /* Type of item (ITEM_XXX)             */
   uint8_t  equip_pos;  /* 0 or position of item in equipment  */
   uint8_t  resistance; /* Magic resistance                    */
};
