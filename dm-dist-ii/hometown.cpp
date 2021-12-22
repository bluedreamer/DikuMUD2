/* *********************************************************************** *
 * File   : hometown.c                                Part of Valhalla MUD *
 * Version: 1.01                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Making several home/start towns avaible in the game            *
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
/* 28-Aug gnort: hometown_unit didn't test if str was NULL! */

#include "comm.h"
#include "db.h"
#include "db_file.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ----- FUNCTIONS ----- */

std::shared_ptr<unit_data> hometown_unit(char *str)
{
   if(str)
   {
      char                       name[80], zone[80];
      std::shared_ptr<unit_data> u;

      split_fi_ref(str, zone, name);
      if((u = world_room(zone, name)))
         return u;
   }

   return entry_room;
}

int change_hometown(struct spec_arg *sarg)
{
   if(!is_command(sarg->cmd, "moveto"))
      return SFR_SHARE;

   send_to_char("Defunct at the moment...\n\r", sarg->activator);

   return SFR_BLOCK;
}
