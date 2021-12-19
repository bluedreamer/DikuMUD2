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

#define PC_MAX_PASSWORD 12 /* Max length of any pc-password  */
#define PC_MAX_NAME     15 /* 14 Characters + Null           */

#define MESS_ATTACKER 1
#define MESS_VICTIM   2
#define MESS_ROOM     3

/* For use in spec_assign.c's unit_function_array[] */
#define SD_NEVER 0 /* Never save this function           */
#define SD_NULL  1 /* Ignore fptr->data (save as 0 ptr)  */
#define SD_ASCII 2 /* If pointer, then it's ascii char * */

/* ----------------- DATABASE STRUCTURES ----------------------- */

/* ----------------- OTHER STRUCTURES ----------------------- */

/* --------------------- DESCRIPTOR STRUCTURES -------------------- */

/* ----------------- ROOM SPECIFIC STRUCTURES ----------------------- */

class room_direction_data
{
public:
   room_direction_data();
   ~room_direction_data();

   class cNamelist open_name; /* For Open & Enter                  */

   file_index_type *key;
   unit_data       *to_room;

   uint8_t exit_info; /* Door info flags                   */
};

class room_data
{
public:
   room_data();
   ~room_data();

   class room_direction_data *dir_option[6]; /* Her?? */

   uint8_t flags;         /* Room flags                              */
   uint8_t movement_type; /* The type of movement (city, hills etc.) */
   uint8_t resistance;    /* Magic resistance of the room            */
};

/* ------------------ OBJ SPECIFIC STRUCTURES ----------------------- */

class obj_data
{
public:
   obj_data();
   ~obj_data();

   int32_t  value[5];     /* Values of the item (see list)       */
   uint32_t cost;         /* Value when sold (gp.)               */
   uint32_t cost_per_day; /* Cost to keep pr. real day           */

   uint8_t flags;      /* Various special object flags        */
   uint8_t type;       /* Type of item (ITEM_XXX)             */
   uint8_t equip_pos;  /* 0 or position of item in equipment  */
   uint8_t resistance; /* Magic resistance                    */
};

/* ------------------  PC SPECIFIC STRUCTURES ------------------------ */
struct pc_time_data
{
   time_t   creation; /* This represents time when the pc was created.     */
   time_t   connect;  /* This is the last time that the pc connected.      */
   time_t   birth;    /* This represents the characters age                */
   uint32_t played;   /* This is the total accumulated time played in secs */
};

struct pc_account_data
{
   float    credit;       /* How many coin units are left on account?       */
   uint32_t credit_limit; /* In coin units (i.e. cents / oerer)             */
   uint32_t total_credit; /* Accumulated credit to date (coin units)        */
   int16_t  last4;        /* The last four digits of his credit card, or -1 */
   uint8_t  cracks;       /* Crack-attempts on CC last4                     */
   uint8_t  discount;     /* 0 - 100% discount                              */
   uint32_t flatrate;     /* The expiration date of a flat rate service     */
};

class pc_data
{
public:
   pc_data();
   ~pc_data();

   struct terminal_setup_type setup;

   struct pc_time_data    time;    /* PCs time info  */
   struct pc_account_data account; /* Accounting     */

   char *guild;    /* Which guild is the player a member of?  */
   char *bank;     /* How much money in bank?                 */
   char *hometown; /* PCs Hometown (symbolic reference)       */

   class extra_descr_data /* For saving INFO information             */
      *info;

   class extra_descr_data /* For saving QUEST information            */
      *quest;

   uint32_t guild_time; /* When (playing secs) player entered      */
   uint16_t vlvl;       /* Virtual Level for player                */
   int32_t  id;         /* Unique identifier for each player (-1 guest) */

   int32_t  skill_points;   /* No of practice points left              */
   int32_t  ability_points; /* No of practice points left              */
   uint16_t flags;          /* flags for PC setup (brief, noshout...)  */

   uint16_t nr_of_crimes;   /* Number of crimes committed              */
   uint16_t crack_attempts; /* Number of wrong passwords entered       */
   uint16_t lifespan;       /* How many year to live....               */

   uint8_t spells[SPL_TREE_MAX];     /* The spells learned                  */
   uint8_t spell_lvl[SPL_TREE_MAX];  /* Practiced within that level         */
   int8_t  spell_cost[SPL_TREE_MAX]; /* Cost modifier                       */

   uint8_t skills[SKI_TREE_MAX];     /* The skills learned                  */
   uint8_t skill_lvl[SKI_TREE_MAX];  /* The skills practiced within level   */
   int8_t  skill_cost[SKI_TREE_MAX]; /* Cost modifier                       */

   uint8_t weapons[WPN_TREE_MAX];     /* The weapons learned                 */
   uint8_t weapon_lvl[WPN_TREE_MAX];  /* The spells learned                  */
   int8_t  weapon_cost[WPN_TREE_MAX]; /* Cost modifier                       */

   uint8_t ability_lvl[ABIL_TREE_MAX];  /* The spells learned                  */
   int8_t  ability_cost[ABIL_TREE_MAX]; /* Cost modifier                       */

   int8_t  conditions[3]; /* Drunk full etc.                     */
   uint8_t nAccessLevel;  /* Access Level for BBS use            */

   char     pwd[PC_MAX_PASSWORD];  /* Needed when loaded w/o descriptor   */
   char     filename[PC_MAX_NAME]; /* The name on disk...                 */
   uint32_t lasthosts[5];          /* last 5 different IPs                */
};

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
