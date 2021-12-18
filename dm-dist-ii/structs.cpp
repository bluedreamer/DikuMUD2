/* *********************************************************************** *
 * File   : structs.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
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

#include "structs.h"
#include "handler.h"
#include "utils.h"

int world_norooms   = 0; /* number of rooms in the world   */
int world_noobjects = 0; /* number of objects in the world */
int world_nochars   = 0; /* number of chars in the world   */
int world_nonpc     = 0; /* number of chars in the world   */
int world_nopc      = 0; /* number of chars in the world   */
int world_nozones   = 0; /* number of zones in the world   */

/* Descriptor stuff is in system.c */

room_direction_data::room_direction_data()
{
   key       = nullptr;
   to_room   = nullptr;
   exit_info = 0;
}

room_direction_data::~room_direction_data() = default;

char_data::char_data()
{
   world_nochars++;

   specific.pc = nullptr;
   money       = nullptr;
   descriptor  = nullptr;
   Combat      = nullptr;
   followers   = nullptr;
   master      = nullptr;
   last_room   = nullptr;

   memset(&points, 0, sizeof(points));
}

char_data::~char_data()
{
   world_nochars--;
}

room_data::room_data()
{
   world_norooms++;
}

room_data::~room_data()
{
   world_norooms--;

   for(int i = 0; i <= 5; i++)
   {
      if(dir_option[i] != nullptr)
      {
         delete dir_option[i];
      }
   }
}

obj_data::obj_data()
{
   world_noobjects++;

   memset(value, 0, sizeof(value));
   cost         = 0;
   cost_per_day = 0;
   flags        = 0;
   type         = 0;
   equip_pos    = 0;
   resistance   = 0;
}

obj_data::~obj_data()
{
   world_noobjects--;
}

pc_data::pc_data()
{
   world_nopc++;

   info     = nullptr;
   bank     = nullptr;
   guild    = nullptr;
   hometown = nullptr;
   quest    = nullptr;

   memset(&setup, 0, sizeof(setup));
   memset(&time, 0, sizeof(setup));
   memset(&account, 0, sizeof(setup));

   guild_time = 0;
   vlvl       = 0;
   id         = -1;

   skill_points   = 0;
   ability_points = 0;
   flags          = 0;

   nr_of_crimes   = 0;
   crack_attempts = 0;

   memset(spells, 0, sizeof(spells));
   memset(spell_lvl, 0, sizeof(spell_lvl));
   memset(spell_cost, 0, sizeof(spell_cost));

   memset(skills, 0, sizeof(skills));
   memset(skill_lvl, 0, sizeof(skill_lvl));
   memset(skill_cost, 0, sizeof(skill_cost));

   memset(weapons, 0, sizeof(weapons));
   memset(weapon_lvl, 0, sizeof(weapon_lvl));
   memset(weapon_cost, 0, sizeof(weapon_cost));

   memset(ability_lvl, 0, sizeof(ability_lvl));
   memset(ability_cost, 0, sizeof(ability_cost));

   memset(conditions, 0, sizeof(conditions));

   nAccessLevel = 0;
   pwd[0]       = 0;
   filename[0]  = 0;
}

pc_data::~pc_data()
{
   world_nopc--;

   if(guild != nullptr)
   {
      free(guild);
   }

   if(hometown != nullptr)
   {
      free(hometown);
   }

   if(bank != nullptr)
   {
      free(bank);
   }

   info->free_list();
   quest->free_list();
}

npc_data::npc_data()
{
   world_nonpc++;

   memset(weapons, 0, sizeof(weapons));
   memset(spells, 0, sizeof(spells));
   default_pos = POSITION_STANDING;
   flags       = 0;
}

npc_data::~npc_data()
{
   world_nonpc--;
}

zone_type::zone_type()
{
   name     = nullptr;
   notes    = nullptr;
   help     = nullptr;
   filename = nullptr;

   fi = nullptr;
   ba = nullptr;

   zri  = nullptr;
   next = nullptr;

   tmpl   = nullptr;
   tmplba = nullptr;

   spmatrix = nullptr;

   access = 255;
}

