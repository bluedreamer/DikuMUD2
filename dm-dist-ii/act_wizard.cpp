/* *********************************************************************** *
 * File   : act_wizard.c                              Part of Valhalla MUD *
 * Version: 1.25                                                           *
 * Author : All.                                                           *
 *                                                                         *
 * Purpose: Implementation of wizard commands.                             *
 *                                                                         *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

/* 19/07/92 seifert: Recursive trans bug fixed.                            */
/* 26/07/92 seifert: Fixed do_purge to match new destruct system           */
/* 13/10/92 seifert: Fixed 'played:' time in do_stat                       */
/* 13/10/92 seifert: Fixed 'do_advance' to allow lower than 255 to advance */
/* 22/01/93  HHS added 'stat zone data' to give reset info (may purge you) */
/* Sun Jun 27 1993 HHS added stat world                                    */
/* Tue Jul 6 1993 HHS: added exchangable lib dir                           */
/* 23/08/93 jubal  : Load puts takable things in inventory                 */
/* 23/08/93 jubal  : Purge looks in inventory first                        */
/* 23/08/93 jubal  : Purge room doesn't remove non-takable items           */
/* 23/08/93 jubal  : Stat room data gives info on exits, and less lines    */
/* 29/08/93 jubal  : Expanded stat zone                                    */
/* 29/08/93 jubal  : Expanded goto to go to zones or enterable units       */
/* 25/02/94 gnort  : Fixed missing functionality/crashbug in stat zone     */
/* 11/08/94 gnort  : got rid of cras and shutdow                           */
/* 10/02/95 gnort  : Made do_users dynamic                                 */

#include "affect.h"
#include "blkfile.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "db_file.h"
#include "dijkstra.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "magic.h"
#include "main.h"
#include "money.h"
#include "nanny.h"
#include "skills.h"
#include "spells.h"
#include "structs.h"
#include "system.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/*   external vars  */

/* from dikumud.c */
extern BLK_FILE *inven_bf;

/* external functs */

auto age(unit_data *ch) -> struct time_info_data;
auto real_time_passed(time_t t2, time_t t1) -> struct time_info_data;

static int WIZ_CMD_LEVEL = 210; /* No need to change this, it is also set
                                   at runtime... */

auto player_exists(const char *pName) -> int;
auto delete_player(const char *name) -> int;

void do_path(unit_data *ch, char *argument, const struct command_info *cmd)
{
   int        i;
   unit_data *thing;
   char       buf[MAX_INPUT_LENGTH];
   char       zone[MAX_INPUT_LENGTH];

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("Path where to? (symbolic / findunit)\n\r", ch);
      return;
   }

   if((thing = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD)) == nullptr)
   {
      str_next_word(argument, argument);
      split_fi_ref(argument, zone, buf);

      if((thing = find_symbolic(zone, buf)) == nullptr)
      {
         send_to_char("No such thing anywhere.\n\r", ch);
         return;
      }
   }

   i = move_to(unit_room(ch), unit_room(thing));

   sprintf(buf, "Status: %d\n\r", i);
   send_to_char(buf, ch);
}

void do_users(unit_data *ch, char *argument, const struct command_info *cmd)
{
   static char *buf      = nullptr;
   static int   cur_size = 1024;

   descriptor_data *d;
   char             tmp[256];
   int              len;
   int              users = 0;

   if(buf == nullptr)
   {
      CREATE(buf, char, cur_size);
   }

   strcpy(buf, "Connections:\n\r------------\n\r");
   len = strlen(buf);

   for(d = descriptor_list; d != nullptr; d = d->next)
   {
      assert(d->character);
      if(CHAR_LEVEL(ch) >= UNIT_MINV(CHAR_ORIGINAL(d->character)))
      {
         users++;
         sprintf(tmp, "<%3d/%3d> %-16s %-10s [%c %4d %-3s %s]\n\r", CHAR_LEVEL(CHAR_ORIGINAL(d->character)),
                 UNIT_MINV(CHAR_ORIGINAL(d->character)), UNIT_NAME(CHAR_ORIGINAL(d->character)),
                 descriptor_is_playing(d) != 0 ? "Playing" : "Menu", g_cServerConfig.FromLAN(d->host) != 0 ? 'L' : 'W', d->nPort,
                 d->nLine == 255 ? "---" : itoa(d->nLine), d->host);

         len += strlen(tmp);
         if(cur_size < len + 1)
         {
            cur_size *= 2;
            RECREATE(buf, char, cur_size);
         }
         strcat(buf, tmp);
      }
   }

   sprintf(tmp, "\n\r%d players connected, %d players playing.\n\r", users, no_players);

   len += strlen(tmp);
   if(cur_size < len + 1)
   {
      cur_size *= 2;
      RECREATE(buf, char, cur_size);
   }

   strcat(buf, tmp);

   page_string(CHAR_DESCRIPTOR(ch), buf);
   free(buf);
}

/* Reset the zone in which the char is in! */
void do_reset(unit_data *ch, char *arg, const struct command_info *cmd)
{
   struct zone_type *zone;

   auto zone_reset(struct zone_type *)->int;

   if(static_cast<unsigned int>(str_is_empty(arg)) == 0U)
   {
      send_to_char("You can only reset the zone you are in.\n\r", ch);
      return;
   }

   if((zone = unit_zone(ch)) == nullptr)
   {
      send_to_char("You are inside no zone to reset!\n\r", ch);
      return;
   }

   send_to_char("Ok.\n\r", ch);
   zone_reset(zone);
}

void do_echo(unit_data *ch, char *arg, const struct command_info *cmd)
{
   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Echo needs an argument.\n\r", ch);
   }
   else
   {
      act("$2t", A_ALWAYS, ch, arg, nullptr, TO_ALL);
   }
}

auto frozen(struct spec_arg *sarg) -> int
{
   if(sarg->activator != sarg->owner)
   {
      return SFR_SHARE;
   }

   switch(sarg->cmd->no)
   {
      case CMD_LOOK:
      case CMD_WHO:
         return SFR_SHARE;
         break;

      default:
         send_to_char("You're totally frozen\n\r", sarg->owner);
   }

   return SFR_BLOCK;
}

void do_freeze(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *unit;
   unit_fptr *fptr;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Freeze who?\n\r", ch);
   }
   else if(((unit = find_unit(ch, &arg, nullptr, FIND_UNIT_SURRO | FIND_UNIT_WORLD)) == nullptr) || !IS_PC(unit))
   {
      send_to_char("No such person around./n/r", ch);
   }
   else if(ch == unit)
   {
      send_to_char("Hehe.  Most amusing, Sire."
                   "  (People have actually DONE this!)\n\r",
                   ch);
   }
   else if(CHAR_LEVEL(ch) <= CHAR_LEVEL(unit))
   {
      send_to_char("It IS a little cold isn't is?.\n\r", ch);
   }
   else if((fptr = find_fptr(unit, SFUN_FROZEN)) != nullptr)
   {
      act("$1n is now unfrozen.", A_SOMEONE, unit, nullptr, ch, TO_VICT);
      send_to_char("You unfreeze.\n\r", unit);
      destroy_fptr(unit, fptr);
   }
   else
   {
      act("$1n is now frozen.", A_SOMEONE, unit, nullptr, ch, TO_VICT);
      send_to_char("You totally freeze.\n\r", unit);
      create_fptr(unit, SFUN_FROZEN, 0, SFB_PRIORITY | SFB_CMD | SFB_AWARE, nullptr);
   }
}

