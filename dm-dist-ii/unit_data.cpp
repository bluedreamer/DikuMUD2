#include "unit_data.h"
#include "file_index_type.h"
#include "handler.h"
#include "namelist.h"
#include "room_data.h"
#include "utils.h"
#include "values.h"

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

   void unlink_affect(unit_data * u, unit_affected_type * af);

   /* Sanity due to wierd bug I saw (MS, 30/05-95) */

#ifdef DMSERVER
   extern unit_data *unit_list;

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
