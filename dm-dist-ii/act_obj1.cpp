#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* Returns: 0 if the object was picked up.                               */
/*          1 if the object was not picked up, but more may be picked up */
/*          2 if no more objects can be picked up                        */
auto get(unit_data *ch, unit_data *obj, unit_data *from_obj, const struct command_info *cmd, char *arg) -> int
{
   unit_data *money  = nullptr;
   amount_t   amount = 0;
   int        weight;

   if(UNIT_IN(ch) == obj)
   {
      act("Pulling $3n from down under you seems quite impossible.", A_ALWAYS, ch, nullptr, obj, TO_CHAR);
      return 1;
   }

   if(IS_OBJ(obj) && (OBJ_EQP_POS(obj) != 0u))
   {
      act("The $3N is equipped, you can't take $3m.", A_SOMEONE, ch, nullptr, obj, TO_CHAR);
      return 1;
   }

   if(from_obj != nullptr)
   {
      /* The ENTER also flag replaces item type container */
      if(!UNIT_WEAR(from_obj, MANIPULATE_ENTER) && (!IS_OBJ(from_obj) || (OBJ_TYPE(from_obj) != ITEM_CONTAINER)))
      {
         act("$3e is not a container.", A_SOMEONE, ch, nullptr, from_obj, TO_CHAR);
         return 2;
      }
      if(IS_CHAR(from_obj))
      {
         act("$3e would probably object to that!", A_SOMEONE, ch, nullptr, from_obj, TO_CHAR);
         return 2;
      }
      if(IS_SET(UNIT_OPEN_FLAGS(from_obj), EX_CLOSED))
      {
         act("The $2N is closed.", A_SOMEONE, ch, from_obj, nullptr, TO_CHAR);
         return 2;
      }
   }

   if(ch == obj || !CHAR_CAN_SEE(ch, obj))
   {
      return 1;
   }

   if(IS_ROOM(obj) || !UNIT_WEAR(obj, MANIPULATE_TAKE))
   {
      act("$2N: You can't take that.", A_SOMEONE, ch, obj, nullptr, TO_CHAR);
      return 1;
   }

   if(!IS_MONEY(obj) && (char_can_carry_n(ch, 1) == 0))
   {
      act("$2n: You can not carry that many items.", A_SOMEONE, ch, obj, nullptr, TO_CHAR);
      return 2;
   }

   /*  Calculate how much this item will weigh extra for char
    *  Money is piled, so we have to calculate the two money-objects weightsum
    */
   if(IS_MONEY(obj) && ((money = unit_has_money_type(ch, MONEY_TYPE(obj))) != nullptr))
   {
      weight = MONEY_WEIGHT_SUM(money, obj);
   }
   else
   {
      weight = UNIT_WEIGHT(obj);
   }

   /*  Check if object will make a difference to what ch is carrying
    *    (i.e. isn't taken from within ch)
    *  If it will, check if ch can carry the extra weight
    *  If ch can't, and it's money, calculate how much of it ch CAN carry
    */
   if(from_obj == nullptr || UNIT_IN(from_obj) != ch)
   {
      if(char_can_carry_w(ch, weight) == 0)
      {
         if(!IS_MONEY(obj) || ((amount = char_can_carry_amount(ch, obj)) <= 0))
         {
            act("$2n: You can't carry that much weight.", A_SOMEONE, ch, obj, nullptr, TO_CHAR);
            return 1;
         }
      }
   }

   if(IS_MONEY(obj))
   {
      char        tmp[256];
      char        buf1[256];
      const char *buf2;

      /*  Amount is set above to how much ch CAN carry, or still 0
       *  If 0, ch can carry the object
       */

      if(amount > 0 && amount < (amount_t)MONEY_AMOUNT(obj))
      {
         sprintf(tmp,
                 "You manage to take %s%%s.\n\rAnything more would "
                 "break your back!",
                 obj_money_string(obj, amount));
      }
      else
      {
         amount = MONEY_AMOUNT(obj);

         if(amount > 0)
         {
            strcpy(tmp, "You get $2t%s.");
         }
         else
         {
            strcpy(tmp, "You get $2t%s but discover that they are fake!");
            amount = 0;
         }
      }

      if(from_obj != nullptr)
      {
         sprintf(buf1, tmp, " from $3n");
         buf2 = UNIT_IN(from_obj) == ch ? "$1n gets some $2t from $1s $3N." : "$1n gets some $2t from $3n.";
      }
      else
      {
         sprintf(buf1, tmp, "");
         buf2 = "$1n gets some $2t.";
      }

      act(buf1, A_SOMEONE, ch, obj_money_string(obj, 0), from_obj, TO_CHAR);
      act(buf2, A_HIDEINV, ch, money_pluralis(obj), from_obj, TO_ROOM);

      if(amount == 0)
      {
         extract_unit(obj); /* Destroy the faked money */
         /* Dont send done... it is destoryed... ?! */
         return 0; /* Object was "picked up" */
      }
      if(amount < (amount_t)MONEY_AMOUNT(obj))
      {
         obj = split_money(obj, amount);
      }
   }
   else
   {
      if(from_obj != nullptr)
      {
         act("You get $2n from $3n.", A_SOMEONE, ch, obj, from_obj, TO_CHAR);
         act(UNIT_IN(from_obj) == ch ? "$1n gets $2n from $1s $3N." : "$1n gets $2n from $3n.", A_HIDEINV, ch, obj, from_obj, TO_ROOM);
      }
      else
      {
         unit_messg(ch, obj, "$get", "You get $2n.", "$1n gets $2n.");
      }
   }

   unit_from_unit(obj);
   unit_to_unit(obj, ch);

   send_done(ch, from_obj, obj, 0, cmd, arg);

   return 0; /* Object was picked up */
}

