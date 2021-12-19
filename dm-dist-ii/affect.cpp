/* *********************************************************************** *
 * File   : affect.c                                  Part of Valhalla MUD *
 * Version: 2.01                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Affect system (magic, 'events', etc.)                          *
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

/* 23/07/92 seifert: Corrected grave error with event_enq when destroyed   */
/* 02/08/92 seifert: Checks for destructed in various places               */

#include "affect.h"
#include "apply_function_type.h"
#include "main.h"
#include "structs.h"
#include "tick_function_type.h"
#include "unit_affected_type.h"
#include "utility.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>

unit_affected_type *affected_list = nullptr; /* Global list pointer       */
unit_affected_type *next_affected_dude;      /* dirty - very dirty indeed */

void register_destruct(int i, void *ptr);
void clear_destruct(int i);

void affect_beat(void * /*p1*/, void * /*p2*/);

/* Link an affected structure into the units affected structure */
void link_affect(unit_data *unit, unit_affected_type *af)
{
   /*if (af->id > ID_TOP_IDX)
     error(HERE, "%s@%s (%s) linked affect ID %d > max value.",
     UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit),
     UNIT_NAME(unit), af->id); */

   af->gprevious = nullptr;

   if(affected_list != nullptr)
   {
      af->gnext                = affected_list;
      affected_list->gprevious = af;
   }
   else
   {
      af->gnext = nullptr;
   }

   affected_list = af;

   af->next            = UNIT_AFFECTED(unit);
   UNIT_AFFECTED(unit) = af;
   af->owner           = unit;
}

auto link_alloc_affect(unit_data *unit, unit_affected_type *orgaf) -> unit_affected_type *
{
   unit_affected_type *af;

   CREATE(af, unit_affected_type, 1);
   assert(!is_destructed(DR_AFFECT, af));

   *af = *orgaf;

   link_affect(unit, af);

   return af;
}

/* Allocate memory, link and call functions of new affect   */
/* If the apf function returns TRUE then the tif - function */
/* is *not* called - but the structure is still alloced and */
/* linked.                                                  */
void create_affect(unit_data *unit, unit_affected_type *af)
{
   if(is_destructed(DR_UNIT, unit) == 0)
   {
      af = link_alloc_affect(unit, af);

      /* If less than zero it is a transfer! */
      if(af->id >= 0)
      {
         if(af->applyf_i >= 0)
         {
            if(static_cast<unsigned int>((*apf[af->applyf_i].func)(af, unit, TRUE)) == 0U)
            {
               return;
            }
         }

         if(af->firstf_i >= 0)
         {
            (*tif[af->firstf_i].func)(af, unit);
         }

         if(af->duration > 0)
         {
            af->duration--; /* When 1 it means stop next tick... */
         }

         if(af->beat > 0)
         {
            event_enq(af->beat, affect_beat, (void *)af, nullptr);
         }
      }
   }
}

/* Unlink  an affect structure from lists        */
/* It is freed by 'clear_destruct' automatically */
/* MS2020 added unit data as parameter. Shouldnt be necessary */
/* But I need it for sanity in DMC where there is an odd bug */
void unlink_affect(unit_data *u, unit_affected_type *af)
{
   unit_affected_type *i;

   assert(af->owner == u); // MS2020

   /* NB! Mucho importanto!                                    */
   /* Affects may never be removed by lower function than this */
   register_destruct(DR_AFFECT, af);

   event_deenq(affect_beat, (void *)af, nullptr);

   if(next_affected_dude == af)
   {
      next_affected_dude = af->gnext;
   }

   /* Unlink affect structure from global list of affects */

   if(affected_list == af)
   {
      affected_list = af->gnext;
   }

   if(af->gnext != nullptr)
   {
      af->gnext->gprevious = af->gprevious;
   }

   if(af->gprevious != nullptr)
   {
      af->gprevious->gnext = af->gnext;
   }

   /* Unlink affect structure from local list */

   i = UNIT_AFFECTED(af->owner);
   if(i == af)
   {
      UNIT_AFFECTED(af->owner) = i->next;
   }
   else
   {
      for(; i->next != af; i = i->next)
      {
         ;
      }

      assert(i);
      i->next = af->next;
   }
}

