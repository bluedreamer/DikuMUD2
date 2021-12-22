/* *********************************************************************** *
 * File   : mobact.c                                  Part of Valhalla MUD *
 * Version: 2.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Handling of calls of special procedure, including re-eventing  *
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

#include "comm.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "main.h"
#include "skills.h"
#include "structs.h"
#include "utility.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void special_event(void *p1, void *p2);
void event_enq(int when, void (*func)(), void *arg1, void *arg2);
void event_deenq(void (*func)(), void *arg1, void *arg2);

extern std::shared_ptr<zone_type> boot_zone;
extern struct unit_function_array_type unit_function_array[];

void SetFptrTimer(std::shared_ptr<unit_data> u, struct unit_fptr *fptr)
{
   ubit32 ticks;

   if((ticks = fptr->heart_beat) > 0)
   {
      if(ticks < PULSE_SEC)
      {
         szonelog(boot_zone, "Error: %s@%s had heartbeat of %d.", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u), ticks);
         ticks = fptr->heart_beat = PULSE_SEC * 3;
      }

      if(IS_SET(fptr->flags, SFB_RANTIME))
         ticks = number(ticks - ticks / 2, ticks + ticks / 2);

      event_enq(ticks, special_event, u, fptr);
   }
}

void ResetFptrTimer(std::shared_ptr<unit_data> u, struct unit_fptr *fptr)
{
   event_deenq(special_event, u, fptr);
   SetFptrTimer(u, fptr);
}

void special_event(void *p1, void *p2)
{
   std::shared_ptr<unit_data> u    = (std::shared_ptr<unit_data> )p1;
   register struct unit_fptr *fptr = (struct unit_fptr *)p2;

   ubit32            ret = SFR_SHARE, ticks;
   struct unit_fptr *ftmp;
   struct spec_arg   sarg;

   void add_func_history(std::shared_ptr<unit_data>  u, ubit16, ubit16);

   extern struct command_info cmd_auto_tick;

   if(g_cServerConfig.m_bNoSpecials)
      return;

   for(ftmp = UNIT_FUNC(u); ftmp; ftmp = ftmp->next)
   {
      if(ftmp != fptr)
      {
         if(IS_SET(ftmp->flags, SFB_PRIORITY))
            break;
      }
      else
      {
         /* If switched, disable all tick functions, so we can control
            the mother fucker! */
         if(!IS_CHAR(u) || !CHAR_IS_SWITCHED(u))
         {
            if(unit_function_array[fptr->index].func)
            {
               if(IS_SET(fptr->flags, SFB_TICK))
               {
#ifdef DEBUG_HISTORY
                  add_func_history(u, fptr->index, SFB_TICK);
#endif
                  sarg.owner     = u;
                  sarg.activator = NULL;
                  sarg.fptr      = fptr;
                  sarg.cmd       = &cmd_auto_tick;
                  sarg.arg       = "";
                  sarg.mflags    = SFB_TICK;
                  sarg.medium    = NULL;
                  sarg.target    = NULL;
                  sarg.pInt      = NULL;

                  ret = (*(unit_function_array[fptr->index].func))(&sarg);
               }
               assert((ret == SFR_SHARE) || (ret == SFR_BLOCK));
            }
            else
               slog(LOG_ALL, 0, "Null function call!");
         }

         break;
      }
   }

   if(is_destructed(DR_FUNC, fptr))
      return;

   SetFptrTimer(u, fptr);
}

/* Return TRUE while stopping events */
void stop_special(std::shared_ptr<unit_data> u, struct unit_fptr *fptr)
{
   event_deenq(special_event, u, fptr);
}

void start_special(std::shared_ptr<unit_data> u, struct unit_fptr *fptr)
{
   if(IS_SET(fptr->flags, SFB_TICK) || fptr->index == SFUN_DIL_INTERNAL)
   {
      /* If people forget to set the ticking functions... */
      if(fptr->heart_beat <= 0)
      {
         fptr->heart_beat = unit_function_array[fptr->index].tick;

         /* Well, the builders are supposed to fix it! That's why it is
            sent to the log, so they can see it! */
         /* HUUUGE amount of log :(  /gnort */
         /* Now we default for the suckers... no need to warn any more
         szonelog(boot_zone,
                  "%s@%s(%s): Heartbeat was 0 (idx %d), "
                  "set to defualt: %d",
                  UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u),
                  UNIT_NAME(u), fptr->index,
                  unit_function_array[fptr->index].tick); */
      }

      event_enq(fptr->heart_beat, special_event, u, fptr);
   }
}

void start_all_special(std::shared_ptr<unit_data> u)
{
   struct unit_fptr *fptr;

   for(fptr = UNIT_FUNC(u); fptr; fptr = fptr->next)
      start_special(u, fptr);
}

void stop_all_special(std::shared_ptr<unit_data> u)
{
   struct unit_fptr *fptr;

   for(fptr = UNIT_FUNC(u); fptr; fptr = fptr->next)
      stop_special(u, fptr);
}