/* searched for "$get" entries in room only
   should be extended to run through
   all visible units.
*/
auto extra_get(unit_data *ch, char *argument) -> int
{
   extra_descr_data *p;
   unit_data        *room = UNIT_IN(ch);

   if(!IS_ROOM(room) && UNIT_IS_TRANSPARENT(room))
   {
      room = UNIT_IN(room);
   }

   if(room == nullptr)
   {
      return 0;
   }

   if(!IS_ROOM(room))
   {
      return 0;
   }

   if(UNIT_IS_DARK(room))
   {
      return 0;
   }

   for(p = UNIT_EXTRA_DESCR(room); p != nullptr; p = p->next)
   {
      if(p->names.Length() >= 2 && (strcmp("$get", p->names.Name(0)) == 0) && (p->names.IsName(argument) != nullptr)) // XXX
      {
         act(p->descr.String(), A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
         return 1;
      }
   }

   return 0; /* not found */
}

void do_get(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *from_unit = nullptr;
   unit_data *thing;
   char       arg1[MAX_INPUT_LENGTH];
   char      *arg2;
   char      *oarg = argument;

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("Get what?\n\r", ch);
      return;
   }

   /* See if it is "from bag" or the like */
   if((arg2 = str_cstr(argument, " from ")) != nullptr)
   {
      strncpy(arg1, argument, arg2 - argument); /* isolate 1st argument too */
      arg1[arg2 - argument] = '\0';

      arg2 += 6; /* Point past from */

      from_unit = find_unit(ch, &arg2, nullptr, FIND_UNIT_HERE);

      if(from_unit == nullptr)
      {
         act("No such thing here to get things from.", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
         return;
      }
      if(IS_CHAR(from_unit))
      {
         act("$3e would probably object to that!", A_ALWAYS, ch, nullptr, from_unit, TO_CHAR);
         return;
      }
   }
   else
   {
      strcpy(arg1, argument);
   }

   if(str_ccmp_next_word(arg1, "all") != nullptr)
   {
      unit_data *next_unit;
      bool       ok   = TRUE;
      bool       pick = FALSE;

      thing           = from_unit != nullptr ? UNIT_CONTAINS(from_unit) : UNIT_CONTAINS(UNIT_IN(ch));

      for(; ok && (thing != nullptr); thing = next_unit)
      {
         next_unit = thing->next;

         /* To avoid endless "can't take that" messages */
         if(UNIT_MINV(thing) > CHAR_LEVEL(ch))
         {
            continue;
         }

         if(UNIT_WEAR(thing, MANIPULATE_TAKE))
         {
            int tmp_get = get(ch, thing, from_unit, cmd, oarg);
            ok          = (tmp_get != 2);         /* If stop */
            pick        = (pick || tmp_get == 0); /* Anything picked up? */
         }
      }

      if(!pick)
      {
         send_to_char("Nothing to get.\n\r", ch);
      }
   }
   else if(str_ccmp_next_word(arg1, "money") != nullptr)
   {
      unit_data *next_unit;
      bool       ok   = TRUE;
      bool       pick = FALSE;

      thing           = from_unit != nullptr ? UNIT_CONTAINS(from_unit) : UNIT_CONTAINS(UNIT_IN(ch));

      for(; ok && (thing != nullptr); thing = next_unit)
      {
         next_unit = thing->next;

         if(IS_MONEY(thing))
         {
            int tmp_get = get(ch, thing, from_unit, cmd, oarg);
            ok          = (tmp_get != 2);         /* If stop */
            pick        = (pick || tmp_get == 0); /* Anything picked up? */
         }
      }

      if(!pick)
      {
         send_to_char("No money to get.\n\r", ch);
      }
   }
   else /* Single object */
   {
      char    *tmp = arg1;
      char     arg[MAX_INPUT_LENGTH];
      amount_t amount = 0;

      if(static_cast<unsigned int>(next_word_is_number(arg1)) != 0U)
      {
         tmp = str_next_word(arg1, arg);
         if((amount = (amount_t)atoi(arg)) < 1)
         {
            send_to_char("That's a silly thing to do.\n\r", ch);
            return;
         }
      }

      thing = from_unit != nullptr ? find_unit(ch, &tmp, UNIT_CONTAINS(from_unit), 0) : find_unit(ch, &tmp, nullptr, FIND_UNIT_SURRO);

      if(thing != nullptr)
      {
         if(IS_MONEY(thing))
         {
            if(amount == 0)
            { /* No number was specified: Take all */
               amount = MONEY_AMOUNT(thing);
            }
            else if((amount_t)MONEY_AMOUNT(thing) < amount)
            {
               act("There aren't that many $3t!", A_ALWAYS, ch, nullptr, money_pluralis(thing), TO_CHAR);
               return;
            }
            thing = split_money(thing, amount);
         }
         else if(amount > 0)
         {
            send_to_char("Sorry, you can't do that...\n\r", ch);
            return;
         }
         get(ch, thing, from_unit, cmd, oarg);

         /* To catch get-aborts, or remaining money */
         if(is_destructed(DR_UNIT, thing) == 0)
         {
            if(IS_MONEY(thing))
            {
               pile_money(thing);
            }
         }
      }
      else if(extra_get(ch, tmp) == 0)
      {
         act("No such thing here.", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
      }
   }
}

auto drop(unit_data *ch, unit_data *unit, const struct command_info *cmd, char *arg) -> int
{
   if(!(IS_OBJ(unit) && (OBJ_EQP_POS(unit) != 0u)))
   {
      if(IS_MONEY(unit))
      {
         act("You drop $2t.", A_SOMEONE, ch, obj_money_string(unit, 0), nullptr, TO_CHAR);
         act("$1n drops some $2t.", A_SOMEONE, ch, money_pluralis(unit), nullptr, TO_ROOM);
      }
      else
      {
         unit_messg(ch, unit, "$drop", "You drop $2n.", "$1n drops $2n.");
      }

      unit_up(unit);
      send_done(ch, unit, nullptr, 0, cmd, arg);
      return static_cast<int>(TRUE);
   }
   return static_cast<int>(FALSE);
}

void do_drop(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *thing;
   unit_data *next_obj;
   char       arg[MAX_INPUT_LENGTH];
   amount_t   amount = 0;
   char      *oarg   = argument;

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("Drop what?\n\r", ch);
      return;
   }

   if(str_ccmp_next_word(argument, "all") != nullptr) /* Drop All */
   {
      int anything = static_cast<int>(FALSE);

      for(thing = UNIT_CONTAINS(ch); thing != nullptr; thing = next_obj)
      {
         next_obj = thing->next;

         if(CHAR_LEVEL(ch) < UNIT_MINV(thing))
         {
            continue;
         }

         if(!IS_MONEY(thing))
         {
            anything |= drop(ch, thing, cmd, oarg);
         }
      }

      if(anything == 0)
      {
         send_to_char("You are not carrying anything.\n\r", ch);
         return;
      }
   }
   else if(str_ccmp_next_word(argument, "money") != nullptr) /* Drop money */
   {
      bool got_any = FALSE;

      for(thing = UNIT_CONTAINS(ch); thing != nullptr; thing = next_obj)
      {
         next_obj = thing->next;
         if(IS_MONEY(thing))
         {
            drop(ch, thing, cmd, oarg);
            got_any = TRUE;
         }
      }

      if(!got_any)
      {
         send_to_char("You are not carrying any money.\n\r", ch);
      }
   }
   else /* Drop Single Object */
   {
      if(static_cast<unsigned int>(next_word_is_number(argument)) != 0U)
      {
         argument = str_next_word(argument, arg);
         if((amount = atoi(arg)) < 1)
         {
            send_to_char("That's a silly thing to do.\n\r", ch);
            return;
         }
      }

      if((thing = find_unit(ch, &argument, nullptr, FIND_UNIT_INVEN)) != nullptr)
      {
         if(IS_MONEY(thing))
         {
            if(amount == 0)
            { /* No number was specified: Drop all */
               amount = MONEY_AMOUNT(thing);
            }
            else if((amount_t)MONEY_AMOUNT(thing) < amount)
            {
               act("You haven't got that many $3t!", A_ALWAYS, ch, nullptr, money_pluralis(thing), TO_CHAR);
               return;
            }
            thing = split_money(thing, amount);
         }
         else if(amount > 0)
         {
            send_to_char("Sorry, you can't do that (yet)...\n\r", ch);
            return;
         }
         drop(ch, thing, cmd, oarg);
      }
      else
      {
         send_to_char("You are not carrying anything like that.\n\r", ch);
      }
   }
}

/* Returns FALSE if there was an "error" putting the stuff */
auto put(unit_data *ch, unit_data *unit, unit_data *tounit, const struct command_info *cmd, char *arg) -> int
{
   unit_data *money = nullptr;
   int        weight;
   int32_t    amt = 0;

   /* Curse check if carrying? */

   if(!IS_SET(UNIT_MANIPULATE(tounit), MANIPULATE_ENTER))
   {
      act("You can't put anything in the $2N.", A_ALWAYS, ch, tounit, nullptr, TO_CHAR);
      return static_cast<int>(FALSE);
   }

   if(unit == tounit)
   {
      act("You attempt to fold the $2N into $2mself, but fail.", A_ALWAYS, ch, tounit, nullptr, TO_CHAR);
      return static_cast<int>(FALSE);
   }

   if(IS_SET(UNIT_OPEN_FLAGS(tounit), EX_CLOSED))
   {
      act("The $2N is closed.", A_ALWAYS, ch, tounit, nullptr, TO_CHAR);
      return static_cast<int>(FALSE);
   }

   if(IS_MONEY(unit) && ((money = unit_has_money_type(tounit, MONEY_TYPE(unit))) != nullptr))
   {
      weight = MONEY_WEIGHT_SUM(money, unit);
   }
   else
   {
      weight = UNIT_WEIGHT(unit);
   }

   if(UNIT_CONTAINING_W(tounit) + weight > UNIT_CAPACITY(tounit))
   {
      if(IS_MONEY(unit) && (amt = unit_can_hold_amount(tounit, unit)) > 0)
      {
         unit = split_money(unit, amt);
      }
      else
      {
         act("The $2N can not contain any more.", A_ALWAYS, ch, tounit, nullptr, TO_CHAR);
         return static_cast<int>(FALSE);
      }
   }

   if(amt > 0)
   {
      act("You manage to squeeze $2t into the $3N.", A_SOMEONE, ch, obj_money_string(unit, 0), tounit, TO_CHAR);
   }
   else
   {
      act("You put $3n into the $2N.", A_ALWAYS, ch, tounit, unit, TO_CHAR);
   }

   if(IS_MONEY(unit))
   {
      act(UNIT_IN(tounit) == ch ? "$1n puts some $2t into $1s $3N." : "$1n puts some $2t into the $3N.",
          A_HIDEINV,
          ch,
          money_pluralis(unit),
          tounit,
          TO_ROOM);
   }
   else
   {
      act(UNIT_IN(tounit) == ch ? "$1n puts $2n into $1s $3N." : "$1n puts $2n into the $3N.", A_HIDEINV, ch, unit, tounit, TO_ROOM);
   }

   unit_from_unit(unit);
   unit_to_unit(unit, tounit);

   send_done(ch, unit, tounit, 0, cmd, arg);

   return static_cast<int>(TRUE);
}

void do_put(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *tounit;
   unit_data *thing;
   char      *arg2;
   char       arg1[MAX_INPUT_LENGTH];
   char       buf[MAX_INPUT_LENGTH];
   amount_t   amount = 0;
   int        all;
   char      *oarg = argument;

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("Put what in what?\n\r", ch);
      return;
   }

   /* See if it is "in bag" or the like */
   if((arg2 = str_cstr(argument, " in ")) == nullptr)
   {
      send_to_char("Put 'in' what?\n\r", ch);
      return;
   }

   strncpy(arg1, argument, arg2 - argument); /* isolate 1st argument too */
   arg1[arg2 - argument] = '\0';

   arg2 += 4; /* Point past " in " */

   tounit = find_unit(ch, &arg2, nullptr, FIND_UNIT_HERE);

   if(tounit == nullptr)
   {
      send_to_char("No such thing to put stuff in here.\n\r", ch);
      return;
   }
   if(IS_CHAR(tounit))
   {
      act("$2e would probably object to that!", A_SOMEONE, ch, tounit, nullptr, TO_CHAR);
      return;
   }

   if(static_cast<unsigned int>(next_word_is_number(argument)) != 0U)
   {
      argument = str_next_word(argument, buf);
      if((amount = (amount_t)atoi(buf)) < 1)
      {
         send_to_char("That's a silly thing to do.\n\r", ch);
         return;
      }
   }

   all = static_cast<int>(str_ccmp_next_word(argument, "all") != nullptr);

   if(all != 0)
   {
      unit_data *nextu;

      for(thing = UNIT_CONTAINS(ch); thing != nullptr; thing = nextu)
      {
         nextu = thing->next;
         if((thing != tounit) && !IS_MONEY(thing) && !UNIT_IS_EQUIPPED(thing) && (UNIT_MINV(thing) <= UNIT_MINV(ch)))
         {
            if(put(ch, thing, tounit, cmd, oarg) == 0)
            {
               break;
            }
         }
      }
   }
   else if((thing = find_unit(ch, &argument, nullptr, FIND_UNIT_INVEN)) != nullptr)
   {
      if(IS_MONEY(thing))
      {
         if(amount == 0)
         {
            amount = MONEY_AMOUNT(thing);
         }
         else if((amount_t)MONEY_AMOUNT(thing) < amount)
         {
            act("You haven't got that many $3t!", A_ALWAYS, ch, nullptr, money_pluralis(thing), TO_CHAR);
            return;
         }
         thing = split_money(thing, amount);
      }
      else if(amount > 0)
      {
         send_to_char("Sorry, you can't do that... (yet)\n\r", ch);
         return;
      }

      put(ch, thing, tounit, cmd, oarg);

      /* To catch put-aborts, or remaining money */
      if(is_destructed(DR_UNIT, thing) == 0)
      {
         if(IS_MONEY(thing))
         {
            pile_money(thing);
         }
      }
   }
   else
   {
      send_to_char("No such thing to put here.\n\r", ch);
   }
}

