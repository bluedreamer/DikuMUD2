/* *********************************************************************** *
 * File   : zone_reset.c                              Part of Valhalla MUD *
 * Version: 1.24                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Manages resetting of zones.                                    *
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

/* 28/1 1993 HHS   Added code to implement Complete flag in reset_zone     */
/* 28/03/93 jubal  : Better error messages                                 */

#include "affect.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "db_file.h"
#include "handler.h"
#include "interpreter.h"
#include "limits.h"
#include "main.h"
#include "money.h"
#include "skills.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void event_enq(int when, void (*func)(), void *arg1, void *arg2);

std::shared_ptr<zone_type> boot_zone; /* Points to the zone currently booted */

/* No Operation */
unit_data *zone_nop(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   /* Return TRUE - NOP always succeedes */

   return reinterpret_cast<unit_data *>(boot_zone.get()); /* dummy */
}

/* Random */
unit_data *zone_random(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   /* Return TRUE if random 0-99 less than given percent  */
   if(number(0, 99) < cmd->num[0])
      return reinterpret_cast<unit_data *>(boot_zone.get()); /* dummy */
   else
      return {};
}

/* Count ->no_in_zone for current 'boot_zone' (above) */
void zone_update_no_in_zone(void)
{
   std::shared_ptr<unit_data> u;
   std::shared_ptr<file_index_type> fi;
   std::shared_ptr<zone_type>       tmp_zone;

   /* Clear ALL ->no_in_zone */
   for(tmp_zone = zone_info.zone_list; tmp_zone; tmp_zone = tmp_zone->next)
      for(fi = tmp_zone->fi; fi; fi = fi->next)
         fi->no_in_zone = 0;

   for(u = unit_list; u; u = u->gnext)
      if(UNIT_FILE_INDEX(u) && unit_zone(u) == boot_zone)
         UNIT_FILE_INDEX(u)->no_in_zone++;
}

/* After loading a unit, call this function to update no_in_zone */
void zone_loaded_a_unit(std::shared_ptr<unit_data> u)
{
   if(unit_zone(u) == boot_zone)
      UNIT_FILE_INDEX(u)->no_in_zone++;
}

/* num[0] is the max allowed existing in world              */
/* num[1] is the max allowed existing in zone.              */
/* num[2] is the max allowed existing in room (object)      */
/* Return TRUE if conditions are met, FALSE otherwise       */
bool zone_limit(std::shared_ptr<unit_data> u, std::shared_ptr<file_index_type> fi, std::shared_ptr<zone_reset_cmd> cmd)
{
   std::shared_ptr<unit_data> tmp;
   sbit16            i;

   if(fi->type == UNIT_ST_NPC)
   {
      /* If no maxima on mobiles, set it to default of one global. */
      if(cmd->num[0] == 0 && cmd->num[1] == 0 && cmd->num[2] == 0)
         cmd->num[0] = 1;
   }
   else
   {
      /* If no maxima on objects, set it to default of one local! */
      if(cmd->num[0] == 0 && cmd->num[1] == 0 && cmd->num[2] == 0)
         cmd->num[2] = 1;
   }

   /* Check for global maxima */
   if(cmd->num[0] && fi->no_in_mem >= (ubit16)(cmd->num[0]))
      return FALSE;

   /* Check for zone maximum */
   if(cmd->num[1] && fi->no_in_zone >= cmd->num[1] && unit_zone(u) == boot_zone)
      return FALSE;

   /* Check for local maxima */
   if((i = cmd->num[2]))
   {
      for(tmp = UNIT_CONTAINS(u); tmp; tmp = tmp->next)
         if(UNIT_FILE_INDEX(tmp) == fi)
            --i;

      if(i <= 0)
         return FALSE;
   }

   return TRUE;
}

/* fi[0] is unit to be loaded                                     */
/* fi[1] is room to place loaded unit in or 0 if a PUT command    */
/* num[0] is the max allowed existing number (0 ignores) in world */
/* num[1] is the max allowed locally existing number              */
std::shared_ptr<unit_data> zone_load(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   std::shared_ptr<unit_data> loaded = NULL;

   /* Destination */
   if(cmd->fi[1] && cmd->fi[1]->room_ptr)
      u = cmd->fi[1]->room_ptr;

   /* Does the destination room exist */
   if(u == NULL)
      szonelog(boot_zone, "Reset Error: Don't know where to put %s@%s", cmd->fi[0]->name, cmd->fi[0]->zone->name);
   else if(cmd->fi[0]->type != UNIT_ST_OBJ && cmd->fi[0]->type != UNIT_ST_NPC)
      szonelog(boot_zone, "Reset Error: %s@%s loaded object is neither an obj nor npc.", cmd->fi[0]->name, cmd->fi[0]->zone->name);
   else if(zone_limit(u, cmd->fi[0], cmd))
   {
      loaded = read_unit(cmd->fi[0]);

      if(IS_MONEY(loaded))
         set_money(loaded, MONEY_AMOUNT(loaded));

      unit_to_unit(loaded, u);
      zone_loaded_a_unit(loaded);

      if(IS_CHAR(loaded))
      {
         act("$1n has arrived.", A_HIDEINV, loaded, 0, 0, TO_ROOM);
         UNIT_SIZE(loaded) += (UNIT_SIZE(loaded) * (55 - dice(10, 10))) / 300;
      }
   }

   return loaded;
}