void do_noshout(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *victim;

   if(!IS_PC(ch))
   {
      return;
   }

   /* Below here is the WIZARD removal of shout / noshout */
   victim = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD);
   if((victim == nullptr) || !IS_PC(victim))
   {
      send_to_char("No such player around.\n\r", ch);
      return;
   }

   if(CHAR_LEVEL(victim) >= CHAR_LEVEL(ch))
   {
      act("$3e might object to that... better not.", A_SOMEONE, ch, nullptr, victim, TO_CHAR);
      return;
   }

   if(IS_SET(PC_FLAGS(victim), PC_NOSHOUTING))
   {
      send_to_char("Your sore throath clears up.\n\r", victim);
      send_to_char("NOSHOUTING removed.\n\r", ch);
   }
   else
   {
      send_to_char("You suddenly get a sore throath!\n\r", victim);
      send_to_char("NOSHOUTING set.\n\r", ch);
   }
   TOGGLE_BIT(PC_FLAGS(victim), PC_NOSHOUTING);
}

void do_notell(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *victim;

   if(!IS_PC(ch))
   {
      return;
   }

   /* Below here is the WIZARD removal of shout / noshout */

   victim = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD);
   if((victim == nullptr) || !IS_PC(victim))
   {
      send_to_char("No such player around.\n\r", ch);
      return;
   }

   if(CHAR_LEVEL(victim) >= CHAR_LEVEL(ch))
   {
      act("$3e might object to that... better not.", A_SOMEONE, ch, nullptr, victim, TO_CHAR);
      return;
   }

   if(IS_SET(PC_FLAGS(victim), PC_NOTELLING))
   {
      send_to_char("You regain your telepatic ability.\n\r", victim);
      send_to_char("NOTELLING removed.\n\r", ch);
   }
   else
   {
      send_to_char("Your telepatic ability fades away!\n\r", victim);
      send_to_char("NOTELLING set.\n\r", ch);
   }
   TOGGLE_BIT(PC_FLAGS(victim), PC_NOTELLING);
}

void do_wizinv(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *unit;
   int        level = GOD_LEVEL - 1;

   arg = skip_spaces(arg);

   if((static_cast<unsigned int>(str_is_empty(arg)) != 0U) || (isdigit(*arg) != 0))
   {
      unit = ch;
   }
   else
   {
      unit = find_unit(ch, &arg, nullptr, FIND_UNIT_GLOBAL);

      if(unit == nullptr)
      {
         send_to_char("No such thing.\n\r", ch);
         return;
      }
   }

   arg = skip_spaces(arg);

   if(isdigit(*arg) != 0)
   {
      level = atoi(arg);
   }
   else if(UNIT_MINV(unit) != 0)
   {
      level = 0;
   }

   if(level > CHAR_LEVEL(ch))
   {
      send_to_char("Level cannot be beyond own level\r\n", ch);
      return;
   }

   if(level > 0)
   {
      act("$3n is now WIZI below level $2d.", A_ALWAYS, ch, &level, unit, TO_CHAR);

      if(unit != ch)
      {
         act("You are now WIZI below level $2d.", A_ALWAYS, unit, &level, nullptr, TO_CHAR);
      }
   }
   else
   {
      act("$3n is no longer WIZI.", A_ALWAYS, ch, nullptr, unit, TO_CHAR);

      if(unit != ch)
      {
         act("You are no longer WIZI.", A_ALWAYS, unit, nullptr, nullptr, TO_CHAR);
      }
   }

   UNIT_MINV(unit) = level;

   if(unit == ch)
   {
      slog(LOG_BRIEF, level, "%s wizinv%s", UNIT_NAME(ch), 0 < level ? "" : " off");
   }
}

void base_trans(unit_data *ch, unit_data *victim)
{
   assert(ch && victim);

   if(unit_recursive(victim, UNIT_IN(ch)) != 0)
   {
      send_to_char("Impossible: Recursive trans.\n\r", ch);
      return;
   }

   if((static_cast<unsigned int>(may_teleport(victim, UNIT_IN(ch))) == 0U) && CHAR_LEVEL(ch) < 240)
   {
      act("You are not allowed to transfer $3n.", A_SOMEONE, ch, nullptr, victim, TO_CHAR);
      return;
   }

   act("$1n disappears in a mushroom cloud.", A_HIDEINV, victim, nullptr, nullptr, TO_REST);
   unit_from_unit(victim);
   unit_to_unit(victim, UNIT_IN(ch));
   act("$1n arrives from a puff of smoke.", A_HIDEINV, victim, nullptr, nullptr, TO_REST);
   act("$1n has transferred you!", A_SOMEONE, ch, nullptr, victim, TO_VICT);
   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(victim, mbuf, &cmd_auto_unknown);
   for(victim = UNIT_CONTAINS(victim); victim != nullptr; victim = victim->next)
   {
      if(IS_CHAR(victim))
      {
         do_look(victim, mbuf, &cmd_auto_unknown);
      }
   }
}

void do_trans(unit_data *ch, char *arg, const struct command_info *cmd)
{
   descriptor_data *i;
   unit_data       *victim;

   if(!IS_PC(ch))
   {
      return;
   }

   victim = find_unit(ch, &arg, nullptr, FIND_UNIT_WORLD);

   if((victim == nullptr) || !IS_CHAR(victim))
   {
      if(str_nccmp("all", arg, 3) != 0)
      {
         send_to_char("No such person.\n\r", ch);
         return;
      }

      for(i = descriptor_list; i != nullptr; i = i->next)
      {
         if((i->character != ch) && (descriptor_is_playing(i) != 0))
         {
            base_trans(ch, i->character);
         }
      }

      send_to_char("Ok.\n\r", ch);
      return;
   }

   /* We found a victim that is a character: */
   if(victim == ch)
   {
      send_to_char("HA! You thought I hadn't thought of it, eh?\n\r", ch);
      return;
   }

   base_trans(ch, victim);
   send_to_char("Ok.\n\r", ch);
}