void give(unit_data *ch, unit_data *thing, unit_data *vict, const struct command_info *cmd, char *arg)
{
   unit_data *money = nullptr;
   int        weight;

   if(!IS_CHAR(vict))
   {
      send_to_char("That is not a person.\n\r", ch);
      return;
   }

   /* NB! Curse Check? */

   if(!IS_MONEY(thing) && (char_can_carry_n(vict, 1) == 0))
   {
      act("$3e seems to have $3s hands full.", A_SOMEONE, ch, nullptr, vict, TO_CHAR);
      return;
   }

   if(IS_MONEY(thing) && ((money = unit_has_money_type(vict, MONEY_TYPE(thing))) != nullptr))
   {
      weight = MONEY_WEIGHT_SUM(money, thing);
   }
   else
   {
      weight = UNIT_WEIGHT(thing);
   }

   if(char_can_carry_w(vict, weight) == 0)
   {
      act("$3e can't carry that much weight.", A_SOMEONE, ch, nullptr, vict, TO_CHAR);
      return;
   }

   if(IS_MONEY(thing))
   {
      char *tmp = obj_money_string(thing, 0);
      act("You give $2t to $3n.", A_SOMEONE, ch, tmp, vict, TO_CHAR);
      act("$1n gives some $2t to $3n.", A_HIDEINV, ch, money_pluralis(thing), vict, TO_NOTVICT);
      act("$1n gives you $2t.", A_SOMEONE, ch, tmp, vict, TO_VICT);
   }
   else
   {
      act("You give $2n to $3n.", A_SOMEONE, ch, thing, vict, TO_CHAR);
      act("$1n gives $2n to $3N.", A_HIDEINV, ch, thing, vict, TO_NOTVICT);
      act("$1n gives you $2n.", A_SOMEONE, ch, thing, vict, TO_VICT);
   }

   unit_from_unit(thing);
   unit_to_unit(thing, vict);

   send_done(ch, thing, vict, 0, cmd, arg);
}