/* fi[0] is unit to be loaded and equipped on parent unit.  */
/* num[0] is the max allowed existing number (0 ignores)    */
/* num[1] is equipment position                             */
std::shared_ptr<unit_data> zone_equip(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   std::shared_ptr<unit_data> loaded = NULL;

   /* Does the destination unit exist */
   if(u == NULL)
      szonelog(boot_zone, "Reset error: %s@%s has no parent in equip.", cmd->fi[0]->name, cmd->fi[0]->zone->name);
   else if(!IS_CHAR(u))
      szonelog(boot_zone, "Reset Error: %s@%s is not a char in equip.", cmd->fi[0]->name, cmd->fi[0]->zone->name);
   else if(cmd->fi[0]->type != UNIT_ST_OBJ)
      szonelog(boot_zone, "Reset Error: %s@%s is not an object in equip.", cmd->fi[0]->name, cmd->fi[0]->zone->name);
   else if(cmd->num[1] <= 0)
      szonelog(boot_zone, "Reset Error: %s@%s doesn't have a legal equip position.", cmd->fi[0]->name, cmd->fi[0]->zone->name);
   else if(!equipment(u, cmd->num[1]) && !(cmd->num[0] && cmd->fi[0]->no_in_mem >= (ubit16)(cmd->num[0])))
   {
      loaded = read_unit(cmd->fi[0]);

      unit_to_unit(loaded, u);
      zone_loaded_a_unit(loaded);

      if(IS_OBJ(loaded))
      {
#ifdef SUSPEKT
         if((cmd->num[1] == WEAR_WIELD) && (OBJ_TYPE(loaded) == ITEM_WEAPON))
         {
            int max = 0, i;
            for(i = 0; i < WPN_GROUP_MAX; i++)
               if(NPC_WPN_SKILL(u, i) > max)
                  max = NPC_WPN_SKILL(u, i);

            if(weapon_defense_skill(u, OBJ_VALUE(loaded, 0)) < max)
               szonelog(UNIT_FI_ZONE(u),
                        "%s@%s: Weapon NOT equipped "
                        "on best skill",
                        UNIT_FI_NAME(u),
                        UNIT_FI_ZONENAME(u));
         }
#endif
         equip_char(u, loaded, cmd->num[1]);
         UNIT_SIZE(loaded) = UNIT_SIZE(u); /* Autofit */
      }
   }

   return loaded;
}

/* fi[0] is room in which the door is located.              */
/* num[0] is the exit number (0..5)                         */
/* num[1] is the new state                                  */
std::shared_ptr<unit_data> zone_door(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   if(!cmd->fi || !cmd->fi[0]->room_ptr)
      szonelog(boot_zone, "Zone Reset Error: Not a room in door reference!");
   else if(!ROOM_EXIT(cmd->fi[0]->room_ptr, cmd->num[0]))
      szonelog(boot_zone, "Zone Reset Error: No %s direction from room %s in door.", dirs[cmd->num[0]], cmd->fi[0]->name);
   else
      ROOM_EXIT(cmd->fi[0]->room_ptr, cmd->num[0])->exit_info = cmd->num[1];

   return NULL;
}

/* fi[0] is the room to be purged.                          */
std::shared_ptr<unit_data> zone_purge(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   std::shared_ptr<unit_data> next;

   if(!cmd->fi[0]->room_ptr)
      szonelog(boot_zone, "Reset Error : No room in purge reference!");
   else
      for(u = UNIT_CONTAINS(cmd->fi[0]->room_ptr); u; u = next)
      {
         next = u->next;
         if(!IS_PC(u) && !IS_ROOM(u))
            extract_unit(u);
      }

   return NULL;
}

