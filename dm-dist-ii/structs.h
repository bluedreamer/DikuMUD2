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

#ifndef _MUD_STRUCTS_H
#define _MUD_STRUCTS_H

#include "blkfile.h"
#include "combat.h"
#include "essential.h"
#include "extra.h"
#include "hashstring.h"
#include "protocol.h"
#include "queue.h"
#include "system.h"
#include "values.h"

#include <memory>
#include <string>
#include <variant>

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

class file_index_type;
class zone_type;
class diltemplate;
/* This must be maintained as an array for use with binary search methods */
struct bin_search_type
{
   const char                                                             *compare; /* Points to the comparison string  */
   std::variant<std::monostate, std::shared_ptr<zone_type>, diltemplate *> block;   /* Points to the relevant block     */
   std::shared_ptr<file_index_type>                                        fi_block;
};

class zone_type;
/* A linked sorted list of all units within a zone file */
class file_index_type : public std::enable_shared_from_this<file_index_type>
{
   file_index_type();
   file_index_type(const file_index_type &) = delete;
   file_index_type(file_index_type &&)      = delete;
   auto operator=(const file_index_type &) -> file_index_type & = delete;
   auto operator=(file_index_type &&) -> file_index_type & = delete;

public:
   ~file_index_type() = default;
   std::shared_ptr<file_index_type>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<file_index_type> Create() { return std::shared_ptr<file_index_type>(new file_index_type); }

   std::string                      name;     /* Unique within this list          */
   std::shared_ptr<zone_type>       zone;     /* Pointer to owner of structure    */
   std::shared_ptr<file_index_type> next;     /* Next File Index                  */
   std::shared_ptr<unit_data> room_ptr; /* Pointer to room if is room       */

   long   filepos; /* Byte offset into file            */
   ubit32 length;  /* No of bytes to read              */
   ubit32 crc;     /* CRC check for compessed items    */

   sbit16 no_in_zone; /* Updated in zone reset for reset  */
   ubit16 no_in_mem;  /* Number of these in the game      */
   ubit16 room_no;    /* The number of the room           */
   ubit8  type;       /* Room/Obj/Char or other?          */
};

/* A linked list of commands to execute */
class zone_reset_cmd : public std::enable_shared_from_this<zone_reset_cmd>
{
public:
   ubit8 cmd_no; /* Index to array of func() ptrs */
   ubit8 cmpl;   /* Complete flag                 */

   std::shared_ptr<file_index_type> fi[2];
   sbit16                           num[3];

   std::shared_ptr<zone_reset_cmd> next;
   std::shared_ptr<zone_reset_cmd> nested;

   std::shared_ptr<zone_reset_cmd>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<zone_reset_cmd> Create() { return std::shared_ptr<zone_reset_cmd>(new zone_reset_cmd); }

private:
   zone_reset_cmd() = default;
};

/* A linked/sorted list of all the zones in the game */
class zone_type : public std::enable_shared_from_this<zone_type>
{
   zone_type(void);

public:
   ~zone_type(void);
   std::shared_ptr<zone_type>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<zone_type> Create() { return std::shared_ptr<zone_type>(new zone_type); }

   class cNamelist creators; /* List of creators of zone         */
   char           *name;     /* Unique in list                   */
   char           *title;    /* A nice looking zone title        */
   char           *notes;    /* Creator notes to zone            */
   char           *help;     /* User-Help to zone                */
   char           *filename; /* The filename of this file        */

   std::shared_ptr<file_index_type> fi; /* Pointer to list of file-index's  */
   struct bin_search_type          *ba; /* Pointer to binarray of type      */

   std::shared_ptr<zone_reset_cmd> zri;  /* List of Zone reset commands      */
   std::shared_ptr<zone_type>      next; /* Next Zone                        */

   struct diltemplate     *tmpl;   /* DIL templates in zone            */
   struct bin_search_type *tmplba; /* Pointer to binarray of type      */

   ubit8 **spmatrix; /* Shortest Path Matrix             */

   ubit16 zone_no;    /* Zone index counter (spmatrix)    */
   ubit16 no_of_fi;   /* Number of fi's in the list       */
   ubit16 zone_time;  /* How often to reset zone          */
   ubit16 no_rooms;   /* The number of rooms              */
   ubit8  reset_mode; /* when/how to reset zone           */
   ubit16 no_tmpl;    /* number of DIL templates          */

   ubit8 access;    /* Access Level 0 = highest (root)  */
   ubit8 loadlevel; /* Level required to load items     */
   ubit8 payonly;   /* TRUE when only 4 paying players  */