void do_give(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *victim;
   unit_data *thing;
   char       buf[MAX_INPUT_LENGTH];
   amount_t   amount = 0;
   char      *oarg   = argument;

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("Give? Just give? Give what to whom?\n\r", ch);
      return;
   }

   if(static_cast<unsigned int>(next_word_is_number(argument)) != 0U)
   {
      argument = str_next_word(argument, buf);
      if((amount = (amount_t)atoi(buf)) < 1)
      {
         send_to_char("That's a silly thing to do.\n\r", ch);
         return;
      }
   }

   if((thing = find_unit(ch, &argument, nullptr, FIND_UNIT_INVEN)) == nullptr)
   {
      send_to_char("You don't seem to have any such thing.\n\r", ch);
      return;
   }

   if((victim = find_unit(ch, &argument, nullptr, FIND_UNIT_SURRO)) == nullptr)
   {
      act("No such person '$2t' here.", A_ALWAYS, ch, argument, nullptr, TO_CHAR);
      return;
   }

   if(IS_MONEY(thing))
   {
      if(amount == 0)
      {
         amount = MONEY_AMOUNT(thing);
      }
      else if((amount_t)MONEY_AMOUNT(thing) < amount)
      {
         act("You haven't got that many $3t!", A_ALWAYS, ch, nullptr, money_pluralis(thing), TO_CHAR);
         return;
      }

      thing = split_money(thing, amount);
   }
   else if(amount > 0)
   {
      send_to_char("Sorry, you can't do that...(yet)\n\r", ch);
      return;
   }

   give(ch, thing, victim, cmd, oarg);

   /* To catch give-aborts, or remaining money */
   if(is_destructed(DR_UNIT, thing) == 0)
   {
      if(IS_MONEY(thing))
      {
         pile_money(thing);
      }
   }
}
