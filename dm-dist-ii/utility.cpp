/* *********************************************************************** *
 * File   : utility.c                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : all.                                                           *
 *                                                                         *
 * Purpose: Various functions.                                             *
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

#include "utility.h"

#include "files.h"

#include <cstdarg> /* va_args in slog()        */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#ifdef SUSPEKT /* Unused */
/* Entries is range of indexes (0..entries-1). Useful for arrays */
/* of strings like "bad", "average", "medium" with values.       */
int string_index(int entries, int value, int minv, int maxv)
{
   int step;
   int idx;

   step = (maxv - minv + 1) / entries;

   if(step == 0)
      return 0;

   idx = (value - minv) / step;

   /* Just for safety in case of illegal parameters */
   idx = std::max(0, idx);
   idx = std::min(idx, entries - 1);

   return idx;
}
#endif

#ifdef SOLARIS
long lrand48();
#endif

/* creates a random number in interval [from;to] */
auto number(int from, int to) -> int
{
   if(from > to)
   {
      slog(LOG_ALL, 0, "From %d and to %d in number()!", from, to);
      return (to - from) / 2;
   }

#ifdef GENERIC_SYSV
   return ((lrand48() % (to - from + 1)) + from);
#else
   return (int)((rand() % (to - from + 1)) + from);
#endif
}

/* simulates dice roll */
auto dice(int number, int size) -> int
{
   int r;
   int sum = 0;

   assert(size >= 1);

   for(r = 1; r <= number; r++)
   {
#ifdef GENERIC_SYSV
      sum += ((lrand48() % size) + 1);
#else
      sum += ((rand() % size) + 1);
   }
#endif

      return sum;
   }

   struct log_buffer log_buf[MAXLOG];

   /* writes a string to the log */
   void              slog(enum log_level level, uint8_t wizinv_level, const char *fmt, ...)
   {
      static uint8_t  idx      = 0;
      static uint32_t log_size = 0;

      char            buf[MAX_STRING_LENGTH];
      char           *t;
      va_list         args;

      time_t          now      = time(nullptr);
      char           *tmstr    = ctime(&now);

      tmstr[strlen(tmstr) - 1] = '\0';

      if(wizinv_level > 0)
      {
         sprintf(buf, "(%d) ", wizinv_level);
      }
      else
      {
         *buf = '\0';
      }

      t = buf;
      TAIL(t);

      va_start(args, fmt);
      vsprintf(t, fmt, args);
      va_end(args);

      /* 5 == " :: \n";  24 == tmstr (Tue Sep 20 18:41:23 1994)*/
      log_size += strlen(buf) + 5 + 24;

      if(log_size > 4000000) /* 4 meg is indeed a very big logfile! */
      {
         fprintf(stderr, "Log-file insanely big!  Going down.\n");
         abort(); // Dont use error, it calls syslog!!! *grin*
      }

      fprintf(stderr, "%s :: %s\n", tmstr, buf);

      if(level > LOG_OFF)
      {
         log_buf[idx].level        = level;
         log_buf[idx].wizinv_level = wizinv_level;
         strncpy(log_buf[idx].str, buf, sizeof(log_buf[idx].str) - 1);
         log_buf[idx].str[sizeof(log_buf[idx].str) - 1] = 0;

         idx++;
         idx %= MAXLOG; /* idx = 1 .. MAXLOG-1 */

         log_buf[idx].str[0] = 0;
      }
   }

   /* MS: Moved szonelog to handler.c to make this module independent. */

   /*  Replacing slog/assert(FALSE)
    *  usage: error(HERE, "Bad id: %d", id);
    */
   void error(const char *file, int line, const char *fmt, ...)
   {
      char    buf[512];
      va_list args;

      va_start(args, fmt);
      vsprintf(buf, fmt, args);
      va_end(args);

      slog(LOG_OFF, 0, "%s:%d: %s", file, line, buf);

      abort();
   }

   auto sprintbit(char *buf, uint32_t vektor, const char *names[])->char *
   {
      char *result = buf;
      long  nr;

      *result = '\0';

      for(nr = 0; vektor != 0U; vektor >>= 1, nr += names[nr] != nullptr ? 1 : 0)
      {
         if(IS_SET(1, vektor))
         {
            sprintf(result, "%s ", names[nr] != nullptr ? names[nr] : "UNDEFINED");
            TAIL(result);
         }
      }

      if(*buf == 0)
      {
         strcpy(buf, "NOBITS");
      }

      return buf;
   }
   /* MS2020. What a messed up function :)) Looks like noone calls with with
              anywhting but NULL as first parameter
   */
   auto sprinttype(char *buf, int type, const char *names[])->const char *
   {
      const char *str;
      int         nr;

      for(nr = 0; names[nr] != nullptr; nr++)
      {
         ;
      }

      str = (0 <= type && type < nr) ? (char *)names[type] : "UNDEFINED";

      if(buf != nullptr)
      {
         return strcpy(buf, str);
      }
      return str;
   }
