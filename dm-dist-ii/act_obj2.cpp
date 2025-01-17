#include "affect.h"
#include "comm.h"
#include "common.h"
#include "constants.h"
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

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* Remove the last name from the drink container */
void name_from_drinkcon(unit_data *obj)
{
   assert(OBJ_TYPE(obj) == ITEM_DRINKCON);

   if(UNIT_NAMES(obj).Length() < 2)
   {
      slog(LOG_OFF, 0, "Too few names in drinkcon");
      return;
   }

   UNIT_NAMES(obj).Remove(UNIT_NAMES(obj).Length() - 1);
}

/* Add a name to the end of a namelist */
void name_to_drinkcon(unit_data *obj, int type)
{
   /*  Notice:
    *  New name must be added in END of namelist to be properly removed
    */

   UNIT_NAMES(obj).AppendName((char *)drinks[type]);
}

static void apply_poison(unit_data *ch, int poison, int amount)
{
   unit_affected_type af;

   if(0 < poison && IS_MORTAL(ch))
   {
      act("Oops, it tasted rather strange ?!!?", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      act("$1n chokes and utters some strange sounds.", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);

      char mbuf[MAX_INPUT_LENGTH] = {0};
      spell_perform(SPL_POISON, MEDIA_SPELL, ch, ch, ch, mbuf, nullptr, amount * poison);
   }
}

/* Return TRUE if something was eaten/drunk */
auto drink_eat(unit_data *ch, unit_data *obj, int amount, const struct command_info *cmd, char *arg) -> bool
{
   int  extract = static_cast<int>(FALSE);
   int  poison  = 0; /* No poison */

   void gain_condition(unit_data * ch, int condition, int value);

   assert(IS_OBJ(obj));
   assert(OBJ_TYPE(obj) == ITEM_DRINKCON || OBJ_TYPE(obj) == ITEM_FOOD);
   assert(amount >= 0);

   if(OBJ_TYPE(obj) == ITEM_DRINKCON)
   {
      if(OBJ_VALUE(obj, 1) <= 0) /* Empty */
      {
         act("The $2N is empty.", A_SOMEONE, ch, obj, nullptr, TO_CHAR);
         return FALSE;
      }

      if(IS_PC(ch) && IS_MORTAL(ch))
      {
         if(PC_COND(ch, DRUNK) >= 24)
         {
            /* The pig is drunk */
            act("You simply fail to reach your mouth!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
            act("$1n tried to drink but missed $1s mouth!", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);
            return FALSE;
         }

         if(PC_COND(ch, THIRST) >= 24)
         {
            /* Stomach full */
            act("Your stomach can't contain anymore!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
            return FALSE;
         }
      }

      if(OBJ_VALUE(obj, 1) >= 0 && amount > OBJ_VALUE(obj, 1))
      {
         amount = OBJ_VALUE(obj, 1);
      }

      act("$1n drinks $3t from $2n.", A_HIDEINV, ch, obj, drinks[OBJ_VALUE(obj, 2)], TO_ROOM);

      act("You drink the $2t.", A_SOMEONE, ch, drinks[OBJ_VALUE(obj, 2)], nullptr, TO_CHAR);

      gain_condition(ch, DRUNK, (drink_aff[OBJ_VALUE(obj, 2)][DRUNK] * amount) / 4);
      gain_condition(ch, FULL, (drink_aff[OBJ_VALUE(obj, 2)][FULL] * amount) / 4);
      gain_condition(ch, THIRST, (drink_aff[OBJ_VALUE(obj, 2)][THIRST] * amount) / 4);

      poison = OBJ_VALUE(obj, 3);

      if(OBJ_VALUE(obj, 0) > 0)
      {
         /* empty the container, and no longer poison. */
         OBJ_VALUE(obj, 1) -= amount;
         weight_change_unit(obj, -amount); /* Subtract amount */

         if(OBJ_VALUE(obj, 1) <= 0)
         { /* The last bit */
            OBJ_VALUE(obj, 2) = 0;
            OBJ_VALUE(obj, 3) = 0;
            name_from_drinkcon(obj);
         }
      }
   }
   else /* FOOD */
   {
      if(IS_PC(ch) && CHAR_LEVEL(ch) < 200 && PC_COND(ch, FULL) >= 24)
      {
         act("You are too full to eat more!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
         return FALSE;
      }

      if(OBJ_VALUE(obj, 0) <= 0)
      {
         act("There is nothing left to eat.", A_SOMEONE, ch, obj, nullptr, TO_CHAR);
         return FALSE;
      }

      if(amount > OBJ_VALUE(obj, 0))
      {
         amount = OBJ_VALUE(obj, 0);
      }

      act("$1n eats $2n.", A_HIDEINV, ch, obj, nullptr, TO_ROOM);
      act("You eat $2n.", A_SOMEONE, ch, obj, nullptr, TO_CHAR);

      gain_condition(ch, FULL, amount);

      poison = OBJ_VALUE(obj, 3);

      OBJ_VALUE(obj, 0) -= amount;

      if(OBJ_VALUE(obj, 0) <= 0)
      {
         extract = static_cast<int>(TRUE);
      }
   }

   apply_poison(ch, poison, amount);

   if(IS_PC(ch))
   {
      if(PC_COND(ch, DRUNK) > 10)
      {
         act("You feel drunk.", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
      }

      if(PC_COND(ch, THIRST) < 15)
      {
         act("You still feel thirsty.", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
      }

      if(PC_COND(ch, FULL) < 15)
      {
         act("You still feel hungry.", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
      }
   }

   send_done(ch, obj, nullptr, poison, cmd, arg);
   if(extract != 0)
   {
      extract_unit(obj);
   }
   return TRUE;
}

void do_drink(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *drink;
   char      *oarg = arg;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("What do you want to drink?\n\r", ch);
   }
   else if((drink = find_unit(ch, &arg, nullptr, FIND_UNIT_HERE)) == nullptr)
   {
      act("You can't find it!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if(IS_OBJ(drink) && OBJ_TYPE(drink) == ITEM_DRINKCON)
   {
      int amount;

      if(OBJ_VALUE(drink, 0) < 0)
      { /* Infinite! */
         amount = number(3, 10);
      }
      else
      {
         amount = IS_GOD(ch) ? std::min(10, OBJ_VALUE(drink, 1)) : number(3, 10);
         amount = std::min(amount, OBJ_VALUE(drink, 1));
         if(amount < 0)
         { /* Straange... Only value "0" should be negative*/
            amount = 0;
         }
      }

      drink_eat(ch, drink, amount, cmd, oarg);
   }
   else
   {
      act("You can not drink from that.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
}

void do_eat(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *food;
   char      *oarg = arg;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("What do you want to eat?\n\r", ch);
   }
   else if((food = find_unit(ch, &arg, nullptr, FIND_UNIT_HERE)) == nullptr)
   {
      act("You've got no such food.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if(IS_OBJ(food) && OBJ_TYPE(food) == ITEM_FOOD)
   {
      drink_eat(ch, food, std::max(0, OBJ_VALUE(food, 0)), cmd, oarg);
   }
   else if(!IS_GOD(ch) || IS_ROOM(food))
   {
      act("Your stomach refuses to eat that!?!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else
   {
      act("$1n eats $2n.", A_HIDEINV, ch, food, nullptr, TO_ROOM);
      act("You eat $2n.", A_HIDEINV, ch, food, nullptr, TO_CHAR);
      extract_unit(food);
   }
}

void do_sip(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *drink;
   char      *oarg = arg;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   { /* No arguments */
      act("What do you want to sip?", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if((drink = find_unit(ch, &arg, nullptr, FIND_UNIT_HERE)) == nullptr)
   {
      act("You can't find such a thing.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if(IS_OBJ(drink) && OBJ_TYPE(drink) == ITEM_DRINKCON)
   {
      char *taste = (char *)drinks[OBJ_VALUE(drink, 2)];

      if(drink_eat(ch, drink, 1, cmd, oarg))
      {
         act("It tastes like $2t.", A_SOMEONE, ch, taste, nullptr, TO_CHAR);
      }
   }
   else
   {
      act("You can't sip from that.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
}

void do_taste(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *drink;
   char      *oarg = arg;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   { /* No arguments */
      act("What do you want to taste?", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if((drink = find_unit(ch, &arg, nullptr, FIND_UNIT_HERE)) == nullptr)
   {
      act("You can not find such a thing.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if(!IS_OBJ(drink))
   {
      act("You can't taste from that.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if(OBJ_TYPE(drink) != ITEM_FOOD || OBJ_TYPE(drink) != ITEM_DRINKCON)
   {
      act("Taste that?!? Your stomach refuses!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else
   {
      char taste[256]; /* strcpy because object may be extracted... */

      strcpy(taste, OBJ_TYPE(drink) == ITEM_DRINKCON ? drinks[OBJ_VALUE(drink, 2)] : UNIT_TITLE_STRING(drink));

      if(drink_eat(ch, drink, 1, cmd, oarg))
      {
         act("It tastes like $2t.", A_SOMEONE, ch, taste, nullptr, TO_CHAR);
      }
   }
}

void do_pour(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *from_obj;
   unit_data *to_obj;
   int        amount;
   char      *oarg = arg;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   { /* No arguments */
      act("What do you want to pour from?", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if((from_obj = find_unit(ch, &arg, nullptr, FIND_UNIT_HERE)) == nullptr)
   {
      act("You can't find it!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if(!(IS_OBJ(from_obj) && OBJ_TYPE(from_obj) == ITEM_DRINKCON))
   {
      act("You can not pour from that.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if(OBJ_VALUE(from_obj, 1) <= 0)
   {
      act("The $2N is empty.", A_SOMEONE, ch, from_obj, nullptr, TO_CHAR);
   }
   else if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   { /* No arguments */
      act("Where do you want it? Out or in what?", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
   else if((to_obj = find_unit(ch, &arg, nullptr, FIND_UNIT_HERE)) != nullptr)
   {
      if(to_obj == from_obj)
      {
         act("You can't pour it into itself!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      }
      else if(!(IS_OBJ(to_obj) && OBJ_TYPE(to_obj) == ITEM_DRINKCON))
      {
         act("You can't pour anything into that.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      }
      else if(OBJ_VALUE(to_obj, 1) != 0 && OBJ_VALUE(to_obj, 2) != OBJ_VALUE(from_obj, 2))
      {
         act("There is already another liquid in it!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      }
      else if(OBJ_VALUE(to_obj, 0) <= OBJ_VALUE(to_obj, 1))
      {
         act("There is no room for more.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      }
      else
      {
         act("You pour the $3t into $2n.", A_SOMEONE, ch, to_obj, drinks[OBJ_VALUE(from_obj, 2)], TO_CHAR);

         /* New alias */
         if(OBJ_VALUE(to_obj, 1) == 0)
         { /* No name */
            name_to_drinkcon(to_obj, OBJ_VALUE(from_obj, 2));
         }

         /* First same type liq. */
         OBJ_VALUE(to_obj, 2) = OBJ_VALUE(from_obj, 2);

         /* Then how much to pour */
         if(OBJ_VALUE(from_obj, 0) < 0) /* Infinity */
         {
            if(OBJ_VALUE(to_obj, 0) < 0)
            {
               amount = 0;
            }
            else
            {
               amount = OBJ_VALUE(to_obj, 0) - OBJ_VALUE(to_obj, 1);
            }
         }
         else if(OBJ_VALUE(to_obj, 0) < 0)
         { /* To infinity? */
            amount = OBJ_VALUE(from_obj, 0);
         }
         else
         {
            amount = OBJ_VALUE(to_obj, 0) - OBJ_VALUE(to_obj, 1);
            amount = std::min(OBJ_VALUE(from_obj, 1), amount);
            amount = std::max(0, amount); /* Just in case someone goofed with it */
         }

         assert(amount >= 0);

         if(OBJ_VALUE(from_obj, 0) >= 0) /* Check infinity */
         {
            OBJ_VALUE(from_obj, 1) -= amount;
            weight_change_unit(from_obj, -amount);
         }

         if(OBJ_VALUE(to_obj, 0) >= 0)
         {
            OBJ_VALUE(to_obj, 1) += amount;
            weight_change_unit(to_obj, amount);
         }

         /* Then the poison boogie */
         if(OBJ_VALUE(from_obj, 3) > OBJ_VALUE(to_obj, 3))
         {
            OBJ_VALUE(to_obj, 3) = number(1, OBJ_VALUE(from_obj, 3));
         }

         if(OBJ_VALUE(from_obj, 1) == 0) /* It is now empty */
         {
            OBJ_VALUE(from_obj, 2) = 0;
            OBJ_VALUE(from_obj, 3) = 0;

            name_from_drinkcon(from_obj);
         }

         send_done(ch, from_obj, to_obj, 0, cmd, oarg);
      }
   }
   else if(str_ccmp_next_word(arg, "out") != nullptr) /* See if it maybe is pour "out" */
   {
      if(UNIT_IN(from_obj) != ch)
      {
         act("You must carry it in order to empty it.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      }
      else if(OBJ_VALUE(from_obj, 0) <= -1)
      {
         act("It is impossible to empty $2n.", A_SOMEONE, ch, from_obj, nullptr, TO_CHAR);
      }
      else
      {
         act("$1n empties $2n.", A_HIDEINV, ch, from_obj, nullptr, TO_ROOM);
         act("You empty $2n.", A_SOMEONE, ch, from_obj, nullptr, TO_CHAR);

         weight_change_unit(from_obj, -OBJ_VALUE(from_obj, 1)); /* Empty */

         OBJ_VALUE(from_obj, 1) = 0;
         OBJ_VALUE(from_obj, 2) = 0;
         OBJ_VALUE(from_obj, 3) = 0;
         name_from_drinkcon(from_obj);
      }
      send_done(ch, from_obj, nullptr, 0, cmd, oarg);
   }
   else
   { /* It was not found, and it was not "out" */
      act("You can not find it.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
   }
}