void do_at(unit_data *ch, char *argument, const struct command_info *cmd)
{
   char             buf[MAX_INPUT_LENGTH];
   unit_data       *target;
   unit_data       *original_loc;
   file_index_type *fi;

   if(!IS_PC(ch))
   {
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("You must supply a unit name or zone reference.\n\r", ch);
      return;
   }

   if(((fi = pc_str_to_file_index(ch, argument)) != nullptr) && ((fi->room_ptr) != nullptr))
   {
      target   = fi->room_ptr;
      argument = one_argument(argument, buf);
   }
   else
   {
      if((target = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD)) != nullptr)
      {
         if(UNIT_IN(target) != nullptr)
         {
            target = UNIT_IN(target);
         }
      }
   }

   if(target == nullptr)
   {
      send_to_char("No such place around.\n\r", ch);
      return;
   }

   if(!IS_ROOM(target))
   {
      send_to_char("Sorry, you may only 'at' at a room!\n\r", ch);
      return;
   }

   original_loc = UNIT_IN(ch);
   unit_from_unit(ch);
   unit_to_unit(ch, target);
   command_interpreter(ch, argument);

   /* check if the guy's still there */
   if((is_destructed(DR_UNIT, ch) == 0) && (is_destructed(DR_UNIT, original_loc) == 0) && (unit_recursive(ch, original_loc) == 0))
   {
      unit_from_unit(ch);
      unit_to_unit(ch, original_loc);
   }
}

void do_goto(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data        *target;
   unit_data        *pers;
   file_index_type  *fi;
   struct zone_type *zone;
   int               i;

   if(!IS_PC(ch))
   {
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      send_to_char("You must supply a unit name or zone reference.\n\r", ch);
      return;
   }

   if(((fi = pc_str_to_file_index(ch, argument)) != nullptr) && ((fi->room_ptr) != nullptr))
   {
      target = fi->room_ptr;
   }
   else if((target = find_unit(ch, &argument, nullptr, FIND_UNIT_SURRO | FIND_UNIT_ZONE | FIND_UNIT_WORLD)) != nullptr)
   {
      /* Find first container which can be entered */
      while((!IS_ROOM(target)) && ((UNIT_IN(target)) != nullptr) && (!IS_SET(UNIT_MANIPULATE(target), MANIPULATE_ENTER)))
      {
         target = UNIT_IN(target);
      }
   }
   else
   { /* Try to goto a zone */
      zone = find_zone(argument);
      if(zone != nullptr)
      {
         /* Find the first room in the zone */
         for(fi = zone->fi; ((fi) != nullptr) && (fi->type != UNIT_ST_ROOM); fi = fi->next)
         {
            ;
         }
         if(fi != nullptr)
         {
            target = fi->room_ptr;
         }
      }
   }

   if(target == nullptr)
   {
      send_to_char("No such place around.\n\r", ch);
      return;
   }

   if(IS_SET(UNIT_FLAGS(target), UNIT_FL_PRIVATE))
   {
      for(i = 0, pers = UNIT_CONTAINS(target); pers != nullptr; pers = pers->next)
      {
         if(IS_PC(pers))
         {
            i++;
         }
      }

      if(i > 1)
      {
         send_to_char("There's a private conversation going on in that room.\n\r", ch);
         return;
      }
   }

   if(static_cast<unsigned int>(may_teleport(ch, target)) == 0U)
   {
      send_to_char("WARNING - this is a no teleport environment.\n\r", ch);
   }

   if(unit_recursive(ch, target) != 0)
   {
      send_to_char("Recursive goto. Would destroy the universe.\n\r", ch);
      return;
   }

   act("$1n disappears into thin air.", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);
   unit_from_unit(ch);
   unit_to_unit(ch, target);
   act("$1n appears from thin air.", A_HIDEINV, ch, nullptr, nullptr, TO_ROOM);
   char mbuf[MAX_INPUT_LENGTH] = {0};
   do_look(ch, mbuf, cmd);
}

void do_crash(unit_data *ch, char *argument, const struct command_info *cmd)
{
   if(static_cast<unsigned int>(cmd_is_abbrev(ch, cmd)) != 0U)
   {
      send_to_char("If you want to crash the game - say so!\n\r", ch);
      return;
   }

   if(strcmp(argument, "the entire game...") != 0)
   {
      send_to_char("You must type 'crash the entire game...'\n\r", ch);
   }
   else
   {
      assert(FALSE); /* Bye bye */
   }
}

void do_execute(unit_data *ch, char *argument, const struct command_info *cmd)
{
   auto system_check(unit_data * pc, char *buf)->int;
   void execute_append(unit_data * pc, char *str);

   argument = skip_spaces(argument);

   if(system_check(ch, argument) == 0)
   {
      return;
   }

   execute_append(ch, argument);
   act("Executing $2t.\n\r", A_ALWAYS, ch, argument, nullptr, TO_CHAR);
}

void do_shutdown(unit_data *ch, char *argument, const struct command_info *cmd)
{
   char       buf[100];
   extern int mud_shutdown, mud_reboot;

   if(!IS_PC(ch))
   {
      return;
   }

   if(static_cast<unsigned int>(cmd_is_abbrev(ch, cmd)) != 0U)
   {
      send_to_char("If you want to shut something down - say so!\n\r", ch);
      return;
   }

   sprintf(buf, "Shutdown by %s.\n\r", UNIT_NAME(ch));
   send_to_all(buf);
   mud_shutdown = 1;
}

void do_reboot(unit_data *ch, char *argument, const struct command_info *cmd)
{
   char       buf[100];
   char       arg[MAX_INPUT_LENGTH];
   extern int mud_shutdown, mud_reboot;

   if(!IS_PC(ch))
   {
      return;
   }

   if(static_cast<unsigned int>(cmd_is_abbrev(ch, cmd)) != 0U)
   {
      send_to_char("If you want to reboot - say so!\n\r", ch);
      return;
   }

   descriptor_data *pDes;

   for(pDes = descriptor_list; pDes != nullptr; pDes = pDes->next)
   {
      if((CHAR_LEVEL(CHAR_ORIGINAL(pDes->character)) > CHAR_LEVEL(ch)))
      {
         send_to_char("Sorry, a God of higher level than yourself "
                      "is connected.\n\r",
                      ch);
         return;
      }
   }

   sprintf(buf, "Reboot by %s.\n\r", UNIT_NAME(ch));
   send_to_all(buf);
   mud_shutdown = mud_reboot = 1;
}

void do_snoop(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *victim;

   void unsnoop(unit_data * ch, int mode);
   void snoop(unit_data * ch, unit_data * victim);

   if(CHAR_DESCRIPTOR(ch) == nullptr)
   {
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      victim = ch;
   }
   else
   {
      victim = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD);
   }

   if(victim == nullptr)
   {
      send_to_char("No such player around.\n\r", ch);
      return;
   }

   if(!IS_CHAR(victim))
   {
      send_to_char("That's not a player / monster!\n\r", ch);
      return;
   }

   if(CHAR_DESCRIPTOR(victim) == nullptr)
   {
      act("$3n has no descriptor-link.", A_SOMEONE, ch, nullptr, victim, TO_CHAR);
      return;
   }

   if(victim == ch)
   {
      if(CHAR_IS_SNOOPING(ch))
      {
         unsnoop(ch, 0); /* Unsnoop just ch himself */
      }
      else
      {
         send_to_char("You are already snooping yourself.\n\r", ch);
      }
      return;
   }

   if(CHAR_IS_SNOOPED(victim))
   {
      send_to_char("Busy already.\n\r", ch);
      return;
   }

   if(CHAR_LEVEL(CHAR_ORIGINAL(victim)) >= CHAR_LEVEL(CHAR_ORIGINAL(ch)))
   {
      send_to_char("You're not allowed to snoop someone your own status.\n\r", ch);
      return;
   }

   send_to_char("Ok.\n\r", ch);

   if(CHAR_IS_SNOOPING(ch))
   {
      unsnoop(ch, 0); /* Unsnoop just ch himself */
   }

   snoop(ch, victim);
}

