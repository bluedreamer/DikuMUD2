#include "affect.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "skills.h"
#include "spells.h"
#include "structs.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "utility.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern file_index_type *corpse_fi;

auto make_corpse(unit_data *ch) -> unit_data *
{
   char                buf[MAX_INPUT_LENGTH];
   unit_data          *corpse;
   unit_data          *u;
   unit_data          *next_dude;
   unit_affected_type  af;
   unit_affected_type *taf1;

   void persist_create(unit_data * u);

   corpse = read_unit(corpse_fi);

   sprintf(buf, STR(UNIT_OUT_DESCR_STRING(corpse)), TITLENAME(ch));
   UNIT_OUT_DESCR(corpse).Reassign(buf);

   if(IS_PC(ch))
   {
      UNIT_NAMES(corpse).PrependName(str_cc("corpse of ", UNIT_NAME(ch)));
   }

   UNIT_WEIGHT(corpse)      = UNIT_BASE_WEIGHT(ch);
   UNIT_BASE_WEIGHT(corpse) = UNIT_WEIGHT(corpse);

   OBJ_TYPE(corpse)      = ITEM_CONTAINER;
   OBJ_PRICE(corpse)     = 0;
   OBJ_PRICE_DAY(corpse) = 0;

   /* *** pointers *** */

   for(u = UNIT_CONTAINS(ch); u != nullptr; u = u->next)
   {
      if(UNIT_IS_EQUIPPED(u))
      {
         unequip_object(u);
      }
   }

   for(u = UNIT_CONTAINS(ch); u != nullptr; u = next_dude)
   {
      next_dude = u->next;

      if(CHAR_LEVEL(ch) >= UNIT_MINV(u))
      {
         unit_from_unit(u);
         unit_to_unit(u, corpse);
      }
   }

   unit_to_unit(corpse, UNIT_IN(ch));

   af.id       = ID_CORPSE;
   af.duration = (IS_PC(ch) ? (2 * 60) : 5); /* In minutes, ply corpses 2hr */
   af.beat     = (WAIT_SEC * 60);
   af.data[0]  = 0;
   af.data[1]  = 0;
   af.data[2]  = 0;
   af.firstf_i = TIF_NONE;
   af.tickf_i  = TIF_CORPSE_DECAY;
   af.lastf_i  = TIF_CORPSE_ZAP;
   af.applyf_i = APF_NONE;

   create_affect(corpse, &af);

   OBJ_VALUE(corpse, 2) = IS_PC(ch) ? 1 : 0;
   OBJ_VALUE(corpse, 3) = CHAR_LEVEL(ch);
   OBJ_VALUE(corpse, 4) = CHAR_RACE(ch);

   if(IS_PC(ch))
   {
      if(UNIT_CONTAINS(corpse) != nullptr)
      {
         persist_create(corpse);
      }

      if(IS_SET(PC_FLAGS(ch), PC_PK_RELAXED))
      {
         UNIT_EXTRA_DESCR(corpse) = UNIT_EXTRA_DESCR(corpse)->add("$BOB", "");
      }
   }

   return corpse;
}