   struct
   {
      int pressure; /* How is the pressure ( Mb )            */
      int change;   /* How fast and what way does it change. */
      int sky;      /* How is the sky.                       */
      int base;     /* What is the basis pressure like?      */
   } weather;
};

/* ----------------- OTHER STRUCTURES ----------------------- */

/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct time_info_data
{
   sbit8  hours, day, month;
   sbit16 year;
};

/* --------------------- DESCRIPTOR STRUCTURES -------------------- */

struct snoop_data
{
   std::shared_ptr<unit_data> snooping; /* Who is this char snooping        */
   std::shared_ptr<unit_data> snoop_by; /* And who is snooping on this char */
};

class descriptor_data
{
public:
   descriptor_data(cMultiHook *pe);
   ~descriptor_data(void);

   void CreateBBS(void);
   void RemoveBBS(void);

   time_t      logon; /* Time of last connect              */
   cMultiHook *multi; /* Multi element pointer             */
   ubit16      id;    /* The ID for the multi              */
   void (*fptr)(struct descriptor_data *, char *);
   int    state;    /* Locally used in each fptr         */
   char   host[50]; /* hostname                          */
   ubit16 nPort;    /* Mplex port                        */
   ubit8  nLine;    /* Serial Line                       */
   int    wait;     /* wait for how many loops           */
   ubit16 timer;    /* num of hours idleness for mortals */
   ubit32 replyid;  /* Used for 'tell reply'             */

   /* For the 'modify-string' system.       */
   char *localstr; /* This string is expanded while editing */

   void (*postedit)(struct descriptor_data *);
   std::shared_ptr<unit_data> editing;
   void            *editref; /* pointer to "where we are editing"     */
                             /* when using (volatile) extras + boards */

   int               prompt_mode;                    /* control of prompt-printing       */
   char              last_cmd[MAX_INPUT_LENGTH + 1]; /* the last entered cmd_str         */
   char              history[MAX_INPUT_LENGTH + 1];  /* simple command history           */
   cQueue            qInput;                         /* q of unprocessed input           */
   std::shared_ptr<unit_data> character;                      /* linked to char                   */
   std::shared_ptr<unit_data> original;                       /* original char                    */
   struct snoop_data snoop;                          /* to snoop people.                 */

   class descriptor_data *next; /* link to next descriptor          */
};

/* ----------------- ROOM SPECIFIC STRUCTURES ----------------------- */

class room_direction_data : public std::enable_shared_from_this<room_direction_data>
{
   room_direction_data();

public:
   ~room_direction_data();

   std::shared_ptr<room_direction_data>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<room_direction_data> Create()
   {
      return std::shared_ptr<room_direction_data>(new room_direction_data);
   }

   class cNamelist open_name; /* For Open & Enter                  */

   std::shared_ptr<file_index_type> key;
   std::shared_ptr<unit_data> to_room;

   ubit8 exit_info; /* Door info flags                   */
};

class room_data : public std::enable_shared_from_this<room_data>
{
   room_data();

public:
   ~room_data();
   std::shared_ptr<room_data>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<room_data> Create() { return std::shared_ptr<room_data>(new room_data); }

   std::shared_ptr<room_direction_data> dir_option[6]; /* Her?? */

   ubit8 flags;         /* Room flags                              */
   ubit8 movement_type; /* The type of movement (city, hills etc.) */
   ubit8 resistance;    /* Magic resistance of the room            */
};

/* ------------------ OBJ SPECIFIC STRUCTURES ----------------------- */

class obj_data : public std::enable_shared_from_this<obj_data>
{
   obj_data();
public:
   ~obj_data();
   std::shared_ptr<obj_data>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<obj_data> Create() { return std::shared_ptr<obj_data>(new obj_data); }

   sbit32 value[5];     /* Values of the item (see list)       */
   ubit32 cost;         /* Value when sold (gp.)               */
   ubit32 cost_per_day; /* Cost to keep pr. real day           */

   ubit8 flags;      /* Various special object flags        */
   ubit8 type;       /* Type of item (ITEM_XXX)             */
   ubit8 equip_pos;  /* 0 or position of item in equipment  */
   ubit8 resistance; /* Magic resistance                    */
};

/* ------------------  PC SPECIFIC STRUCTURES ------------------------ */
struct pc_time_data
{
   time_t creation; /* This represents time when the pc was created.     */
   time_t connect;  /* This is the last time that the pc connected.      */
   time_t birth;    /* This represents the characters age                */
   ubit32 played;   /* This is the total accumulated time played in secs */
};