/* return -> switch <no_arg> */

void do_switch(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *victim;

   void switchbody(unit_data * ch, unit_data * victim);
   void unswitchbody(unit_data * npc);

   if(CHAR_DESCRIPTOR(ch) == nullptr)
   {
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      if(CHAR_IS_SWITCHED(ch))
      {
         unswitchbody(ch);
      }
      else
      {
         send_to_char("You are already home in your good old body.\n\r", ch);
      }
      return;
   }

   victim = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD | FIND_UNIT_SURRO);

   if((victim == nullptr) || !IS_NPC(victim))
   {
      send_to_char("No such monster around.\n\r", ch);
      return;
   }

   if(ch == victim)
   {
      send_to_char("He he he... We are jolly funny today, eh?\n\r", ch);
      return;
   }

   if(CHAR_DESCRIPTOR(victim) != nullptr)
   {
      act("$3n's body is already in use!", A_ALWAYS, ch, nullptr, victim, TO_CHAR);
   }
   else
   {
      send_to_char("Ok.\n\r", ch);
      switchbody(ch, victim);
   }
}

void base_force(unit_data *ch, unit_data *victim, char *arg)
{
   if(CHAR_LEVEL(ch) < CHAR_LEVEL(victim) && CHAR_CAN_SEE(ch, victim))
   {
      send_to_char("Oh no you don't!!\n\r", ch);
   }
   else
   {
      act("$1n has forced you to '$2t'", A_SOMEONE, ch, arg, victim, TO_VICT);
      command_interpreter(victim, arg);
   }
}

void do_force(unit_data *ch, char *argument, const struct command_info *cmd)
{
   descriptor_data *i;
   unit_data       *victim;

   if(!IS_PC(ch))
   {
      return;
   }

   victim = find_unit(ch, &argument, nullptr, FIND_UNIT_SURRO | FIND_UNIT_ZONE | FIND_UNIT_WORLD);

   if((victim == nullptr) || !IS_CHAR(victim))
   {
      if(str_nccmp("all", argument, 3) != 0)
      {
         send_to_char("No such character around.\n\r", ch);
         return;
      }
      argument = skip_spaces(argument + 3);

      if(*argument == 0)
      {
         send_to_char("Who do you wish to force to do what?\n\r", ch);
         return;
      }

      for(i = descriptor_list; i != nullptr; i = i->next)
      {
         if(i->character != ch && (descriptor_is_playing(i) != 0))
         {
            base_force(ch, i->character, argument);
         }
      }

      send_to_char("Ok.\n\r", ch);
      return;
   }

   argument = skip_spaces(argument);

   /* We now have a victim : */
   if(*argument == 0)
   {
      send_to_char("Who do you wish to force to do what?\n\r", ch);
   }
   else
   {
      base_force(ch, victim, argument);
      send_to_char("Ok.\n\r", ch);
   }
}

void do_finger(unit_data *ch, char *arg, const struct command_info *cmd)
{
   char buf[MAX_INPUT_LENGTH];

   void reset_char(unit_data * ch);
   auto player_exists(char *pName)->int;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Finger who?\n\r", ch);
      return;
   }

   arg = one_argument(arg, buf);

   if(find_descriptor(buf, nullptr) != nullptr)
   {
      send_to_char("A player by that name is connected - do a wstat.\n\r", ch);
      return;
   }

   send_to_char("This command will soon give finger info about other "
                "players.\n\r",
                ch);
}

void do_load(unit_data *ch, char *arg, const struct command_info *cmd)
{
   char             buf[MAX_INPUT_LENGTH];
   file_index_type *fi;
   unit_data       *u;
   unit_data       *tmp;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Load? Load what??\n\r", ch);
      return;
   }

   arg = one_argument(arg, buf);

   if(find_descriptor(buf, nullptr) != nullptr)
   {
      send_to_char("A player by that name is connected.\n\r", ch);
      return;
   }

   if((fi = pc_str_to_file_index(ch, buf)) == nullptr)
   {
      for(tmp = unit_list; tmp != nullptr; tmp = tmp->gnext)
      {
         if(IS_PC(tmp) && (str_ccmp(UNIT_NAME(tmp), buf) == 0))
         {
            send_to_char("A player by that name is linkdead in the game.\n\r", ch);
            return;
         }
      }

      if(player_exists(buf) != 0)
      {
         if((u = load_player(buf)) == nullptr)
         {
            send_to_char("Load error\n\r", ch);
            return;
         }

         enter_game(u);

         unit_from_unit(u);
         unit_to_unit(u, UNIT_IN(ch));
         send_to_char("You have loaded the player.\n\r", ch);

         if(UNIT_CONTAINS(u) != nullptr)
         {
            send_to_char("Inventory loaded.\n\r", ch);
         }

         if(CHAR_LEVEL(u) > CHAR_LEVEL(ch))
         {
            slog(LOG_EXTENSIVE, UNIT_MINV(ch), "LEVEL: %s loaded %s when lower level.", UNIT_NAME(ch), UNIT_NAME(u));
         }
         return;
      }

      send_to_char("No such file index reference found.\n\r", ch);
      return;
   }

   if((fi->room_ptr != nullptr) || fi->type == UNIT_ST_ROOM)
   {
      send_to_char("Sorry, you are not allowed to load rooms.\n\r", ch);
      return;
   }

   if(CHAR_LEVEL(ch) < fi->zone->loadlevel)
   {
      if(fi->zone->creators.IsName(UNIT_NAME(ch)) == nullptr)
      {
         int i = fi->zone->loadlevel;

         act("Level $2d is required to load items from this zone.", A_ALWAYS, ch, &i, nullptr, TO_CHAR);
         return;
      }
   }

   u = read_unit(fi);

   if(IS_MONEY(u))
   {
      if(!IS_ADMINISTRATOR(ch))
      {
         send_to_char("No you don't.\n\r", ch);
         extract_unit(u);
         return;
      }

      set_money(u, MONEY_AMOUNT(u));
   }

   if(IS_OBJ(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_TAKE))
   {
      unit_to_unit(u, ch);
      act("You secretly load $2n.", A_SOMEONE, ch, u, nullptr, TO_CHAR);
   }
   else
   {
      unit_to_unit(u, UNIT_IN(ch));
      act("$1n opens an interdimensional gate and fetches $3n.", A_SOMEONE, ch, nullptr, u, TO_ROOM);
      act("$1n says, 'Hello World!'", A_SOMEONE, u, nullptr, nullptr, TO_ROOM);
   }
}

