#include "comm.h"

#include "cHook.h"
#include "constants.h"
#include "handler.h"
#include "interpreter.h"
#include "protocol.h"
#include "structs.h"
#include "system.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

/* external vars */
extern descriptor_data *descriptor_list;

/*
 *  Public routines for system-to-player communication
 *  Sends directly to multiplexer.
 */


void send_to_descriptor(const char *messg, descriptor_data *d)
{
   void multi_close(struct multi_element * pe);

   if((d != nullptr) && (messg != nullptr) && (*messg != 0))
   {
      if(d->prompt_mode == PROMPT_IGNORE)
      {
         d->prompt_mode = PROMPT_EXPECT;
         send_to_descriptor("\n\r", d);
      }

      protocol_send_text(d->multi, d->id, messg, MULTI_TEXT_CHAR);

      if(d->snoop.snoop_by != nullptr)
      {
         send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->snoop.snoop_by));
         send_to_descriptor(messg, CHAR_DESCRIPTOR(d->snoop.snoop_by));
      }
   }
}
void send_to_descriptor(const std::string &messg, descriptor_data *d)
{
   send_to_descriptor(messg.c_str(), d);
}

void page_string(descriptor_data *d, const char *messg)
{
   if((d != nullptr) && (messg != nullptr) && (*messg != 0))
   {
      protocol_send_text(d->multi, d->id, messg, MULTI_PAGE_CHAR);

      if(d->snoop.snoop_by != nullptr)
      {
         send_to_descriptor(SNOOP_PROMPT, CHAR_DESCRIPTOR(d->snoop.snoop_by));
         send_to_descriptor(messg, CHAR_DESCRIPTOR(d->snoop.snoop_by));
      }
   }
}

void send_to_char(const std::string &messg, const unit_data *ch)
{
   if(IS_CHAR(ch))
   {
      send_to_descriptor(messg, CHAR_DESCRIPTOR(ch));
   }
}

void send_to_char(const char *messg, const unit_data *ch)
{
   if(IS_CHAR(ch))
   {
      send_to_descriptor(messg, CHAR_DESCRIPTOR(ch));
   }
}

void send_to_all(const char *messg)
{
   descriptor_data *i;

   if((messg != nullptr) && (*messg != 0))
   {
      for(i = descriptor_list; i != nullptr; i = i->next)
      {
         if(descriptor_is_playing(i) != 0)
         {
            send_to_descriptor(messg, i);
         }
      }
   }
}

void send_to_zone_outdoor(const struct zone_type *z, const char *messg)
{
   descriptor_data *i;

   if((messg != nullptr) && (*messg != 0))
   {
      for(i = descriptor_list; i != nullptr; i = i->next)
      {
         if((descriptor_is_playing(i) != 0) && UNIT_IS_OUTSIDE(i->character) && unit_zone(i->character) == z && CHAR_AWAKE(i->character) &&
            !IS_SET(UNIT_FLAGS(UNIT_IN(i->character)), UNIT_FL_NO_WEATHER) &&
            !IS_SET(UNIT_FLAGS(unit_room(i->character)), UNIT_FL_NO_WEATHER))
         {
            send_to_descriptor(messg, i);
         }
      }
   }
}

void send_to_outdoor(const char *messg)
{
   descriptor_data *i;

   if((messg != nullptr) && (*messg != 0))
   {
      for(i = descriptor_list; i != nullptr; i = i->next)
      {
         if((descriptor_is_playing(i) != 0) && UNIT_IS_OUTSIDE(i->character) && CHAR_AWAKE(i->character) &&
            !IS_SET(UNIT_FLAGS(UNIT_IN(i->character)), UNIT_FL_NO_WEATHER) &&
            !IS_SET(UNIT_FLAGS(unit_room(i->character)), UNIT_FL_NO_WEATHER))
         {
            send_to_descriptor(messg, i);
         }
      }
   }
}

void send_to_room(const char *messg, unit_data *room)
{
   unit_data *i;

   if(messg != nullptr)
   {
      for(i = UNIT_CONTAINS(room); i != nullptr; i = i->next)
      {
         if(IS_CHAR(i) && (CHAR_DESCRIPTOR(i) != nullptr))
         {
            send_to_descriptor(messg, CHAR_DESCRIPTOR(i));
         }
      }
   }
}

