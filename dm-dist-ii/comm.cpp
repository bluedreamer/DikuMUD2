/* *********************************************************************** *
 * File   : comm.c                                    Part of Valhalla MUD *
 * Version: 1.30                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Communications.                                                *
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

/* 26/07/92 seifert: Changed close_socket to match redesign of extract     */
/* 29/07/92 seifert: Changed close_socket to new menu struct               */
/* 30/07/92 seifert: Mana added to prompt after several requests           */
/* 14/08/92 quinn  : Changed eventqueue to a heap for performance.         */
/* 22/08/92 bombman: Split up of comm.c to isolate net stuff               */
/* 13/10/93 bombman: Make act take care of transparent rooms               */
/* 25/03/94 bombman: Edited act, to correct leaking from non !transp. unit */
/* 21/07/94 gnort  : Removed dollar doubling on input, which is ok, as     */
/*                   player input is NEVER fed directly to act             */

#include "comm.h"

#include "constants.h"
#include "handler.h"
#include "interpreter.h"
#include "protocol.h"
#include "structs.h"
#include "sysport.h"
#include "system.h"
#include "textutil.h"
#include "unixshit.h"
#include "utility.h"
#include "utils.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *  Public routines for system-to-player communication
 *  Sends directly to multiplexer.
 */

void send_to_descriptor(const char *messg, struct descriptor_data *d)
{
   void multi_close(struct multi_element * pe);

   if(d && messg && *messg)
   {
      if(d->prompt_mode == PROMPT_IGNORE)
      {
         d->prompt_mode = PROMPT_EXPECT;
         send_to_descriptor("\n\r", d);
      }

      protocol_send_text(d->multi, d->id, messg, MULTI_TEXT_CHAR);

      if(d->snoop.snoop_by)
      {
         send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->snoop.snoop_by));
         send_to_descriptor(messg, CHAR_DESCRIPTOR(d->snoop.snoop_by));
      }
   }
}

void page_string(struct descriptor_data *d, const char *messg)
{
   void multi_close(struct multi_element * pe);

   if(d && messg && *messg)
   {
      protocol_send_text(d->multi, d->id, messg, MULTI_PAGE_CHAR);

      if(d->snoop.snoop_by)
      {
         send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->snoop.snoop_by));
         send_to_descriptor(messg, CHAR_DESCRIPTOR(d->snoop.snoop_by));
      }
   }
}

void send_to_char(const char *messg, const std::shared_ptr<unit_data> ch)
{
   if(IS_CHAR(ch))
      send_to_descriptor(messg, CHAR_DESCRIPTOR(ch));
}

void send_to_all(const char *messg)
{
   struct descriptor_data *i;

   if(messg && *messg)
      for(i = descriptor_list; i; i = i->next)
         if(descriptor_is_playing(i))
            send_to_descriptor(messg, i);
}

void send_to_zone_outdoor(const std::shared_ptr<zone_type> z, const char *messg)
{
   struct descriptor_data *i;

   if(messg && *messg)
      for(i = descriptor_list; i; i = i->next)
         if(descriptor_is_playing(i) && UNIT_IS_OUTSIDE(i->character) && unit_zone(i->character) == z && CHAR_AWAKE(i->character) &&
            !IS_SET(UNIT_FLAGS(UNIT_IN(i->character)), UNIT_FL_NO_WEATHER) &&
            !IS_SET(UNIT_FLAGS(unit_room(i->character)), UNIT_FL_NO_WEATHER))
            send_to_descriptor(messg, i);
}

void send_to_outdoor(const char *messg)
{
   struct descriptor_data *i;

   if(messg && *messg)
      for(i = descriptor_list; i; i = i->next)
         if(descriptor_is_playing(i) && UNIT_IS_OUTSIDE(i->character) && CHAR_AWAKE(i->character) &&
            !IS_SET(UNIT_FLAGS(UNIT_IN(i->character)), UNIT_FL_NO_WEATHER) &&
            !IS_SET(UNIT_FLAGS(unit_room(i->character)), UNIT_FL_NO_WEATHER))
            send_to_descriptor(messg, i);
}

void send_to_room(const char *messg, std::shared_ptr<unit_data> room)
{
   std::shared_ptr<unit_data> i;

   if(messg)
      for(i = UNIT_CONTAINS(room); i; i = i->next)
         if(IS_CHAR(i) && CHAR_DESCRIPTOR(i))
            send_to_descriptor(messg, CHAR_DESCRIPTOR(i));
}

