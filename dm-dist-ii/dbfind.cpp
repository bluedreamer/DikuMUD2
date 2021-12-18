/* *********************************************************************** *
 * File   : dbfind.c                                  Part of Valhalla MUD *
 * Version: 1.30                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Database stuff for locating database objects.                  *
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

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "db.h"
#include "dbfind.h"
#include "dil.h"
#include "handler.h"
#include "structs.h"
#include "textutil.h"
#include "unixshit.h"
#include "utility.h"
#include "utils.h"

extern struct descriptor_data *descriptor_list;

auto find_descriptor(const char *name, struct descriptor_data *except) -> struct descriptor_data *
{
   struct descriptor_data *d;

   /* Check if already playing */
   for(d = descriptor_list; d != nullptr; d = d->next)
   {
      if(d != except && str_ccmp(PC_FILENAME(CHAR_ORIGINAL(d->character)), name) == 0)
      {
         return d;
      }
   }

   return nullptr;
}

/*  Top is the size of the array (minimum 1).
 *  Returns pointer to element of array or null.
 *  Perhaps an index vs. -1 would be better?
 */
auto binary_search(struct bin_search_type *ba, const char *str, int top) -> struct bin_search_type *
{
   int mid = 0;
   int bot;
   int cmp;

   cmp = 1; /* Assume no compare                        */
   bot = 0; /* Point to lowest element in array         */
   top--;   /* Point to top element in array [0..top-1] */

   while(bot <= top)
   {
      mid = (bot + top) / 2;
      if((cmp = strcmp(str, ba[mid].compare)) < 0)
      {
         top = mid - 1;
      }
      else if(cmp > 0)
      {
         bot = mid + 1;
      }
      else
      { /* cmp == 0 */
         break;
      }
   }

   return (cmp != 0 ? nullptr : &ba[mid]);
}

/* Find a named zone */
auto find_zone(const char *zonename) -> struct zone_type *
{
   struct bin_search_type *ba;

   if((zonename == nullptr) || (*zonename == 0))
   {
      return nullptr;
   }

   ba = binary_search(zone_info.ba, zonename, zone_info.no_of_zones);

   return ba != nullptr ? (struct zone_type *)ba->block : nullptr;
}

/* Zonename & name must point to non-empty strings */
auto find_file_index(const char *zonename, const char *name) -> struct file_index_type *
{
   struct zone_type       *zone;
   struct bin_search_type *ba;

   if(*name == 0)
   {
      return nullptr;
   }

   if((zone = find_zone(zonename)) == nullptr)
   {
      return nullptr;
   }

   if((ba = binary_search(zone->ba, name, zone->no_of_fi)) == nullptr)
   {
      return nullptr;
   }

   return (struct file_index_type *)ba->block;
}

/* Zonename & name must point to non-empty strings */
auto find_dil_index(char *zonename, char *name) -> struct diltemplate *
{
   struct zone_type       *zone;
   struct bin_search_type *ba;

   if(str_is_empty(name) != 0u)
   {
      return nullptr;
   }

   if((zone = find_zone(zonename)) == nullptr)
   {
      return nullptr;
   }

   if((ba = binary_search(zone->tmplba, name, zone->no_tmpl)) == nullptr)
   {
      return nullptr;
   }

   return (struct diltemplate *)ba->block;
}

/*
 * This function searches for the
 * defined DIL template for external
 * call by a DIL program.
 * Uses find_dil_index...
 */
auto find_dil_template(const char *name) -> struct diltemplate *
{
   char zbuf[256];
   char pbuf[256];

   if(str_is_empty(name) != 0u)
   {
      return nullptr;
   }

   split_fi_ref(name, zbuf, pbuf);

   return find_dil_index(zbuf, pbuf);
}

/*  Find a room in the world based on zone name and name e.g.
 *  "midgaard", "prison" and return a pointer to the room
 */
auto world_room(const char *zone, const char *name) -> struct unit_data *
{
   struct file_index_type *fi;

   return (fi = find_file_index(zone, name)) != nullptr ? fi->room_ptr : nullptr;
}

/*  Find file index.
 *  String MUST be in format 'name@zone\0' or 'zone/name'.
 */
auto str_to_file_index(const char *str) -> struct file_index_type *
{
   char name[FI_MAX_UNITNAME + 1];
   char zone[FI_MAX_ZONENAME + 1];

   split_fi_ref(str, zone, name);

   return find_file_index(zone, name);
}

/*  As str_to_file_index, except that if no zone is given, the
 *  zone of the 'ch' is assumed
 */
auto pc_str_to_file_index(const struct unit_data *ch, const char *str) -> struct file_index_type *
{
   char name[MAX_INPUT_LENGTH + 1];
   char zone[MAX_INPUT_LENGTH + 1];

   split_fi_ref(str, zone, name);

   if((*name != 0) && (*zone == 0))
   {
      strcpy(zone, unit_zone(ch)->name);
   }

   return find_file_index(zone, name);
}
