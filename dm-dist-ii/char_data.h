#pragma once
#include "cCombat.h"
#include "char_follow_type.h"
#include "char_point_data.h"
#include "descriptor_data.h"

class char_data
{
public:
   char_data();
   ~char_data();

   union
   {
      class pc_data  *pc;
      class npc_data *npc;
   } specific;

   char *money; /*  Money transfer from db-files.
                 *  Converted to real money when pc/npc is loaded.
                 */

   descriptor_data *descriptor;

   char_point_data points;

   cCombat *Combat;

   struct char_follow_type *followers;
   unit_data               *master; /* Must be a char */

   unit_data *last_room; /* Last location of character */
};