void act_generate(char                      *buf,
                  const char                *str,
                  int                        show_type,
                  comm_variant               arg1,
                  comm_variant               arg2,
                  comm_variant               arg3,
                  int                        type,
                  std::shared_ptr<unit_data> to)
{
   const char *strp;
   char       *point;
   const char *i = NULL;

   comm_variant sub;
   /*
   union
  {
     const void      *vo;
     class unit_data *un;
     const char      *str;
     const int       *num;
  } sub;
*/
   int          uppercase = FALSE;

   *buf = 0;

   if(!IS_CHAR(to) || !CHAR_DESCRIPTOR(to) || std::holds_alternative<std::monostate>(arg1))
      return;

   if(to == std::get<std::shared_ptr<unit_data>>(arg1) && (type == TO_ROOM || type == TO_NOTVICT || type == TO_REST))
      return;

   if(to == std::get<std::shared_ptr<unit_data>>(arg3) && type == TO_NOTVICT)
      return;

   if(UNIT_IN(to) == std::get<std::shared_ptr<unit_data>>(arg1) && type == TO_REST)
      return;

   if((show_type == A_HIDEINV && !CHAR_CAN_SEE(to, std::get<std::shared_ptr<unit_data>>(arg1))) ||
      (show_type != A_ALWAYS && !CHAR_AWAKE(to)))
      return;

   for(strp = str, point = buf;;)
   {
      if(*strp == '$')
      {
         switch(*++strp)
         {
            case '1':
               sub = arg1;
               break;
            case '2':
               sub = arg2;
               break;
            case '3':
               sub = arg3;
               break;
            case '$':
               i = "$";
               break;

            default:
               slog(LOG_ALL, 0, "Illegal first code to act(): %s", str);
               *point = 0;
               return;
         }

         if(i == NULL)
         {
            if(!std::holds_alternative<std::monostate>(sub))
            {
               switch(*++strp)
               {
                  case 'n':
                  {
                     auto un = std::get<std::shared_ptr<unit_data>>(sub);
                     if(CHAR_CAN_SEE(to, un))
                     {
                        if(IS_PC(un))
                        {
                           /* Upper-case it */
                           uppercase = TRUE;
                           i         = UNIT_NAME(un);
                        }
                        else
                           i = UNIT_TITLE(un).String();
                     }
                     else
                        i = SOMETON(un);
                     break;
                  }
                  case 'N':
                  {
                     auto un = std::get<std::shared_ptr<unit_data>>(sub);
                     i       = UNIT_SEE_NAME(to, un);
                     break;
                  }
                  case 'm':
                  {
                     auto un = std::get<std::shared_ptr<unit_data>>(sub);
                     i       = HMHR(to, un);
                     break;
                  }
                  case 's':
                  {
                     auto un = std::get<std::shared_ptr<unit_data>>(sub);
                     i       = HSHR(to, un);
                     break;
                  }
                  case 'e':
                  {
                     auto un = std::get<std::shared_ptr<unit_data>>(sub);
                     i       = HESH(to, un);
                     break;
                  }
                  case 'p':
                  {
                     auto un = std::get<std::shared_ptr<unit_data>>(sub);
                     if(IS_CHAR(un))
                        i = char_pos[CHAR_POS(un)];
                     else
                        i = "lying";
                     break;
                  }
                  case 'a':
                  {
                     auto un = std::get<std::shared_ptr<unit_data>>(sub);
                     i       = UNIT_ANA(un);
                     break;
                  }
                  case 'd':
                     i = itoa(*std::get<int *>(sub));
                     break;
                  case 't':
                     i = std::get<const char *>(sub);
                     break;
                  default:
                     slog(LOG_ALL, 0, "ERROR: Illegal second code to act(): %s", str);
                     *point = 0;
                     return;
               } /* switch */
            }
         }

         if(i == NULL)
            i = "NULL";

         if(uppercase && *i)
         {
            *point++ = toupper(*i);
            i++;
            uppercase = FALSE;
         }

         while((*point = *(i++)))
            point++;

         i = NULL;

         ++strp;
      }
      else if(!(*(point++) = *(strp++)))
         break;
   }

   *(point - 1) = '\n';
   *(point)     = '\r';
   *(point + 1) = 0;

   /* Cap the first letter, but skip all colour and control codes! */

   point = buf;
   while(*point == CONTROL_CHAR)
      point += 2;

   *point = toupper(*point);
}

void act(const char *str, int show_type, comm_variant arg1, comm_variant arg2, comm_variant arg3, int type)
{
   std::shared_ptr<unit_data> to;
   std::shared_ptr<unit_data> u;
   char                       buf[MAX_STRING_LENGTH];

   /* This to catch old-style FALSE/TRUE calls...  */
   assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

   if(!str || !*str)
      return;

   if(type == TO_VICT)
      to = std::get<std::shared_ptr<unit_data>>(arg3);
   else if(type == TO_CHAR)
      to = std::get<std::shared_ptr<unit_data>>(arg1);
   else if(std::holds_alternative<std::monostate>(arg1) || UNIT_IN(std::get<std::shared_ptr<unit_data>>(arg1)) == NULL)
      return;
   else
      to = UNIT_CONTAINS(UNIT_IN(std::get<std::shared_ptr<unit_data>>(arg1)));

   /* same unit or to person */
   for(; to; to = to->next)
   {
      if(IS_CHAR(to))
      {
         act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
         send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
      }

      if(type == TO_VICT || type == TO_CHAR)
         return;
      if(UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to))
         for(u = UNIT_CONTAINS(to); u; u = u->next)
            if IS_CHAR(u)
            {
               act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
               send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
            }
   }

   /* other units outside transparent unit */
   if((to = UNIT_IN(UNIT_IN(std::get<std::shared_ptr<unit_data>>(arg1)))) &&
      UNIT_IS_TRANSPARENT(UNIT_IN(std::get<std::shared_ptr<unit_data>>(arg1))))
      for(to = UNIT_CONTAINS(to); to; to = to->next)
      {
         if(IS_CHAR(to))
         {
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
            send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
         }

         if(UNIT_CHARS(to) && UNIT_IS_TRANSPARENT(to) && to != UNIT_IN(std::get<std::shared_ptr<unit_data>>(arg1)))
            for(u = UNIT_CONTAINS(to); u; u = u->next)
               if(IS_CHAR(u))
               {
                  act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
                  send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
               }
      }
}
