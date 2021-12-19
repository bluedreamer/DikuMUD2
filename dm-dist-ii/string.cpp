/* *********************************************************************** *
 * File   : string.c                                  Part of Valhalla MUD *
 * Version: 1.03                                                           *
 * Author : seifert@diku.dk / gnort@daimi.aau.dk                           *
 *                                                                         *
 * Purpose: To make a string allocation scheme for known constant strings. *
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
#include "hashstring.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"
#include "cHashSystem.h"
#include <cstdio>

#define STATISTICS 1

/* MS: there is a LOT of room for optimizations! Pass on hash-value
       exploit strlen in compare, use memcmp instead of strcmp. */

#if STATISTICS
unsigned long int alloced_strings    = 0;
unsigned long int alloced_memory     = 0;
unsigned long int potential_memory   = 0;
unsigned long int referenced_strings = 0;
#endif

#ifdef DMSERVER

void string_statistics(unit_data *ch)
{
   uint32_t depth;
   uint32_t slots;

   #if STATISTICS
   char              buf[4096];
   int               overhead = (int)(sizeof Hash + alloced_strings * sizeof(class cStringConstant));
   unsigned long int usage    = alloced_memory + overhead;

   sprintf(buf,
           "  %lu allocated strings with %lu references.\n\r"
           "  %lu allocated bytes (plus overhead %d = %lu bytes),\n\r"
           "  potentially allocated string-memory: %lu.\n\r"
           "  saved memory: %ld bytes\n\r",
           alloced_strings,
           referenced_strings,
           alloced_memory,
           overhead,
           usage,
           potential_memory,
           (long int)(potential_memory - usage));

   #else
   char *buf = "  Not available.\n\r";
   #endif

   send_to_char(buf, ch);

   depth = Hash.MaxDepth(&slots);

   sprintf(buf, "  Slots used in hash-list and deepest entry: %d/%d -- %d\n\r", slots, HASH_SIZE, depth);

   send_to_char(buf, ch);
}

#endif

/* ===================================================================== */

/* Prime-finder proggy:
main() {
  static char array[10000];
  int i, t;
  for (i = 2; i < 10000; i++)
    if (!array[i]) {
      printf("%d\n", i);
      for (t = i * 2; t < 10000; t += i)
        array[t] = 1;
    }
}
*/
