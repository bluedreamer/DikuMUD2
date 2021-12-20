
#include "comm.h"
#include "db.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

auto hometown_unit(char *str) -> unit_data *
{
   extern unit_data *entry_room;

   if(str != nullptr)
   {
      char       name[80];
      char       zone[80];
      unit_data *u;

      split_fi_ref(str, zone, name);
      if((u = world_room(zone, name)) != nullptr)
      {
         return u;
      }
   }

   return entry_room;
}

auto change_hometown(spec_arg *sarg) -> int
{
   if(static_cast<unsigned int>(is_command(sarg->cmd, "moveto")) == 0U)
   {
      return SFR_SHARE;
   }

   send_to_char("Defunct at the moment...\n\r", sarg->activator);

   return SFR_BLOCK;
}
