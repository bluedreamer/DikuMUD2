#pragma once
#include "namelist.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include <cstdint>

class char_data;
class room_data;
class obj_data;
// class unit_affected_type;
class file_index_type;
class extra_descr_data;
class cStringInstance;

class unit_data
{
public:
   explicit unit_data(uint8_t type);
   ~unit_data();

   cNamelist names; /* Name Keyword list for get, enter, etc.      */

   union
   {
      char_data *ch;
      room_data *room;
      obj_data  *obj;
   } data;
   /* Function pointer type                      */
   unit_fptr *func;

   unit_affected_type *affected;

   file_index_type *fi; /* Unit file-index                               */

   file_index_type *key; /* Pointer to fileindex to Unit which is the key */

   unit_data *outside; /* Pointer out of the unit, ei. from an object   */
                       /* out to the char carrying it                   */
   unit_data *inside;  /* Linked list of chars,rooms & objs             */
                       /* For next unit in 'inside' linked list         */
   unit_data *next;
   /* global l-list of objects, chars & rooms       */
   unit_data *gnext;
   unit_data *gprevious;

   uint32_t manipulate;  /* WEAR_XXX macros                               */
   uint16_t flags;       /* Invisible, can_bury, burried...               */
   int16_t  base_weight; /* The "empty" weight of a room/char/obj         */
   int16_t  weight;      /* Current weight of a room/obj/char             */
   int16_t  capacity;    /* Capacity of obj/char/room, -1 => any          */
   uint16_t size;        /* (cm) MOBs height, weapons size, ropes length  */

   uint8_t status;     /* IS_ROOM, IS_OBJ, IS_PC, IS_NPC                */
   uint8_t open_flags; /* In general OPEN will mean can "enter"?        */
   int8_t  light;      /* Number of active light sources in unit        */
   int8_t  bright;     /* How much the unit shines                      */
   int8_t  illum;      /* how much bright is by transparency            */
   uint8_t chars;      /* How many chars is inside the unit             */
   uint8_t minv;       /* Level of wizard invisible                     */
   int32_t max_hp;     /* The maximum number of hitpoint                */
   int32_t hp;         /* The actual amount of hitpoints left           */

   int16_t alignment; /* +-1000 for alignments                         */

   /* MS2020 Cleaned up the swap stuff */

   /* Room title, Char title, Obj "the barrel", NPC "the Beastly Fido" */
   cStringInstance title;

   /* The outside description of a unit           */
   cStringInstance out_descr;

   /* The inside description of a unit            */
   cStringInstance in_descr;
   /* All the look 'at' stuff                     */
   extra_descr_data *extra_descr;
};