void do_delete(unit_data *ch, char *arg, const struct command_info *cmd)
{
   char       buf[MAX_INPUT_LENGTH];
   unit_data *tmp;

   if(static_cast<unsigned int>(cmd_is_abbrev(ch, cmd)) != 0U)
   {
      send_to_char("If you want to delete someone - say so!\n\r", ch);
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Delete what player???\n\r", ch);
      return;
   }

   arg = one_argument(arg, buf);

   if(find_descriptor(buf, nullptr) != nullptr)
   {
      send_to_char("A player by that name is connected. Purge first.\n\r", ch);
      return;
   }

   for(tmp = unit_list; tmp != nullptr; tmp = tmp->gnext)
   {
      if(IS_PC(tmp) && (str_ccmp(UNIT_NAME(tmp), buf) == 0))
      {
         send_to_char("A player by that name is linkdead in the game.\n\r", ch);
         return;
      }
   }

   if(delete_player(buf) != 0)
   {
      send_to_char("You have deleted the player!\n\r", ch);
   }
   else
   {
      send_to_char("No such player found in index file.\n\r", ch);
   }
}

/* clean a room of all mobiles and objects */
void do_purge(unit_data *ch, char *argument, const struct command_info *cmd)
{
   char             buf[MAX_INPUT_LENGTH];
   unit_data       *thing;
   unit_data       *next_thing;
   descriptor_data *d;

   void close_socket(descriptor_data * d);

   if(!IS_PC(ch))
   {
      return;
   }

   one_argument(argument, buf);

   if(static_cast<unsigned int>(str_is_empty(argument)) != 0U)
   {
      act("$1n gestures... You are surrounded by scorching flames!", A_SOMEONE, ch, nullptr, nullptr, TO_ROOM);
      act("You are surrounded by scorching flames!", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);

      for(thing = UNIT_CONTAINS(UNIT_IN(ch)); thing != nullptr; thing = next_thing)
      {
         next_thing = thing->next;
         if(!IS_PC(thing) && !IS_ROOM(thing) && (find_fptr(thing, SFUN_PERSIST_INTERNAL) == nullptr))
         {
            if(!IS_OBJ(thing) || IS_SET(UNIT_MANIPULATE(thing), MANIPULATE_TAKE))
            {
               extract_unit(thing);
            }
         }
      }
      return;
   }

   if(!IS_ADMINISTRATOR(ch))
   {
      send_to_char("You can only purge to clean rooms...", ch);
      return;
   }

   if((str_ccmp_next_word(buf, "_doomsday_") != nullptr) && IS_ULTIMATE(ch))
   {
      send_to_char("You do a MAJOR cleanup!\n\r", ch);

      for(thing = unit_list; thing != nullptr; thing = next_thing)
      {
         next_thing = thing->gnext;
         if(!IS_PC(thing) && !IS_ROOM(thing))
         {
            if(IS_ROOM(UNIT_IN(thing)))
            {
               extract_unit(thing);
            }
         }
      }

      return;
   }

   if((d = find_descriptor(buf, nullptr)) != nullptr)
   {
      if((d->character != nullptr) && (CHAR_LEVEL(ch) < CHAR_LEVEL(d->character)))
      {
         send_to_char("Fuuuuuuuuu!\n\r", ch);
         return;
      }

      send_to_char("Purged completely from the game.\n\r", ch);

      if(d->fptr == descriptor_interpreter)
      {
         extract_unit(d->character);
      }
      descriptor_close(d);
   }
   else if((thing = find_unit(ch, &argument, nullptr, FIND_UNIT_INVEN | FIND_UNIT_SURRO)) != nullptr)
   {
      if(IS_PC(thing) && (CHAR_LEVEL(ch) <= CHAR_LEVEL(thing)))
      {
         send_to_char("Fuuuuuuuuu!\n\r", ch);
      }
      else if(!IS_ROOM(thing))
      {
         act("$1n disintegrates $3n.", A_SOMEONE, ch, nullptr, thing, TO_NOTVICT);
         act("You disintegrate $3n.", A_SOMEONE, ch, nullptr, thing, TO_CHAR);
         extract_unit(thing);
      }
      else
      {
         act("You may not purge rooms.", A_SOMEONE, ch, nullptr, nullptr, TO_CHAR);
      }
   }
   else
   {
      send_to_char("No such thing found...\n\r", ch);
   }
}

void do_advance(unit_data *ch, char *argument, const struct command_info *cmd)
{
   unit_data *victim;
   char       name[100];
   char       level[100];
   int        newlevel;

   if(!IS_PC(ch))
   {
      return;
   }

   argument_interpreter(argument, name, level);

   if(*name != 0)
   {
      if(((victim = find_unit(ch, &argument, nullptr, FIND_UNIT_SURRO | FIND_UNIT_WORLD)) == nullptr) || !IS_PC(victim))
      {
         send_to_char("That player is not here.\n\r", ch);
         return;
      }
   }
   else
   {
      send_to_char("Advance who?\n\r", ch);
      return;
   }

   /*   if (CHAR_LEVEL(victim) < START_LEVEL)
        {
        send_to_char("Player must be minimum start level.\n\r", ch);
        return;
        }*/

   if(*level == 0)
   {
      send_to_char("You must supply a level number.\n\r", ch);
      return;
   }

   if(isdigit(*level) == 0)
   {
      send_to_char("Second argument must be a positive integer.\n\r", ch);
      return;
   }

   newlevel = atoi(level);

   if(newlevel > ULTIMATE_LEVEL || newlevel < START_LEVEL)
   {
      send_to_char("New level must be between 4 and 255.\n\r", ch);
      return;
   }

   if(newlevel < CHAR_LEVEL(victim) && newlevel >= IMMORTAL_LEVEL && IS_ADMINISTRATOR(ch) && CHAR_LEVEL(victim) < CHAR_LEVEL(ch))
   {
      CHAR_EXP(victim)   = 0;
      CHAR_LEVEL(victim) = newlevel;
      send_to_char("You feel pretty awesome!\n\r", ch);
      act("$3n makes some strange gestures. "
          "A horrible feeling comes upon you, like a giant hand, darkness"
          "comes down from above, grabbing your body, which begin to ache "
          "with striking pain from inside. Your head seems to be filled with "
          "deamons from another plane as your body dissolves into the "
          "elements of time and space itself. You feel less powerful.",
          A_ALWAYS, victim, nullptr, ch, TO_CHAR);
      return;
   }

   if(newlevel <= CHAR_LEVEL(victim))
   {
      send_to_char("Can't diminish that players status.\n\r", ch);
      return;
   }

   if(CHAR_LEVEL(ch) < ULTIMATE_LEVEL && newlevel >= CHAR_LEVEL(ch))
   {
      send_to_char("Thou art not godly enough.\n\r", ch);
      return;
   }

   send_to_char("You feel generous.\n\r", ch);
   act("$3n makes some strange gestures. "
       "A strange feeling comes upon you, like a giant hand, light comes "
       "down from above, grabbing your body, which begins to pulse with "
       "coloured lights from inside. Your head seems to be filled with "
       "deamons from another plane as your body dissolves into the elements "
       "of time and space itself. Suddenly a silent explosion of light snaps "
       "you back to reality. You feel slightly different.",
       A_ALWAYS, victim, nullptr, ch, TO_CHAR);

   if(newlevel >= IMMORTAL_LEVEL)
   {
      CHAR_LEVEL(victim) = newlevel;
      CHAR_EXP(ch)       = 0;
   }
   else
   {
      gain_exp_regardless(victim, required_xp(newlevel) - CHAR_EXP(victim));
   }
}