/* fi[0] is the thing(s) to be removed.                          */
/* fi[1] is the room to remove from.                             */
std::shared_ptr<unit_data> zone_remove(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   std::shared_ptr<unit_data> next;

   if(!cmd->fi[1]->room_ptr)
      szonelog(boot_zone, "Reset Error: No room in remove reference!");
   else
      for(u = UNIT_CONTAINS(cmd->fi[1]->room_ptr); u; u = next)
      {
         next = u->next;
         if(UNIT_FILE_INDEX(u) == cmd->fi[0] && !IS_ROOM(u))
            extract_unit(u);
      }

   return NULL;
}

/* 'u' is the 'master' that will be followed                      */
/* fi[0] is char to be loaded to  follow 'u'                      */
/* fi[1] -                                                        */
/* num[0] is the max allowed existing number (0 ignores) in world */
/* num[1] is the max allowed locally existing number              */
std::shared_ptr<unit_data> zone_follow(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   std::shared_ptr<unit_data> loaded = NULL;

   /* Does the master exist */
   if(u == NULL)
      szonelog(boot_zone, "Reset Error: Non Existant destination-unit in follow");
   else if(!IS_CHAR(u))
      szonelog(boot_zone, "Reset Error: Master to follow is not a mobile.");
   else if(cmd->fi[0]->type != UNIT_ST_NPC)
      szonelog(boot_zone, "Reset Error: Follower %s is not a mobile.", cmd->fi[0]->name);
   else if(zone_limit(u, cmd->fi[0], cmd))
   {
      loaded = read_unit(cmd->fi[0]);

      unit_to_unit(loaded, UNIT_IN(u));
      start_following(loaded, u);
      zone_loaded_a_unit(loaded);

      act("$1n has arrived.", A_HIDEINV, loaded, 0, 0, TO_ROOM);
   }

   return loaded;
}

std::shared_ptr<unit_data> (*exec_zone_cmd[])(std::shared_ptr<unit_data> , std::shared_ptr<zone_reset_cmd>) = {
   zone_nop, zone_load, zone_equip, zone_door, zone_purge, zone_remove, zone_follow, zone_random};

bool low_reset_zone(std::shared_ptr<unit_data> u, std::shared_ptr<zone_reset_cmd> cmd)
{
   std::shared_ptr<unit_data> success;
   bool              ok = TRUE;

   for(; cmd; cmd = cmd->next)
   {
      success = (*exec_zone_cmd[cmd->cmd_no])(u, cmd);
      if(success && cmd->nested && !low_reset_zone(success, cmd->nested) && cmd->cmpl)
      {
         extract_unit(success);
         success = 0;
      }

      ok = ok && success;
   }
   return ok;
}

void zone_reset(std::shared_ptr<zone_type> zone)
{
   /* extern int memory_total_alloc;
      int i = memory_total_alloc; */

   boot_zone = zone;

   zone_update_no_in_zone(); /* Reset the fi->no_in_zone */

   low_reset_zone(NULL, zone->zri);

   /* Far too much LOG:
   slog(LOG_OFF, 0, "Zone reset of '%s' done (%d bytes used).",
        zone->name, memory_total_alloc - i); */

   boot_zone = NULL;
}

/* MS: Changed this to queue reset events at boot such that game comes up
   really fast */
void reset_all_zones(void)
{
   int                        j, n;
   std::shared_ptr<zone_type> zone;

   void zone_event(void *, void *);

   for(n = j = 0; j <= 255; j++)
   {
      for(zone = zone_info.zone_list; zone; zone = zone->next)
      {
         if(j == 0)
            world_nozones++;

         if(zone->access != j)
            continue;

         if(zone->zone_time > 0)
            event_enq(++n * PULSE_SEC, zone_event, zone.get(), 0);
      }
   }
}

bool zone_is_empty(std::shared_ptr<zone_type> zone)
{
   struct descriptor_data *d;

   for(d = descriptor_list; d; d = d->next)
      if(descriptor_is_playing(d))
         if(unit_zone(d->character) == zone)
            return FALSE;

   return TRUE;
}

/* Check if any zones needs updating */
void zone_event(void *p1, void *p2)
{
   std::shared_ptr<zone_type> zone = std::shared_ptr<zone_type>(reinterpret_cast<zone_type *>(p1));

   if(zone->reset_mode != RESET_IFEMPTY || zone_is_empty(zone))
   {
      zone_reset(zone);

      /* Papi: Did a little random boogie to prevent reset all at the
       *       same time (causes lags!)
       */

      if(zone->reset_mode != RESET_NOT)
         event_enq(zone->zone_time * PULSE_ZONE + number(0, WAIT_SEC * 180), zone_event, zone.get(), 0);
   }
   else
      event_enq(1 * PULSE_ZONE, zone_event, zone.get(), 0);
}
