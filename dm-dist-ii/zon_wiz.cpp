#include "comm.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "justice.h"
#include "spells.h"
#include "textutil.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>

auto ball(struct spec_arg *sarg) -> int
{
#define TOP_MAX 10

   char      *arg = (char *)sarg->arg;
   char       buf[128];
   unit_data *u;
   unit_data *top[TOP_MAX];
   int        v1;
   int        v2;
   int        i;

   if(sarg->cmd->no == CMD_USE)
   {
      u = find_unit(sarg->activator, &arg, nullptr, FIND_UNIT_IN_ME);

      if(u != sarg->owner)
      {
         return SFR_SHARE;
      }

      arg = one_argument(arg, buf);
      v1  = atoi(buf);
      arg = one_argument(arg, buf);
      v2  = atoi(buf);
      if((v2 < 0) || (v2 > 4))
      {
         v2 = 0;
      }

      for(i = 0; i < TOP_MAX; top[i++] = nullptr)
      {
         ;
      }

      sprintf(buf, "Searching for Objects of type %d with max value[%d]\n\r", v1, v2);

      send_to_char(buf, sarg->activator);

      for(u = unit_list; u != nullptr; u = u->gnext)
      {
         if(IS_OBJ(u) && (OBJ_TYPE(u) == v1))
         {
            for(i = 0; i < TOP_MAX; i++)
            {
               if((top[i] == nullptr) || OBJ_VALUE(u, v2) > OBJ_VALUE(top[i], v2))
               {
                  top[i] = u;
                  break;
               }
            }
         }
      }

      for(i = 0; i < TOP_MAX; i++)
      {
         if(top[i] != nullptr)
         {
            sprintf(buf,
                    "%4ld %-15s@%-15s  IN  %s [%s@%s]\n\r",
                    (signed long)OBJ_VALUE(top[i], v2),
                    UNIT_FI_NAME(top[i]),
                    UNIT_FI_ZONENAME(top[i]),
                    UNIT_NAME(UNIT_IN(top[i])),
                    UNIT_FI_NAME(UNIT_IN(top[i])),
                    UNIT_FI_ZONENAME(UNIT_IN(top[i])));
            send_to_char(buf, sarg->activator);
         }
      }
      return SFR_BLOCK;
   }
   return SFR_SHARE;
#undef TOP_MAX
}
