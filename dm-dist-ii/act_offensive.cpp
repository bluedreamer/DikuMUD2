#include "affect.h"
#include "comm.h"
#include "db.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "utility.h"
#include "utils.h"

#include <climits>
#include <cstdio>
#include <cstring>

/* extern variables */

void do_decapitate(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data              *corpse;
   unit_data              *head;
   unit_affected_type     *af;
   unit_affected_type      naf;
   char                   *c;
   char                   *d;
   char                    buf[256];
   char                    buf2[256];

   extern file_index_type *head_fi;

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("What corpse do you wish to decapitate?\n\r", ch);
      return;
   }

   corpse = find_unit(ch, &argument, nullptr, FIND_UNIT_INVEN | FIND_UNIT_SURRO);

   if(corpse == nullptr)
   {
      send_to_char("No such corpse around.\n\r", ch);
      return;
   }

   if(IS_CHAR(corpse))
   {
      act("Perhaps you should kill $3m first?", A_SOMEONE, ch, nullptr, corpse, TO_CHAR);
      return;
   }

   c = str_str(UNIT_OUT_DESCR_STRING(corpse), " corpse of ");
   d = str_str(UNIT_OUT_DESCR_STRING(corpse), " is here.");

   if(!IS_OBJ(corpse) || c == nullptr || d == nullptr)
   {
      act("Huh? That can't be done.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      return;
   }

   strncpy(buf, c + 11, d - (c + 11));
   buf[d - (c + 11)] = 0;

   act("You brutally decapitate the $2N.", A_SOMEONE, ch, corpse, nullptr, TO_CHAR);
   act("$1n brutally decapitates the $2N.", A_SOMEONE, ch, corpse, nullptr, TO_ROOM);

   head = read_unit(head_fi);

   UNIT_OUT_DESCR(corpse).Reassign("A beheaded corpse is left here.");

   sprintf(buf2, UNIT_OUT_DESCR_STRING(head), buf);
   UNIT_OUT_DESCR(head).Reassign(buf2);

   naf.id       = ID_CORPSE;
   naf.duration = 5;
   naf.beat     = WAIT_SEC * 60 * 5;
   naf.data[0]  = 0;
   naf.data[1]  = 0;
   naf.data[2]  = 0;
   naf.firstf_i = TIF_NONE;
   naf.tickf_i  = TIF_NONE;
   naf.lastf_i  = TIF_CORPSE_ZAP;
   naf.applyf_i = APF_NONE;

   create_affect(head, &naf);

   OBJ_VALUE(head, 4)   = OBJ_VALUE(corpse, 4); /* Copy RACE  */
   OBJ_VALUE(head, 3)   = OBJ_VALUE(corpse, 3); /* Copy LEVEL */
   OBJ_VALUE(corpse, 3) = 0;                    /* The corpse is not level-less (demigod reason)*/

   af                   = affected_by_spell(corpse, ID_REWARD);

   unit_to_unit(head, ch);
   if(af != nullptr)
   {
      create_affect(head, af);
      destroy_affect(af);
   }
}

void do_hit(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *victim;

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      act("Who do you want to hit?", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
      return;
   }

   victim = find_unit(ch, &argument, nullptr, FIND_UNIT_SURRO);

   if((victim == nullptr) || !IS_CHAR(victim))
   {
      act("There is nobody here called $2t which you can hit.", A_ALWAYS, ch, argument, nullptr, TO_CHAR);
      return;
   }

   if(victim == ch)
   {
      send_to_char("You hit yourself... OUCH!.\n\r", ch);
      act("$1n hits $1mself, and says OUCH!", A_SOMEONE, ch, nullptr, victim, TO_ROOM);
   }
   else
   {
#ifdef SUSPEKT
      if(CHAR_IS_AFFECTED(ch, CHAR_CHARM) && (CHAR_MASTER(ch) == victim))
      {
         act("$3n is just such a good friend, you simply can't hit $3m.", A_SOMEONE, ch, 0, victim, TO_CHAR);
         return;
      }
#endif
      if(pk_test(ch, victim, static_cast<int>(TRUE)) != 0)
      {
         return;
      }

      if(!CHAR_FIGHTING(ch))
      {
         simple_one_hit(ch, victim);
      }
      else
      {
         send_to_char("You do the best you can!\n\r", ch);
      }
   }
}

void do_kill(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *victim;

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      act("Who do you want to kill?", A_ALWAYS, ch, nullptr, nullptr, TO_CHAR);
      return;
   }

   if((CHAR_LEVEL(ch) < ULTIMATE_LEVEL) || IS_NPC(ch))
   {
      do_hit(ch, argument, &cmd_auto_unknown);
      return;
   }

   victim = find_unit(ch, &argument, nullptr, FIND_UNIT_SURRO);

   if((victim == nullptr) || !IS_CHAR(victim))
   {
      act("There is nobody here called $2t which you can kill.", A_ALWAYS, ch, argument, nullptr, TO_CHAR);
      return;
   }

   if(ch == victim)
   {
      send_to_char("Your mother would be so sad.. :(\n\r", ch);
   }
   else
   {
      act("You chop $3m to pieces! Ah! The blood!", A_SOMEONE, ch, nullptr, victim, TO_CHAR);
      act("$3n chops you to pieces!", A_SOMEONE, victim, nullptr, ch, TO_CHAR);
      act("$1n brutally slays $3n.", A_SOMEONE, ch, nullptr, victim, TO_NOTVICT);
      set_fighting(ch, victim, static_cast<int>(TRUE)); /* Point to the killer! */
      raw_kill(victim);
   }
}

void do_order(unit_data *ch, char *argument, const struct command_info *cmd)
{
#ifdef SUSPEKT
   char                name[100], message[256];
   char                buf[256];
   bool                found = FALSE;
   int                 org_room;
   unit_data          *victim;
   struct follow_type *k;

   half_chop(argument, name, message);

   if(!*name || !*message)
      send_to_char("Order who to do what?\n\r", ch);
   else if(!(victim = get_char_room_vis(ch, name)) && str_cmp("follower", name) && str_cmp("followers", name))
      send_to_char("That person isn't here.\n\r", ch);
   else if(ch == victim)
      send_to_char("You obviously suffer from skitzofrenia.\n\r", ch);

   else
   {
      if(IS_AFFECTED(ch, AFF_CHARM))
      {
         send_to_char("Your superior would not aprove of you giving orders.\n\r", ch);
         return;
      }

      if(victim)
      {
         act("$3n orders you to '$2t'", A_SOMEONE, victim, message, ch, TO_CHAR);
         act("$1n gives $3n an order.", A_SOMEONE, ch, 0, victim, TO_ROOM);

         if((victim->master != ch) || !IS_AFFECTED(victim, AFF_CHARM))
            act("$1n has an indifferent look.", A_SOMEONE, victim, 0, 0, TO_ROOM);
         else
         {
            send_to_char("Ok.\n\r", ch);
            command_interpreter(victim, message);
         }
      }
      else
      { /* This is order "followers" */
         act("$1n issues the order '$2t'.", A_SOMEONE, ch, message, victim, TO_ROOM);

         org_room = ch->in_room;

         for(k = ch->followers; k; k = k->next)
         {
            if(org_room == k->follower->in_room)
               if(IS_AFFECTED(k->follower, AFF_CHARM))
               {
                  found = TRUE;
                  command_interpreter(k->follower, message);
               }
         }
         if(found)
            send_to_char("Ok.\n\r", ch);
         else
            send_to_char("Nobody here are loyal subjects of yours!\n\r", ch);
      }
   }
#endif
}