struct pc_account_data
{
   float  credit;       /* How many coin units are left on account?       */
   ubit32 credit_limit; /* In coin units (i.e. cents / oerer)             */
   ubit32 total_credit; /* Accumulated credit to date (coin units)        */
   sbit16 last4;        /* The last four digits of his credit card, or -1 */
   ubit8  cracks;       /* Crack-attempts on CC last4                     */
   ubit8  discount;     /* 0 - 100% discount                              */
   ubit32 flatrate;     /* The expiration date of a flat rate service     */
};

class pc_data : public std::enable_shared_from_this<pc_data>
{
   pc_data(void);

public:
   ~pc_data(void);
   std::shared_ptr<pc_data>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<pc_data> Create() { return std::shared_ptr<pc_data>(new pc_data); }

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

   ubit32 guild_time; /* When (playing secs) player entered      */
   ubit16 vlvl;       /* Virtual Level for player                */
   sbit32 id;         /* Unique identifier for each player (-1 guest) */

   sbit32 skill_points;   /* No of practice points left              */
   sbit32 ability_points; /* No of practice points left              */
   ubit16 flags;          /* flags for PC setup (brief, noshout...)  */

   ubit16 nr_of_crimes;   /* Number of crimes committed              */
   ubit16 crack_attempts; /* Number of wrong passwords entered       */
   ubit16 lifespan;       /* How many year to live....               */

   ubit8 spells[SPL_TREE_MAX];     /* The spells learned                  */
   ubit8 spell_lvl[SPL_TREE_MAX];  /* Practiced within that level         */
   sbit8 spell_cost[SPL_TREE_MAX]; /* Cost modifier                       */

   ubit8 skills[SKI_TREE_MAX];     /* The skills learned                  */
   ubit8 skill_lvl[SKI_TREE_MAX];  /* The skills practiced within level   */
   sbit8 skill_cost[SKI_TREE_MAX]; /* Cost modifier                       */

   ubit8 weapons[WPN_TREE_MAX];     /* The weapons learned                 */
   ubit8 weapon_lvl[WPN_TREE_MAX];  /* The spells learned                  */
   sbit8 weapon_cost[WPN_TREE_MAX]; /* Cost modifier                       */

   ubit8 ability_lvl[ABIL_TREE_MAX];  /* The spells learned                  */
   sbit8 ability_cost[ABIL_TREE_MAX]; /* Cost modifier                       */

   sbit8 conditions[3]; /* Drunk full etc.                     */
   ubit8 nAccessLevel;  /* Access Level for BBS use            */

   char   pwd[PC_MAX_PASSWORD];  /* Needed when loaded w/o descriptor   */
   char   filename[PC_MAX_NAME]; /* The name on disk...                 */
   ubit32 lasthosts[5];          /* last 5 different IPs                */
};

/* ------------------ NPC SPECIFIC STRUCTURES ----------------------- */

class npc_data : public std::enable_shared_from_this<npc_data>
{
   npc_data();

public:
   ~npc_data();
   std::shared_ptr<npc_data>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<npc_data> Create() { return std::shared_ptr<npc_data>(new npc_data); }

   ubit8 weapons[WPN_GROUP_MAX];
   ubit8 spells[SPL_GROUP_MAX];

   ubit8 default_pos; /* Default position for NPC               */
   ubit8 flags;       /* flags for NPC behavior                 */
};

/* ----------------- CHAR SPECIFIC STRUCTURES ----------------------- */

struct char_point_data
{
   ubit32 flags; /* Char flags                               */
   sbit32 exp;   /* The experience of the player             */

   sbit16 mana;      /* How many mana points are left?           */
   sbit16 endurance; /* How many endurance points are left?      */
   ubit16 race;      /* PC/NPC race, Humanoid, Animal, etc.     */

   sbit16 offensive; /* The OB of a character.                   */
   sbit16 defensive; /* The DB of a character.                   */

   ubit8 speed;          /* The default speed for natural combat     */
   ubit8 natural_armour; /* The natural built-in armour (ARM_)       */
   ubit8 attack_type;    /* PC/NPC Attack Type for bare hands (WPN_) */

   ubit8 dex_reduction;            /* For speed of armour calculations only    */
   ubit8 sex;                      /* PC / NPC s sex                           */
   ubit8 level;                    /* PC / NPC s level                         */
   ubit8 position;                 /* Standing, sitting, fighting...           */
   ubit8 abilities[ABIL_TREE_MAX]; /* Str/dex etc.                 */
};

struct char_follow_type
{
   std::shared_ptr<unit_data> follower; /* Must be a char */
   struct char_follow_type *next;
};

class char_data : public std::enable_shared_from_this<char_data>
{
   char_data();

public:
   ~char_data(void);