void do_verify(unit_data *ch, char *arg, const struct command_info *cmd)
{
#ifdef SUSPEKT
   unit_data *pc, *obj;
   int        i, j;
   float      asum, ssum;
   float      atot, stot;
   char       buf[256];

   extern struct requirement_type pc_race_base[];

   if(!IS_PC(ch))
      return;

   if(str_is_empty(arg))
      send_to_char("Verify who?\n\r", ch);

   if(!(pc = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)) || !IS_PC(pc))
   {
      send_to_char("No such player in the game.\n\r", ch);
      return;
   }

   if(CHAR_LEVEL(ch) <= CHAR_LEVEL(pc))
   {
      send_to_char("No. You don't want to do that.\n\r", ch);
      return;
   }

   if(IS_IMMORTAL(pc))
   {
      send_to_char("Only on mortal PC's.\n\r", ch);
      return;
   }

   for(obj = UNIT_CONTAINS(pc); obj; obj = obj->next)
      if(IS_OBJ(obj) && OBJ_EQP_POS(obj))
         unequip_object(obj);

   affect_clear_unit(pc);

   if(UNIT_AFFECTED(pc))
   {
      send_to_char("ERROR: Could not entirely destroy affects!\n\r", ch);
      return;
   }

   asum = PC_ABILITY_POINTS(pc);
   ssum = PC_SKILL_POINTS(pc);

   for(i = 0; i < ABIL_TREE_MAX; i++)
   {
      for(j = 0; j < CHAR_ABILITY(pc, i); j++)
         asum += next_point_cost((int)pc_race_base[CHAR_RACE(pc)].abilities[i], j);
   }

   PC_SKI_SKILL(pc, SKI_FLEE) -= 50;
   PC_SKI_SKILL(pc, SKI_CONSIDER) -= 50;
   PC_SKI_SKILL(pc, SKI_DIAGNOSTICS) -= 50;

   for(i = 0; i < SKI_TREE_MAX; i++)
   {
      for(j = 0; j < PC_SKI_SKILL(pc, i); j++)
         ssum += next_point_cost(100, j);
   }

   PC_SKI_SKILL(pc, SKI_FLEE) += 50;
   PC_SKI_SKILL(pc, SKI_CONSIDER) += 50;
   PC_SKI_SKILL(pc, SKI_DIAGNOSTICS) += 50;

   for(i = 0; i < SPL_TREE_MAX; i++)
   {
      for(j = 0; j < PC_SPL_SKILL(pc, i); j++)
         ssum += next_point_cost(100, j);
   }

   for(i = 0; i < WPN_TREE_MAX; i++)
   {
      for(j = 0; j < PC_WPN_SKILL(pc, i); j++)
         ssum += next_point_cost(100, j);
   }

   atot = ability_point_total(START_LEVEL);
   stot = skill_point_total(START_LEVEL);

   for(i = START_LEVEL + 1; i <= CHAR_LEVEL(pc); i++)
   {
      atot += (int)ability_point_gain(i);
      stot += (int)skill_point_gain(i);
   }

   sprintf(buf, "Has %.0f (excess) ability points (%.0f / %.0f).\n\r", asum - atot, asum, atot);
   send_to_char(buf, ch);
   sprintf(buf, "Has %.0f (excess) skill points (%.0f / %.0f).\n\r", ssum - stot, ssum, stot);
   send_to_char(buf, ch);
#endif
}

void reroll(unit_data *victim)
{
   struct extra_descr_data *exd;
   struct extra_descr_data *nextexd;
   unit_data               *obj;
   int                      i;

   void race_cost(unit_data * ch);
   void points_reset(unit_data * ch);

   void clear_training_level(unit_data * ch);

   if(IS_IMMORTAL(victim))
   {
      return;
   }

   for(obj = UNIT_CONTAINS(victim); obj != nullptr; obj = obj->next)
   {
      if(IS_OBJ(obj) && (OBJ_EQP_POS(obj) != 0u))
      {
         unequip_object(obj);
      }
   }

   affect_clear_unit(victim);

   if(UNIT_AFFECTED(victim) != nullptr)
   {
      send_to_char("You were not rerolled!\n\r", victim);
      return;
   }

   race_cost(victim);
   race_cost(victim);
   points_reset(victim);

   UNIT_MAX_HIT(victim) = UNIT_HIT(victim) = hit_limit(victim);

   send_to_char("You are re-initialized. Go practice.\n\r", victim);

   clear_training_level(victim);

   CHAR_LEVEL(victim) = PC_VIRTUAL_LEVEL(victim) = 0;

   PC_SKILL_POINTS(victim)   = skill_point_total(0);
   PC_ABILITY_POINTS(victim) = ability_point_total(0);

   if(PC_GUILD(victim))
   {
      free(PC_GUILD(victim));
      PC_GUILD(victim) = nullptr;
   }

   for(exd = PC_QUEST(victim); exd != nullptr; exd = nextexd)
   {
      nextexd = exd->next;

      if((exd->names.Name() != nullptr) && exd->names.Name()[0] == '$')
      {
         PC_QUEST(victim) = PC_QUEST(victim)->remove(exd->names.Name());
      }
   }
}

void do_reroll(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *victim;

   if(!IS_PC(ch))
   {
      return;
   }

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Reroll who?\n\r", ch);
   }

   if((victim = find_unit(ch, &arg, nullptr, FIND_UNIT_WORLD)) == nullptr)
   {
      send_to_char("No such player in the game.\n\r", ch);
      return;
   }

   if(!IS_PC(victim))
   {
      send_to_char("Only on PC's.\n\r", ch);
      return;
   }

   if(CHAR_LEVEL(ch) <= CHAR_LEVEL(victim))
   {
      send_to_char("No. You don't want to do that.\n\r", ch);
      return;
   }

   reroll(victim);

   send_to_char("Rerolled.\n\r", ch);
}