/* Call apply (unset), Unlink, and last function, then free structure */
/* If the apf funtion returns TRUE then the affect will neither be    */
/* Unliked nor freed nor will the tif funtion be called               */
void destroy_affect(unit_affected_type *af)
{
   /* It is assumed that none of these function calls can */
   /* destroy the affect.                                 */
   if(af->id >= 0)
   {
      if(af->applyf_i >= 0)
      {
         if(static_cast<unsigned int>((*apf[af->applyf_i].func)(af, af->owner, FALSE)) == 0U)
         {
            af->duration = 0;
            af->beat     = WAIT_SEC * 5;
            event_enq(number(120, 240), affect_beat, (void *)af, nullptr);
            return;
         }
      }

      if(af->lastf_i >= 0 && (is_destructed(DR_UNIT, af->owner) == 0))
      {
         (*tif[af->lastf_i].func)(af, af->owner);
      }
   }

   assert(!is_destructed(DR_AFFECT, af));
   unlink_affect(af->owner, af);
}

/* Attempts to clear a unit entirely of affects */
void affect_clear_unit(unit_data *unit)
{
   int                 i;
   unit_affected_type *taf1;
   unit_affected_type *taf2;

   /* Some affects may not be destroyed at first attempt if it would */
   /* cause an overflow, therefore do several attemps to destroy     */
   for(i = 0; (UNIT_AFFECTED(unit) != nullptr) && (i < 5); i++)
   {
      for(taf1 = UNIT_AFFECTED(unit); taf1 != nullptr; taf1 = taf2)
      {
         taf2 = taf1->next;
         destroy_affect(taf1);
      }
   }

   if(UNIT_AFFECTED(unit) != nullptr)
   {
      slog(LOG_ALL, 0, "ERROR: Could not clear unit of affects!");
   }
}

auto affected_by_spell(const unit_data *unit, int16_t id) -> unit_affected_type *
{
   unit_affected_type *af;

   for(af = UNIT_AFFECTED(const_cast<unit_data *>(unit)); af != nullptr; af = af->next)
   {
      if(af->id == id)
      {
         return af;
      }
   }

   return nullptr;
}

/* Called by event handler when its ticking time */
void affect_beat(void *p1, void *p2)
{
   auto *af = (unit_affected_type *)p1;
   int   destroyed;

   assert(af->id >= 0); /* Negative ids (transfer) dont have beats */

   /* Used to be assert(af->beat > 0);  */
   /* But crashes game, I've set 0 to 8 */
   if(af->beat <= 0)
   {
      af->beat = 2 * WAIT_SEC;
   }

   destroyed = static_cast<int>(FALSE);

   if(!IS_PC(af->owner) || (CHAR_DESCRIPTOR(af->owner) != nullptr))
   {
      if(af->duration == 0)
      {
         /* 'destroy_affect' will try to destroy the event, but there */
         /* is none. It doesn't matter though                         */
         destroy_affect(af);
         return;
      }

      if(af->tickf_i >= 0)
      {
         (*tif[af->tickf_i].func)(af, af->owner);
      }

      destroyed = is_destructed(DR_AFFECT, af);

      if((destroyed == 0) && (af->duration > 0))
      {
         af->duration--;
      }
   }
   if(destroyed == 0)
   {
      event_enq(af->beat, affect_beat, (void *)af, nullptr);
   }
}

/* ONLY USED WHEN LOADING UNITS                          */
/* If 'apply' is TRUE then apply function will be called */
void apply_affect(unit_data *unit)
{
   unit_affected_type *af;

   /* If less than zero it is a transfer, and nothing will be set */
   for(af = UNIT_AFFECTED(unit); af != nullptr; af = af->next)
   {
      if((af->id >= 0) && (af->applyf_i >= 0))
      {
         if(static_cast<unsigned int>((*apf[af->applyf_i].func)(af, unit, TRUE)) == 0U)
         {
            continue;
         }
      }
   }
}

void start_affect(unit_data *unit)
{
   unit_affected_type *af;

   /* If less than zero it is a transfer, and nothing will be set */
   for(af = UNIT_AFFECTED(unit); af != nullptr; af = af->next)
   {
      if((af->id >= 0) && (af->beat > 0))
      {
         event_enq(af->beat, affect_beat, (void *)af, nullptr);
      }
   }
}

void stop_affect(unit_data *unit)
{
   unit_affected_type *af;

   for(af = UNIT_AFFECTED(unit); af != nullptr; af = af->next)
   {
      event_deenq(affect_beat, (void *)af, nullptr);
   }
}