   std::shared_ptr<char_data>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<char_data> Create() { return std::shared_ptr<char_data>(new char_data); }

   struct
   {
      std::shared_ptr<pc_data>  pc;
      std::shared_ptr<npc_data> npc;
   } specific;

   char *money; /*  Money transfer from db-files.
                 *  Converted to real money when pc/npc is loaded.
                 */

   struct descriptor_data *descriptor;

   struct char_point_data points;

   class cCombat *Combat;

   struct char_follow_type *followers;
   std::shared_ptr<unit_data> master; /* Must be a char */

   std::shared_ptr<unit_data> last_room; /* Last location of character */
};

/* ----------------- UNIT GENERAL STRUCTURES ----------------------- */

struct unit_affected_type
{
   sbit16 id;
   ubit16 beat;     /* Beat in 1/4 of secs, 0 = None */
   sbit16 duration; /* How many beats until end      */

   int data[3];

   sbit16 firstf_i;
   sbit16 tickf_i;
   sbit16 lastf_i;
   sbit16 applyf_i;

   std::shared_ptr<unit_data> owner;
   struct unit_affected_type *next, *gnext, *gprevious;
};

struct unit_fptr
{
   ubit16            index;      /* Index to function pointer array             */
   ubit16            heart_beat; /* in 1/4 of a sec                             */
   ubit16            flags;      /* When to override next function (boolean)    */
   void             *data;       /* Pointer to data local for this unit         */
   struct unit_fptr *next;       /* Next in linked list                         */
};

class unit_data : public std::enable_shared_from_this<unit_data>
{
   unit_data(ubit8 type);

public:
   ~unit_data(void);

   class cNamelist                                 names; /* Name Keyword list for get, enter, etc.      */
   std::shared_ptr<unit_data>                      getptr() { return shared_from_this(); }
   [[nodiscard]] static std::shared_ptr<unit_data> Create(ubit8 type) {
      return std::shared_ptr<unit_data>(new unit_data(type));
   }

   struct
   {
      std::shared_ptr<char_data> ch;
      std::shared_ptr<room_data> room;
      std::shared_ptr<obj_data>  obj;
   } data;

   struct unit_fptr /* Function pointer type                      */
      *func;

   struct unit_affected_type *affected;

   std::shared_ptr<file_index_type> fi; /* Unit file-index                               */

   std::shared_ptr<file_index_type> key; /* Pointer to fileindex to Unit which is the key */

   std::shared_ptr<unit_data> outside;
   std::shared_ptr<unit_data> inside; /* Linked list of chars,rooms & objs             */

                                      /* For next unit in 'inside' linked list         */
   std::shared_ptr<unit_data>      next;

   /* global l-list of objects, chars & rooms       */
      std::shared_ptr<unit_data> gnext;
      std::shared_ptr<unit_data> gprevious;

   ubit32 manipulate;  /* WEAR_XXX macros                               */
   ubit16 flags;       /* Invisible, can_bury, burried...               */
   sbit16 base_weight; /* The "empty" weight of a room/char/obj         */
   sbit16 weight;      /* Current weight of a room/obj/char             */
   sbit16 capacity;    /* Capacity of obj/char/room, -1 => any          */
   ubit16 size;        /* (cm) MOBs height, weapons size, ropes length  */

   ubit8  status;     /* IS_ROOM, IS_OBJ, IS_PC, IS_NPC                */
   ubit8  open_flags; /* In general OPEN will mean can "enter"?        */
   sbit8  light;      /* Number of active light sources in unit        */
   sbit8  bright;     /* How much the unit shines                      */
   sbit8  illum;      /* how much bright is by transparency            */
   ubit8  chars;      /* How many chars is inside the unit             */
   ubit8  minv;       /* Level of wizard invisible                     */
   sbit32 max_hp;     /* The maximum number of hitpoint                */
   sbit32 hp;         /* The actual amount of hitpoints left           */

   sbit16 alignment; /* +-1000 for alignments                         */

   /* MS2020 Cleaned up the swap stuff */

   /* Room title, Char title, Obj "the barrel", NPC "the Beastly Fido" */
   class cStringInstance title;

   /* The outside description of a unit           */
   class cStringInstance out_descr;

   /* The inside description of a unit            */
   class cStringInstance in_descr;

   class extra_descr_data /* All the look 'at' stuff                     */
      *extra_descr;
};

/* ----------------- Destructed decalrations ----------------------- */

#define DR_UNIT   0
#define DR_AFFECT 1
#define DR_FUNC   2

int is_destructed(int i, void *ptr);
int is_destructed(int i, std::shared_ptr<unit_data> ptr);

#endif /* _MUD_STRUCTS_H */