zone_type::~zone_type()
{
   if(name != nullptr)
   {
      free(name);
   }

   if(title != nullptr)
   {
      free(title);
   }

   if(notes != nullptr)
   {
      free(notes);
   }

   if(help != nullptr)
   {
      free(help);
   }

   if(filename != nullptr)
   {
      free(filename);
   }

   struct file_index_type *p;
   struct file_index_type *nextfi;

   for(p = fi; p != nullptr; p = nextfi)
   {
      nextfi = p->next;
      delete p;
   }

   struct zone_reset_cmd *pzri;
   struct zone_reset_cmd *nextzri;

   for(pzri = zri; pzri != nullptr; pzri = nextzri)
   {
      nextzri = pzri->next;
      free(pzri);
   }

   /*
   struct diltemplate *pt, *nextpt;

   for (pt = tmpl; pt; pt = nextpt)
   {
      nextpt = pt->next;

      free(pt->prgname);
      free(pt->argt);
      free(pt->core);
      free(pt->vart);

      free(pt);
   }
*/

   // struct bin_search_type *ba;    /* Pointer to binarray of type      */
   // struct diltemplate *tmpl;      /* DIL templates in zone            */
   // struct bin_search_type *tmplba;/* Pointer to binarray of type      */

   free(spmatrix);
}

file_index_type::file_index_type()
{
   name     = nullptr;
   zone     = nullptr;
   next     = nullptr;
   room_ptr = nullptr;
}

file_index_type::~file_index_type()
{
   if(name != nullptr)
   {
      free(name);
   }
}

unit_data::unit_data(uint8_t type)
{
   status = type;

   data.ch     = nullptr;
   func        = nullptr;
   affected    = nullptr;
   fi          = nullptr;
   key         = nullptr;
   outside     = nullptr;
   inside      = nullptr;
   next        = nullptr;
   gnext       = nullptr;
   gprevious   = nullptr;
   extra_descr = nullptr;

   chars       = 0;
   manipulate  = 0;
   flags       = 0;
   base_weight = 0;
   weight      = 0;
   capacity    = 0;
   open_flags  = 0;
   light       = 0;
   bright      = 0;
   illum       = 0;
   chars       = 0;
   minv        = 0;
   max_hp      = 0;
   hp          = 0;
   alignment   = 0;

   if(IS_ROOM(this))
   {
      U_ROOM(this) = new(class room_data);
   }
   else if(IS_OBJ(this))
   {
      U_OBJ(this) = new(class obj_data);
   }
   else if(IS_CHAR(this))
   {
      U_CHAR(this) = new(class char_data);

      if(IS_PC(this))
      {
         U_PC(this) = new(class pc_data);
      }
      else
      {
         U_NPC(this) = new(class npc_data);
      }
   }
   else
   {
      assert(FALSE);
   }
}

unit_data::~unit_data()
{
#ifdef MEMORY_DEBUG
   extern int memory_pc_alloc;
   extern int memory_npc_alloc;
   extern int memory_obj_alloc;
   extern int memory_room_alloc;
   extern int memory_total_alloc;
   int        memory_start = memory_total_alloc;
#endif

   uint8_t type;

   void unlink_affect(struct unit_data * u, struct unit_affected_type * af);

   /* Sanity due to wierd bug I saw (MS, 30/05-95) */

#ifdef DMSERVER
   extern class unit_data *unit_list;

   assert(gnext == nullptr);
   assert(gprevious == nullptr);
   assert(next == nullptr);
   assert(unit_list != this);
#endif

   while(UNIT_FUNC(this))
   {
      destroy_fptr(this, UNIT_FUNC(this)); /* Unlinks, no free */
   }

   while(UNIT_AFFECTED(this))
   {
      unlink_affect(this, UNIT_AFFECTED(this));
   }

   type = UNIT_TYPE(this);

   /* Call functions of the unit which have any data                     */
   /* that they might want to work on.                                   */
   extra_descr->free_list();

   if(IS_OBJ(this))
   {
      delete U_OBJ(this);
   }
   else if(IS_ROOM(this))
   {
      delete U_ROOM(this);
   }
   else if(IS_CHAR(this))
   {
      if(IS_NPC(this))
      {
         delete U_NPC(this);
      }
      else
      {
         delete U_PC(this);
      }

      delete U_CHAR(this);
   }
   else
   {
      assert(FALSE);
   }

#ifdef MEMORY_DEBUG
   switch(type)
   {
      case UNIT_ST_PC:
         memory_pc_alloc -= memory_total_alloc - memory_start;
         break;
      case UNIT_ST_NPC:
         memory_npc_alloc -= memory_total_alloc - memory_start;
         break;
      case UNIT_ST_OBJ:
         memory_obj_alloc -= memory_total_alloc - memory_start;
         break;
      case UNIT_ST_ROOM:
         memory_room_alloc -= memory_total_alloc - memory_start;
         break;
   }
#endif
}
