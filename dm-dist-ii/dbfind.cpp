#include "dbfind.h"

#include "dil.h"
#include "handler.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"
#include "zone_info_type.h"

#include <cstring>

extern descriptor_data *descriptor_list;

auto                    find_descriptor(const char *name, descriptor_data *except) -> descriptor_data *
{
   descriptor_data *d;

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
auto find_file_index(const char *zonename, const char *name) -> file_index_type *
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

   return (file_index_type *)ba->block;
}

/* Zonename & name must point to non-empty strings */
auto find_dil_index(char *zonename, char *name) -> diltemplate *
{
   struct zone_type       *zone;
   struct bin_search_type *ba;

   if(static_cast<unsigned int>(str_is_empty(name)) != 0U)
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

   return (diltemplate *)ba->block;
}

/*
 * This function searches for the
 * defined DIL template for external
 * call by a DIL program.
 * Uses find_dil_index...
 */
auto find_dil_template(const char *name) -> diltemplate *
{
   char zbuf[256];
   char pbuf[256];

   if(static_cast<unsigned int>(str_is_empty(name)) != 0U)
   {
      return nullptr;
   }

   split_fi_ref(name, zbuf, pbuf);

   return find_dil_index(zbuf, pbuf);
}

/*  Find a room in the world based on zone name and name e.g.
 *  "midgaard", "prison" and return a pointer to the room
 */
auto world_room(const char *zone, const char *name) -> unit_data *
{
   file_index_type *fi;

   return (fi = find_file_index(zone, name)) != nullptr ? fi->room_ptr : nullptr;
}

/*  Find file index.
 *  String MUST be in format 'name@zone\0' or 'zone/name'.
 */
auto str_to_file_index(const char *str) -> file_index_type *
{
   char name[FI_MAX_UNITNAME + 1];
   char zone[FI_MAX_ZONENAME + 1];

   split_fi_ref(str, zone, name);

   return find_file_index(zone, name);
}

/*  As str_to_file_index, except that if no zone is given, the
 *  zone of the 'ch' is assumed
 */
auto pc_str_to_file_index(const unit_data *ch, const char *str) -> file_index_type *
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