void act_generate(char *buf, const char *str, int show_type, const void *arg1, const void *arg2, const void *arg3, int type, unit_data *to)
{
   const char *strp;
   char       *point;
   const char *i = nullptr;

   union
   {
      const void *vo;
      unit_data  *un;
      const char *str;
      const int  *num;
   } sub;

   int uppercase = static_cast<int>(FALSE);

   *buf = 0;

   if(!IS_CHAR(to) || (CHAR_DESCRIPTOR(to) == nullptr) || arg1 == nullptr)
   {
      return;
   }

   if(to == (unit_data *)arg1 && (type == TO_ROOM || type == TO_NOTVICT || type == TO_REST))
   {
      return;
   }

   if(to == (unit_data *)arg3 && type == TO_NOTVICT)
   {
      return;
   }

   if(UNIT_IN(to) == (unit_data *)arg1 && type == TO_REST)
   {
      return;
   }

   if((show_type == A_HIDEINV && !CHAR_CAN_SEE(to, (unit_data *)arg1)) || (show_type != A_ALWAYS && !CHAR_AWAKE(to)))
   {
      return;
   }

   for(strp = str, point = buf;;)
   {
      if(*strp == '$')
      {
         switch(*++strp)
         {
            case '1':
               sub.vo = arg1;
               break;
            case '2':
               sub.vo = arg2;
               break;
            case '3':
               sub.vo = arg3;
               break;
            case '$':
               i = "$";
               break;

            default:
               slog(LOG_ALL, 0, "Illegal first code to act(): %s", str);
               *point = 0;
               return;
         }

         if(i == nullptr)
         {
            if(sub.vo != nullptr)
            {
               switch(*++strp)
               {
                  case 'n':
                     if(CHAR_CAN_SEE(to, sub.un))
                     {
                        if(IS_PC(sub.un))
                        {
                           /* Upper-case it */
                           uppercase = static_cast<int>(TRUE);
                           i         = UNIT_NAME(sub.un);
                        }
                        else
                        {
                           i = UNIT_TITLE(sub.un).String();
                        }
                     }
                     else
                     {
                        i = SOMETON(sub.un);
                     }
                     break;
                  case 'N':
                     i = UNIT_SEE_NAME(to, sub.un);
                     break;
                  case 'm':
                     i = HMHR(to, sub.un);
                     break;
                  case 's':
                     i = HSHR(to, sub.un);
                     break;
                  case 'e':
                     i = HESH(to, sub.un);
                     break;
                  case 'p':
                     if(IS_CHAR(sub.un))
                     {
                        i = char_pos[CHAR_POS(sub.un)];
                     }
                     else
                     {
                        i = "lying";
                     }
                     break;
                  case 'a':
                     i = UNIT_ANA(sub.un);
                     break;
                  case 'd':
                     i = itoa(*(sub.num));
                     break;
                  case 't':
                     i = sub.str;
                     break;
                  default:
                     slog(LOG_ALL, 0, "ERROR: Illegal second code to act(): %s", str);
                     *point = 0;
                     return;
               } /* switch */
            }
         }

         if(i == nullptr)
         {
            i = "NULL";
         }

         if((uppercase != 0) && (*i != 0))
         {
            *point++ = toupper(*i);
            i++;
            uppercase = static_cast<int>(FALSE);
         }

         while((*point = *(i++)) != 0)
         {
            point++;
         }

         i = nullptr;

         ++strp;
      }
      else if((*(point++) = *(strp++)) == 0)
      {
         break;
      }
   }

   *(point - 1) = '\n';
   *(point)     = '\r';
   *(point + 1) = 0;

   /* Cap the first letter, but skip all colour and control codes! */

   point = buf;
   while(*point == CONTROL_CHAR)
   {
      point += 2;
   }

   *point = toupper(*point);
}

void act(const char *str, int show_type, const void *arg1, const void *arg2, const void *arg3, int type)
{
   unit_data *to;
   unit_data *u;
   char       buf[MAX_STRING_LENGTH];

   /* This to catch old-style FALSE/TRUE calls...  */
   assert(show_type == A_SOMEONE || show_type == A_HIDEINV || show_type == A_ALWAYS);

   if((str == nullptr) || (*str == 0))
   {
      return;
   }

   if(type == TO_VICT)
   {
      to = (unit_data *)arg3;
   }
   else if(type == TO_CHAR)
   {
      to = (unit_data *)arg1;
   }
   else if(arg1 == nullptr || UNIT_IN((unit_data *)arg1) == nullptr)
   {
      return;
   }
   else
   {
      to = UNIT_CONTAINS(UNIT_IN((unit_data *)arg1));
   }

   /* same unit or to person */
   for(; to != nullptr; to = to->next)
   {
      if(IS_CHAR(to))
      {
         act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
         send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
      }

      if(type == TO_VICT || type == TO_CHAR)
      {
         return;
      }
      if((UNIT_CHARS(to) != 0u) && UNIT_IS_TRANSPARENT(to))
      {
         for(u = UNIT_CONTAINS(to); u != nullptr; u = u->next)
         {
            if(IS_CHAR(u))
            {
               act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
               send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
            }
         }
      }
   }

   /* other units outside transparent unit */
   if(((to = UNIT_IN(UNIT_IN((unit_data *)arg1))) != nullptr) && UNIT_IS_TRANSPARENT(UNIT_IN((unit_data *)arg1)))
   {
      for(to = UNIT_CONTAINS(to); to != nullptr; to = to->next)
      {
         if(IS_CHAR(to))
         {
            act_generate(buf, str, show_type, arg1, arg2, arg3, type, to);
            send_to_descriptor(buf, CHAR_DESCRIPTOR(to));
         }

         if((UNIT_CHARS(to) != 0u) && UNIT_IS_TRANSPARENT(to) && to != UNIT_IN((unit_data *)arg1))
         {
            for(u = UNIT_CONTAINS(to); u != nullptr; u = u->next)
            {
               if(IS_CHAR(u))
               {
                  act_generate(buf, str, show_type, arg1, arg2, arg3, type, u);
                  send_to_descriptor(buf, CHAR_DESCRIPTOR(u));
               }
            }
         }
      }
   }
}
