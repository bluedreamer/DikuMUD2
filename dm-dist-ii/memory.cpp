/* *********************************************************************** *
 * File   : memory.c                                  Part of Valhalla MUD *
 * Version: 1.10                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Trapping illegal free() and realloc() calls.                   *
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

/* 28/03/93 gnort  : Fixed sanitycheck error                               */
/* 10/04/94 seifert: Added memory alloc bytes total.                       */
/* 29/09/94 seifert: Added overhead count and changed CRC to one byte.     */
/*                   This makes the system more shaky, but less mem hungry */

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "essential.h"

#define CRC_SIZE 4
#define RAN_SIZE 4

#ifdef MEMORY_DEBUG /* Endif is at the very bottom! */

uint32_t memory_total_limit = 64L * 1024L * 1024L;

uint32_t memory_total_alloc     = 0L;
uint32_t memory_total_overhead  = 0L;
uint32_t memory_dijkstra_alloc  = 0L;
uint32_t memory_pc_alloc        = 0L;
uint32_t memory_npc_alloc       = 0L;
uint32_t memory_obj_alloc       = 0L;
uint32_t memory_room_alloc      = 0L;
uint32_t memory_roomread_alloc  = 0L;
uint32_t memory_zoneidx_alloc   = 0L;
uint32_t memory_zonereset_alloc = 0L;

   #define MAX_MEMORY_CHUNK (1000000L) /* Purely for sanity checking! */

void memory_status(char *Buf)
{
   sprintf(Buf,
           "\n\rMemory Status:\n\r"
           "   %8d bytes soft coded memory limit.\n\r"
           "   %8d safe-chunks allocated (%d bytes overhead)\n\r"
           "   %8d bytes Total safe malloced memory.\n\r"
           "   %8d bytes used by dijkstra.\n\r"
           "   %8d bytes used by Zone indexes\n\r"
           "   %8d bytes used by room allocation\n\r"
           "   %8d bytes used by boottime reset of all zones.\n\r"
           "   %8d bytes used by PC alloc (very approx).\n\r"
           "   %8d bytes used by NPC alloc (very approx).\n\r"
           "   %8d bytes used by OBJ alloc (very approx).\n\r"
           "   %8d bytes used by ROOM alloc (very approx).\n\r",
           memory_total_limit, memory_total_overhead, memory_total_overhead * (RAN_SIZE + CRC_SIZE), memory_total_alloc,
           memory_dijkstra_alloc, memory_zoneidx_alloc, memory_roomread_alloc, memory_zonereset_alloc, memory_pc_alloc, memory_npc_alloc,
           memory_obj_alloc, memory_room_alloc);
}
#else
void memory_status(char *Buf)
{
   sprintf(Buf, "\n\rMemory Status:\n\r"
                "   Memory debug is off.\n\r");
}
#endif

#ifdef MEMORY_DEBUG /* Endif is at the very bottom! */
   #undef malloc
   #undef calloc
   #undef free
   #undef realloc

/* Set debug info into allocated memory block (size + info_Size) */
/* Save no of bytes alloced as RAN_SIZE                          */
void safe_set_info(void *p, uint32_t len)
{
   uint32_t crc;
   uint16_t low;
   uint16_t high;
   uint8_t *ptr;

   assert(len > 0);

   if(p == nullptr)
   {
      perror("Unable to satisfy memory allocation request");
      assert(FALSE);
   }

   crc = len ^ 0xAAAAAAAA;

   ptr = (uint8_t *)p;

   low  = crc & 0xFFFF;
   high = crc >> 16 & 0xFFFF;

   memcpy(ptr, &low, sizeof(uint16_t));
   memcpy(sizeof(uint16_t) + ptr, &len, sizeof(len));
   ptr = (uint8_t *)p + len - CRC_SIZE / 2;
   memcpy(ptr, &high, sizeof(uint16_t));
}

/* Reset and check debug info from memory block */
/* Return len of data checkked.                 */
auto safe_check_info(void *p) -> uint32_t
{
   uint32_t crc;
   uint16_t low;
   uint16_t high;
   uint32_t len;
   uint8_t *pEnd;

   if(p == nullptr)
   {
      perror("NULL pointer to memory block.");
      assert(FALSE);
   }

   pEnd = (uint8_t *)p;
   memcpy(&low, pEnd, sizeof(uint16_t));
   memcpy(&len, sizeof(uint16_t) + pEnd, sizeof(len));

   assert(len < MAX_MEMORY_CHUNK);

   pEnd = (uint8_t *)p + len - CRC_SIZE / 2;
   memcpy(&high, pEnd, sizeof(uint16_t));

   crc = (((uint32_t)high) << 16) | (uint32_t)low;

   if(crc != (len ^ 0xAAAAAAAA))
   {
      perror("CRC mismatch!");
      assert(FALSE);
   }

   return len;
}

auto safe_malloc(size_t size) -> void *
{
   void *p;

   assert(size > 0);
   assert(size < MAX_MEMORY_CHUNK);

   memory_total_alloc += size + RAN_SIZE + CRC_SIZE;
   assert(memory_total_alloc < memory_total_limit);

   p = malloc(size + RAN_SIZE + CRC_SIZE);
   memory_total_overhead++;

   safe_set_info(p, size + RAN_SIZE + CRC_SIZE);

   return (uint8_t *)p + RAN_SIZE + CRC_SIZE / 2; /* Skip control info */
}

auto safe_realloc(void *p, size_t size) -> void *
{
   assert(size > 0);

   p = (uint8_t *)p - (RAN_SIZE + CRC_SIZE / 2);

   memory_total_alloc -= safe_check_info(p);
   memory_total_alloc += size + RAN_SIZE + CRC_SIZE;
   assert(memory_total_alloc < memory_total_limit);

   p = realloc(p, size + RAN_SIZE + CRC_SIZE);
   safe_set_info(p, size + RAN_SIZE + CRC_SIZE);

   return (uint8_t *)p + RAN_SIZE + CRC_SIZE / 2; /* Skip control info */
}

auto safe_calloc(size_t nobj, size_t size) -> void *
{
   void  *p;
   size_t sum;

   sum = nobj * size;
   p   = safe_malloc(sum);
   memset(p, 0, sum);

   return p;
}

void safe_free(void *p)
{
   uint32_t len;

   if(p == nullptr)
   {
      assert(FALSE);
   }
   p   = (uint8_t *)p - (RAN_SIZE + CRC_SIZE / 2);
   len = safe_check_info(p);
   memset(p, 255, len);

   memory_total_alloc -= len;
   memory_total_overhead--;

   free((uint8_t *)p);
}
#endif
