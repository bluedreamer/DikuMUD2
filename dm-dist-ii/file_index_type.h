#pragma once
#include "zone_type.h"
#include <cstdint>

class unit_data;

/* A linked sorted list of all units within a zone file */
class file_index_type
{
public:
   file_index_type();
   ~file_index_type();

   char            *name;     /* Unique within this list          */
   zone_type       *zone;     /* Pointer to owner of structure    */
   file_index_type *next;     /* Next File Index                  */
   unit_data       *room_ptr; /* Pointer to room if is room       */

   long     filepos; /* Byte offset into file            */
   uint32_t length;  /* No of bytes to read              */
   uint32_t crc;     /* CRC check for compessed items    */

   int16_t  no_in_zone; /* Updated in zone reset for reset  */
   uint16_t no_in_mem;  /* Number of these in the game      */
   uint16_t room_no;    /* The number of the room           */
   uint8_t  type;       /* Room/Obj/Char or other?          */
};