void do_restore(unit_data *ch, char *argument, const struct command_info *cmd)
{
   int        i;
   unit_data *victim;

   if(!IS_PC(ch))
   {
      return;
   }

   victim = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD);

   if((victim == nullptr) || !IS_CHAR(victim))
   {
      send_to_char("No such character to restore.\n\r", ch);
      return;
   }

   CHAR_MANA(victim) = mana_limit(victim);
   UNIT_HIT(victim) = UNIT_MAX_HIT(victim) = hit_limit(victim);
   CHAR_ENDURANCE(victim)                  = move_limit(victim);

   if(IS_PC(victim))
   {
      if(IS_GOD(victim))
      {
         for(i = 0; i < ABIL_TREE_MAX; i++)
         {
            CHAR_ABILITY(ch, i) = 150;
         }
         for(i = 0; i < SPL_TREE_MAX; i++)
         {
            PC_SPL_SKILL(ch, i) = 150;
         }
         for(i = 0; i < WPN_TREE_MAX; i++)
         {
            PC_WPN_SKILL(ch, i) = 150;
         }
         for(i = 0; i < SKI_TREE_MAX; i++)
         {
            PC_SKI_SKILL(ch, i) = 150;
         }
      }
      else
      {
         PC_COND(victim, FULL)   = 24;
         PC_COND(victim, THIRST) = 24;
         PC_COND(victim, DRUNK)  = 0;
      }
   }

   update_pos(victim);
   send_to_char("Done.\n\r", ch);
   act("You have been fully healed by $3n!", A_SOMEONE, victim, nullptr, ch, TO_CHAR);
}

/****************************
 * The command `file' below *
 ****************************/

extern char *wizlist, *news, *credits, *motd, *goodbye;
auto         read_info_file(char *name, char *oldstr) -> char *;

static auto file_install(char *file, bool bNew) -> bool
{
   char buf[256];

   sprintf(buf, "%s%s", libdir, file);

   if(bNew)
   {
      if(static_cast<unsigned int>(file_exists(str_cc(buf, ".new"))) != 0U)
      {
         rename(buf, str_cc(buf, ".old"));
         rename(str_cc(buf, ".new"), buf);
      }
      else
      {
         return FALSE;
      }
   }
   else
   {
      if(static_cast<unsigned int>(file_exists(str_cc(buf, ".old"))) != 0U)
      {
         rename(str_cc(buf, ".old"), buf);
      }
      else
      {
         return FALSE;
      }
   }

   return TRUE;
}

void do_file(unit_data *ch, char *argument, const struct command_info *cmd)
{
   char        buf[MAX_INPUT_LENGTH];
   const char *str = "$2t installed.";

   argument = one_argument(argument, buf);

   if(strcmp(buf, "new") == 0)
   {
      one_argument(argument, buf);

      if(file_install(buf, TRUE))
      {
         str = "$2t installed.  Backup in $2t.old";
      }
      else
      {
         str = "No new file for $2t detected.";
      }
   }
   else if(strcmp(buf, "old") == 0)
   {
      one_argument(argument, buf);

      if(file_install(buf, FALSE))
      {
         str = "$2t.old re-installed.";
      }
      else
      {
         str = "No backup for $2t present.";
      }
   }

   if(strcmp(buf, "wizlist") == 0)
   {
      g_cServerConfig.m_pWizlist = read_info_file(str_cc(libdir, WIZLIST_FILE), g_cServerConfig.m_pWizlist);
   }
   else if(strcmp(buf, "news") == 0)
   {
      g_cServerConfig.m_pNews = read_info_file(str_cc(libdir, NEWS_FILE), g_cServerConfig.m_pNews);
   }
   else if(strcmp(buf, "motd") == 0)
   {
      g_cServerConfig.m_pMotd = read_info_file(str_cc(libdir, MOTD_FILE), g_cServerConfig.m_pMotd);
   }
   else if(strcmp(buf, "credits") == 0)
   {
      g_cServerConfig.m_pCredits = read_info_file(str_cc(libdir, CREDITS_FILE), g_cServerConfig.m_pCredits);
   }
   else if(strcmp(buf, "goodbye") == 0)
   {
      g_cServerConfig.m_pGoodbye = read_info_file(str_cc(libdir, GOODBYE_FILE), g_cServerConfig.m_pGoodbye);
   }
   else
   {
      str = "Usage:\n\r"
            "file [new|old] <wizlist | news | credits | motd | goodbye>";
   }

   act(str, A_ALWAYS, ch, buf, nullptr, TO_CHAR);
}

/* end file */

void do_message(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *vict;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Who? What??\n\r", ch);
   }
   else if((vict = find_unit(ch, &arg, nullptr, FIND_UNIT_WORLD)) == nullptr)
   {
      send_to_char("No-one by that name here.\n\r", ch);
   }
   else if(vict == ch)
   {
      send_to_char("You recieve a message from yourself.\n\r", ch);
   }
   else if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("What??\n\r", ch);
   }
   else
   {
      act("$2t", A_ALWAYS, vict, skip_spaces(arg), nullptr, TO_CHAR);
      send_to_char("Ok.\n\r", ch);
   }
}

void do_broadcast(unit_data *ch, char *arg, const struct command_info *cmd)
{
   descriptor_data *d;

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Yeah, that makes a LOT of sense!\n\r", ch);
   }
   else
   {
      for(d = descriptor_list; d != nullptr; d = d->next)
      {
         if(descriptor_is_playing(d) != 0)
         {
            act("$2t", A_ALWAYS, d->character, arg, nullptr, TO_CHAR);
         }
      }
   }
}

void list_wizards(unit_data *ch, bool value)
{
   descriptor_data *d;
   bool             any = FALSE;
   char             buf[MAX_STRING_LENGTH];
   char            *s = buf;

   sprintf(s, "Wizards %sline:\n\r", value ? "on" : "off");
   TAIL(s);

   for(d = descriptor_list; d != nullptr; d = d->next)
   {
      if((descriptor_is_playing(d) != 0) && WIZ_CMD_LEVEL <= CHAR_LEVEL(CHAR_ORIGINAL(d->character)) &&
         CHAR_CAN_SEE(ch, CHAR_ORIGINAL(d->character)))
      {
         bool nowiz = IS_SET(PC_FLAGS(CHAR_ORIGINAL(d->character)), PC_NOWIZ);

         if((value && !nowiz) || (!value && nowiz))
         {
            any = TRUE;
            sprintf(s, "%s%s(%d), ", UNIT_MINV(d->character) > 0 ? "*" : "", UNIT_NAME(CHAR_ORIGINAL(d->character)),
                    CHAR_LEVEL(CHAR_ORIGINAL(d->character)));
            TAIL(s);
         }
      }
   }

   if(any)
   {
      *(s - 2) = '\0';
      act("$2t.", A_ALWAYS, ch, buf, nullptr, TO_CHAR);
   }
}

