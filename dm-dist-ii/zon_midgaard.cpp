#include "affect.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "justice.h"
#include "money.h"
#include "skills.h"
#include "spells.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

/*   external vars  */
extern descriptor_data *descriptor_list;

/* extern procedures */
auto                    obj_trade_price(unit_data *u) -> amount_t;

/* ------------------------------------------------------------------------- */
/*                     M O B I L E   R O U T I N E S                         */
/* ------------------------------------------------------------------------- */

auto                    fido(struct spec_arg *sarg) -> int
{
   unit_data *i;
   unit_data *temp;
   unit_data *next_obj;

   if(sarg->cmd->no != CMD_AUTO_TICK || !CHAR_IS_READY(sarg->owner))
   {
      return SFR_SHARE;
   }

   for(i = UNIT_CONTAINS(UNIT_IN(sarg->owner)); i != nullptr; i = i->next)
   {
      if(i != sarg->owner)
      {
         if(IS_CHAR(i) && (number(0, 5000) == 0))
         {
            char mbuf[MAX_INPUT_LENGTH] = {0};
            switch(number(1, 3))
            {
               case 1:
                  strcpy(mbuf, "fart");
                  command_interpreter(sarg->owner, mbuf);
                  break;

               case 2:
                  strcpy(mbuf, "drool");
                  command_interpreter(sarg->owner, mbuf);
                  break;

               case 3:
                  act("$1n wets on $3n.", A_SOMEONE, sarg->owner, nullptr, i, TO_ROOM);
                  break;
            }
         }
         else
         {
            if(IS_OBJ(i) && OBJ_TYPE(i) == ITEM_CONTAINER && (affected_by_spell(i, ID_CORPSE) != nullptr) &&
               !IS_SET(UNIT_FLAGS(i), UNIT_FL_BURIED))
            {
               act("$1n savagely devour a corpse.", A_SOMEONE, sarg->owner, nullptr, nullptr, TO_ROOM);
               for(temp = UNIT_CONTAINS(i); temp != nullptr; temp = next_obj)
               {
                  next_obj = temp->next;
                  unit_up(temp);
               }
               extract_unit(i);
               return SFR_BLOCK;
            }
         }
      }
   }
   return SFR_SHARE;
}

auto janitor(struct spec_arg *sarg) -> int
{
   unit_data *i;

   if(sarg->cmd->no != CMD_AUTO_TICK || !CHAR_IS_READY(sarg->owner))
   {
      return SFR_SHARE;
   }

   for(i = UNIT_CONTAINS(UNIT_IN(sarg->owner)); i != nullptr; i = i->next)
   {
      if(UNIT_WEAR(i, MANIPULATE_TAKE) && UNIT_WEIGHT(i) <= 100 && IS_OBJ(i) && (OBJ_TYPE(i) == ITEM_DRINKCON || obj_trade_price(i) <= 10))
      {
         act("$1n picks up some trash.", A_SOMEONE, sarg->owner, nullptr, nullptr, TO_ROOM);
         unit_down(i, sarg->owner);
         extract_unit(i); /* Not sure this is fair... */
         return SFR_BLOCK;
      }
   }

   return SFR_SHARE;
}

auto evaluate(struct spec_arg *sarg) -> int
{
   char      *arg = (char *)sarg->arg;
   unit_data *u1;
   char       buf[MAX_STRING_LENGTH];
   amount_t   cost;
   currency_t currency;
   int        craft;
   int        category;

   if(sarg->cmd->no != CMD_AUTO_UNKNOWN || (static_cast<unsigned int>(is_abbrev(sarg->cmd->cmd_str, "evaluate")) == 0U))
   {
      return SFR_SHARE;
   }

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      act("$1n says, 'What item do you wish to evaluate $3n?", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
      return SFR_BLOCK;
   }

   u1 = find_unit(sarg->activator, &arg, nullptr, FIND_UNIT_IN_ME);
   if(u1 == nullptr)
   {
      act("$1n says, 'You do not have such an item $3n?", A_SOMEONE, sarg->owner, nullptr, sarg->activator, TO_ROOM);
      return SFR_BLOCK;
   }

   cost     = sarg->fptr->data != nullptr ? atoi((char *)sarg->fptr->data) : 50;

   currency = local_currency(sarg->owner);

   if(static_cast<unsigned int>(char_can_afford(sarg->activator, cost, currency)) == 0U)
   {
      act("$1n says, 'The cost is merely $2t, get them first.'",
          A_SOMEONE,
          sarg->owner,
          money_string(cost, currency, TRUE),
          nullptr,
          TO_ROOM);
      return SFR_BLOCK;
   }

   if(!IS_OBJ(u1) || (OBJ_TYPE(u1) != ITEM_WEAPON && OBJ_TYPE(u1) != ITEM_ARMOR && OBJ_TYPE(u1) != ITEM_SHIELD))
   {
      act("$1n says, 'The $2N is neither a sword, shield nor armor!", A_SOMEONE, sarg->owner, u1, sarg->activator, TO_ROOM);
      return SFR_BLOCK;
   }

   category = OBJ_VALUE(u1, 0);
   craft    = OBJ_VALUE(u1, 1);

   switch(OBJ_TYPE(u1))
   {
      case ITEM_WEAPON:
         sprintf(buf, "$1n says, 'The $2N is a %s of %s craftsmanship and material.", wpn_text[category], skill_text(how_good, craft));
         break;

      case ITEM_ARMOR:
         sprintf(buf, "$1n says, 'The $2N is a %s of %s craftsmanship and material.", arm_text[category], skill_text(how_good, craft));
         break;

      case ITEM_SHIELD:
         sprintf(buf, "$1n says, 'The $2N is a %s of %s craftsmanship and material.", shi_text[category], skill_text(how_good, craft));
         break;
   }

   act(buf, A_SOMEONE, sarg->owner, u1, nullptr, TO_ROOM);
   money_transfer(sarg->activator, sarg->owner, cost, currency);

   return SFR_BLOCK;
}
