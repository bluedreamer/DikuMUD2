#pragma once
/* *********************************************************************** *
 * File   : structs.h                                 Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
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

#include "bin_search_type.h"
#include "blkfile.h"
#include "combat.h"
#include "essential.h"
#include "extra.h"
#include "hashstring.h"
#include "protocol.h"
#include "queue.h"
#include "snoop_data.h"
#include "system.h"
#include "values.h"
#include "zone_reset_cmd.h"

#define FI_MAX_ZONENAME 30 /* Max length of any zone-name    */
#define FI_MAX_UNITNAME 15 /* Max length of any unit-name    */

#define MESS_ATTACKER 1
#define MESS_VICTIM   2
#define MESS_ROOM     3

/* For use in spec_assign.c's unit_function_array[] */
#define SD_NEVER 0 /* Never save this function           */
#define SD_NULL  1 /* Ignore fptr->data (save as 0 ptr)  */
#define SD_ASCII 2 /* If pointer, then it's ascii char * */

/* ------------------ OBJ SPECIFIC STRUCTURES ----------------------- */

/* ------------------  PC SPECIFIC STRUCTURES ------------------------ */

#include "pc_account_data.h"
#include "pc_time_data.h"

/* ------------------ NPC SPECIFIC STRUCTURES ----------------------- */

class npc_data
{
public:
   npc_data();
   ~npc_data();

   uint8_t weapons[WPN_GROUP_MAX];
   uint8_t spells[SPL_GROUP_MAX];

   uint8_t default_pos; /* Default position for NPC               */
   uint8_t flags;       /* flags for NPC behavior                 */
};

/* ----------------- CHAR SPECIFIC STRUCTURES ----------------------- */

struct char_point_data
{
   uint32_t flags; /* Char flags                               */
   int32_t  exp;   /* The experience of the player             */

   int16_t  mana;      /* How many mana points are left?           */
   int16_t  endurance; /* How many endurance points are left?      */
   uint16_t race;      /* PC/NPC race, Humanoid, Animal, etc.     */

   int16_t offensive; /* The OB of a character.                   */
   int16_t defensive; /* The DB of a character.                   */

   uint8_t speed;          /* The default speed for natural combat     */
   uint8_t natural_armour; /* The natural built-in armour (ARM_)       */
   uint8_t attack_type;    /* PC/NPC Attack Type for bare hands (WPN_) */

   uint8_t dex_reduction;            /* For speed of armour calculations only    */
   uint8_t sex;                      /* PC / NPC s sex                           */
   uint8_t level;                    /* PC / NPC s level                         */
   uint8_t position;                 /* Standing, sitting, fighting...           */
   uint8_t abilities[ABIL_TREE_MAX]; /* Str/dex etc.                 */
};

struct char_follow_type
{
   unit_data               *follower; /* Must be a char */
   struct char_follow_type *next;
};

class char_data
{
public:
   char_data();
   ~char_data();

   union
   {
      class pc_data  *pc;
      class npc_data *npc;
   } specific;

   char *money; /*  Money transfer from db-files.
                 *  Converted to real money when pc/npc is loaded.
                 */

   descriptor_data *descriptor;

   struct char_point_data points;

   class cCombat *Combat;

   struct char_follow_type *followers;
   unit_data               *master; /* Must be a char */

   unit_data *last_room; /* Last location of character */
};

/* ----------------- UNIT GENERAL STRUCTURES ----------------------- */

struct unit_affected_type
{
   int16_t  id;
   uint16_t beat;     /* Beat in 1/4 of secs, 0 = None */
   int16_t  duration; /* How many beats until end      */

   int data[3];

   int16_t firstf_i;
   int16_t tickf_i;
   int16_t lastf_i;
   int16_t applyf_i;

   unit_data                 *owner;
   struct unit_affected_type *next, *gnext, *gprevious;
};

struct unit_fptr
{
   uint16_t          index;      /* Index to function pointer array             */
   uint16_t          heart_beat; /* in 1/4 of a sec                             */
   uint16_t          flags;      /* When to override next function (boolean)    */
   void             *data;       /* Pointer to data local for this unit         */
   struct unit_fptr *next;       /* Next in linked list                         */
};

/* ----------------- Destructed decalrations ----------------------- */

#define DR_UNIT   0
#define DR_AFFECT 1
#define DR_FUNC   2

auto is_destructed(int i, void *ptr) -> int;