void do_wiz(unit_data *ch, char *arg, const struct command_info *cmd)
{
   descriptor_data *d;
   char             tmp[MAX_INPUT_LENGTH];
   bool             emote = FALSE;
   int              level;

   if(CHAR_DESCRIPTOR(ch) == nullptr)
   {
      return;
   }

   if(cmd->no == CMD_WIZ)
   {
      WIZ_CMD_LEVEL = cmd->minimum_level;
   }

   if(IS_SET(UNIT_FLAGS(ch), UNIT_FL_BURIED))
   {
      send_to_char("You can not talk while buried.\n\r", ch);
      return;
   }

   level = MAX(WIZ_CMD_LEVEL, UNIT_MINV(CHAR_ORIGINAL(ch)));

   switch(*arg)
   {
      case '@':
         emote = TRUE;
         /* fall-thru! */

      case '#':
         one_argument(arg + 1, tmp);
         if(static_cast<unsigned int>(str_is_number(tmp)) != 0U)
         {
            arg   = one_argument(++arg, tmp);
            level = MAX(atoi(tmp), WIZ_CMD_LEVEL);
         }
         else if(emote)
         {
            arg++;
         }
         break;

      case '?':
         if(static_cast<unsigned int>(str_is_empty(arg + 1)) != 0U)
         {
            list_wizards(ch, TRUE);
            list_wizards(ch, FALSE);
            return;
         }
         break;

      case '-':
         if(static_cast<unsigned int>(str_is_empty(arg + 1)) != 0U)
         {
            if(IS_SET(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ))
            {
               send_to_char("But you're ALREADY offline!\n\r", ch);
            }
            else
            {
               send_to_char("You won't hear the wizline from now on.\n\r", ch);
               SET_BIT(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ);
            }
            return;
         }
         break;

      case '+':
         if(static_cast<unsigned int>(str_is_empty(arg + 1)) != 0U)
         {
            if(!IS_SET(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ))
            {
               send_to_char("But you're ALREADY online!\n\r", ch);
            }
            else
            {
               send_to_char("You can now hear the wizline again.\n\r", ch);
               REMOVE_BIT(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ);
            }
            return;
         }
         break;
   }

   if(IS_SET(PC_FLAGS(CHAR_ORIGINAL(ch)), PC_NOWIZ))
   {
      send_to_char("You're offline!!\n\r", ch);
      return;
   }

   arg = skip_spaces(arg);

   if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
   {
      send_to_char("Gods don't like being bothered like that!\n\r", ch);
      return;
   }

   sprintf(tmp, COLOUR_COMM ":%s: $1n %s$2t" COLOUR_NORMAL, WIZ_CMD_LEVEL < level ? itoa(level) : "", emote ? "" : ":: ");

   for(d = descriptor_list; d != nullptr; d = d->next)
   {
      if((descriptor_is_playing(d) != 0) && level <= CHAR_LEVEL(CHAR_ORIGINAL(d->character)) &&
         !IS_SET(PC_FLAGS(CHAR_ORIGINAL(d->character)), PC_NOWIZ))
      {
         act(tmp, A_SOMEONE, CHAR_ORIGINAL(ch), arg, d->character, TO_VICT);
      }
   }
}

void do_title(unit_data *ch, char *arg, const struct command_info *cmd)
{
   unit_data *u;
   char      *oldarg = arg;

   if(!IS_PC(ch) || (static_cast<unsigned int>(str_is_empty(arg)) != 0U))
   {
      send_to_char("That's rather silly, I think.\n\r", ch);
      return;
   }

   u = find_unit(ch, &arg, nullptr, FIND_UNIT_SURRO | FIND_UNIT_WORLD);

   if(u == nullptr || !IS_PC(u))
   {
      arg = oldarg;
      u   = ch;
   }
   else
   {
      arg = skip_spaces(arg);

      if(static_cast<unsigned int>(str_is_empty(arg)) != 0U)
      {
         send_to_char("You can't assign an empty title\n\r", ch);
         return;
      }
   }

   if(strlen(arg) > 50)
   {
      send_to_char("Title too long - Truncated.\n\r", ch);
      arg[50] = '\0';
   }
   else
   {
      send_to_char("Ok.\n\r", ch);
   }

   UNIT_TITLE(u).Reassign(arg);
}

/*  0: free access
 * >0: locked for below this level
 */
void do_wizlock(unit_data *ch, char *arg, const struct command_info *cmd)
{
   int  lvl;
   char buf[128];

   arg = one_argument(arg, buf);

   if(*buf != 0)
   {
      lvl = atoi(buf) + 1;
   }
   else
   {
      lvl = GOD_LEVEL;
   }

   if(lvl >= CHAR_LEVEL(ch))
   {
      lvl = CHAR_LEVEL(ch);
   }
   if(lvl == 0)
   {
      lvl = 1;
   }

   if((wizlock != 0) && (*buf == 0))
   {
      send_to_char("Game is no longer wizlocked.\n\r", ch);
      wizlock = 0;
   }
   else
   {
      sprintf(buf, "Game is now wizlocked for level %d%s.\n\r", lvl - 1, lvl - 1 > 0 ? " and down" : "");
      send_to_char(buf, ch);
      wizlock = lvl;
   }
}

void do_wizhelp(unit_data *ch, char *arg, const struct command_info *cmd)
{
   char                       buf[MAX_STRING_LENGTH];
   char                      *b;
   int                        no;
   int                        i;
   extern struct command_info cmd_info[];

   if(!IS_PC(ch))
   {
      return;
   }

   send_to_char("The following privileged commands are available:\n\r\n\r", ch);

   *buf = '\0';
   b    = buf;

   for(no = 1, i = 0; *cmd_info[i].cmd_str != 0; i++)
   {
      if(CHAR_LEVEL(ch) >= cmd_info[i].minimum_level && cmd_info[i].minimum_level >= IMMORTAL_LEVEL)
      {
         sprintf(b, "%3d %-10s", cmd_info[i].minimum_level, cmd_info[i].cmd_str);
         if((no % 5) == 0)
         {
            strcat(buf, "\n\r");
         }
         no++;
         TAIL(b);
      }
   }

   strcpy(b, "\n\r");

   page_string(CHAR_DESCRIPTOR(ch), buf);
}

void do_kickit(unit_data *ch, char *arg, const struct command_info *cmd)
{
   send_to_char("No compiler in this version, I guess.\n\r", ch);
}

void do_corpses(unit_data *ch, char *arg, const struct command_info *cmd)
{
   extern auto in_string(unit_data * ch, unit_data * u)->char *;

   unit_data *c;
   bool       found = FALSE;
   char      *c1;
   char      *c2;
   char       buf[512];

   send_to_char("The following player corpses were found:\n\r", ch);

   for(c = unit_list; c != nullptr; c = c->gnext)
   {
      if(IS_OBJ(c) && OBJ_VALUE(c, 2) == 1 /* 1 means player corpse, if a corpse */
         && (UNIT_CONTAINS(c) != nullptr)) /* skip empty corpses */
      {
         c1 = str_str(UNIT_OUT_DESCR_STRING(c), " corpse of ");
         c2 = str_str(UNIT_OUT_DESCR_STRING(c), " is here.");

         if(c1 == nullptr || c2 == nullptr)
         {
            continue;
         }

         found = TRUE;

         strncpy(buf, c1 + 1, c2 - (c1 + 1));
         buf[c2 - (c1 + 1)] = '\0';

         act("  $2t: $3t", A_ALWAYS, ch, buf, in_string(ch, c), TO_CHAR);
      }
   }

   if(!found)
   {
      send_to_char("  None!\n\r", ch);
   }
}
