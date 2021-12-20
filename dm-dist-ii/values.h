#pragma once

#include <cstddef>

#define ADMIN_MAIL "mud@valhalla-usa.com"

enum Level
{
   START_LEVEL         = 1,
   MORTAL_MAX_LEVEL    = 50,  /* Maximum level for mortal players */
   IMMORTAL_LEVEL      = 200, /* At 200 you can't really die      */
   GOD_LEVEL           = 220, /* Here are the *real* gods         */
   CREATOR_LEVEL       = 230,
   OVERSEER_LEVEL      = 253,
   ADMINISTRATOR_LEVEL = 254,
   ULTIMATE_LEVEL      = 255,
};

#define POLICE_ACADEMY    "$Police Member"
#define POLICE_BANNED     "$Police Banned"
#define GUILD_UDG_FIGHTER "Udgaard Fighter"
#define GUILD_UDG_THIEF   "Udgaard Thief"
#define GUILD_UDG_CLERIC  "Udgaard Healer"
#define GUILD_UDG_MAGE    "Udgaard Conjurer"
#define GUILD_PALADIN     "Midgaard Paladin"
#define GUILD_SORCERER    "Midgaard Sorcerer"
#define GUILD_ASSASSIN    "Khorsabad Assassin"
#define GUILD_MYSTIC      "Midgaard Mystic"
#define GUILD_NECROMANCER "Necromancer"

#define QUEST_WW              "Wight warrens quest"
#define QUEST_WELMAR          "King Welmar Quest"
#define QUEST_RABBIT_ONGOING  "Rabbit Stew Ongoing"
#define QUEST_RABBIT_COMPLETE "Rabbit Stew Complete"
#define QUEST_POT_COMPLETE    "Mary's Pot Retrieved"

#define A_HIDEINV 10
#define A_SOMEONE 11
#define A_ALWAYS  12

enum ToDirection
{
   TO_ROOM    = 0,
   TO_VICT    = 1,
   TO_NOTVICT = 2,
   TO_CHAR    = 3,
   TO_ALL     = 4,
   TO_REST    = 5,
};

#define FIND_UNIT_EQUIP         (0x0001)
#define FIND_UNIT_INVEN         (0x0002)
#define FIND_UNIT_SURRO         (0x0004)
#define FIND_UNIT_ZONE          (0x0008)
#define FIND_UNIT_WORLD         (0x0010)
#define FIND_UNIT_LOCATION_MASK (FIND_UNIT_EQUIP | FIND_UNIT_INVEN | FIND_UNIT_SURRO | FIND_UNIT_ZONE | FIND_UNIT_WORLD)

#define FIND_UNIT_PAY   (0x0020) /* Only find units in pay zones */
#define FIND_UNIT_NOPAY (0x0040) /* Only find units in nopay zones */

#define FIND_UNIT_IN_ME  (FIND_UNIT_EQUIP + FIND_UNIT_INVEN)
#define FIND_UNIT_HERE   (FIND_UNIT_IN_ME + FIND_UNIT_SURRO)
#define FIND_UNIT_GLOBAL (FIND_UNIT_HERE + FIND_UNIT_ZONE + FIND_UNIT_WORLD)

#define FIND_ROOM (UNIT_ST_ROOM)
#define FIND_OBJ  (UNIT_ST_OBJ)
#define FIND_PC   (UNIT_ST_PC)
#define FIND_NPC  (UNIT_ST_NPC)

#define FIND_CHAR    (FIND_PC + FIND_NPC)
#define FIND_NONROOM (FIND_OBJ + FIND_CHAR)
#define FIND_UNIT    (FIND_NONROOM + FIND_ROOM)

#define PULSE_SEC      4 /* Each pulse is 1/4 of a second  */
#define WAIT_SEC       4
#define PULSE_ZONE     (60 * PULSE_SEC)
#define PULSE_MOBILE   (10 * PULSE_SEC)
#define PULSE_VIOLENCE (3 * PULSE_SEC)
#define PULSE_POINTS   (60 * PULSE_SEC) /* Pointupdate every 60 secs */
#define PULSE_ROUND    (1 * PULSE_SEC)

constexpr int SECS_PER_REAL_MIN  = 60;
constexpr int SECS_PER_REAL_HOUR = (60 * SECS_PER_REAL_MIN);
constexpr int SECS_PER_REAL_DAY  = (24 * SECS_PER_REAL_HOUR);
constexpr int SECS_PER_REAL_YEAR = (365 * SECS_PER_REAL_DAY);

constexpr int MUD_DAY            = 24; /* Twentyfour hours per day    */
constexpr int MUD_WEEK           = 7;  /* Seven mud days per mud week */
constexpr int MUD_MONTH          = 14; /* 21 mud days per mud month   */
constexpr int MUD_YEAR           = 9;  /* 9 mud months per mud year   */

constexpr int SECS_PER_MUD_HOUR  = (5 * SECS_PER_REAL_MIN);
constexpr int SECS_PER_MUD_DAY   = (24 * SECS_PER_MUD_HOUR);
constexpr int SECS_PER_MUD_MONTH = (MUD_MONTH * SECS_PER_MUD_DAY);
constexpr int SECS_PER_MUD_YEAR  = (MUD_YEAR * SECS_PER_MUD_MONTH);

/* These number must be unique AND at the same time it represents */
/* how much each crime is weighted, thus it is four times as bad  */
/* to murder as it is to steal                                    */
#define CRIME_EXTRA    2 /* Extra witness to same crime */
#define CRIME_STEALING 3
#define CRIME_MURDER   8
#define CRIME_PK       64 /* Murder on player */

/* How much light is in the land ? */
enum LightInLand
{
   SUN_DARK  = 0,
   SUN_RISE  = 1,
   SUN_LIGHT = 2,
   SUN_SET   = 3,
};

/* And how is the sky ? */
enum SkyConditions
{
   SKY_CLOUDLESS = 0,
   SKY_CLOUDY    = 1,
   SKY_RAINING   = 2,
   SKY_LIGHTNING = 3,
};
/* ------------------------- Directions ------------------------------
 NORTH, EAST, SOUTH, WEST, UP and DOWN
   The macros to use in exits.
   For example:
      exit[NORTH] to temple;
   We recommend that you use the shorthand direction macros. For example:
      north to temple;
*/
enum Direction
{
   NORTH = 0,
   EAST  = 1,
   SOUTH = 2,
   WEST  = 3,
   UP    = 4,
   DOWN  = 5,
};

/* Directions for use in 'pathto' */
enum PathTo
{
   DIR_NORTH      = NORTH,
   DIR_SOUTH      = SOUTH,
   DIR_WEST       = WEST,
   DIR_UP         = UP,
   DIR_DOWN       = DOWN,
   DIR_ENTER      = 6,
   DIR_EXIT       = 7,
   DIR_IMPOSSIBLE = 8,
   DIR_HERE       = 9,
};
/* ---------------------- UNIT DEFINITIONS ------------------------- */
/* These are only flags for the purpose of quick testing, a unit     */
/* must NEVER assume more than one of these flags.                   */

enum UnitDefinitions
{
   UNIT_ST_NPC  = 0x01, /* Is this a NPC?                  */
   UNIT_ST_PC   = 0x02, /* Is this a PC?                   */
   UNIT_ST_ROOM = 0x04, /* Is this a ROOM?                 */
   UNIT_ST_OBJ  = 0x08, /* Is this a OBJ?                  */
};
/* ------------------ PC DEFINITIONS ----------------------- */

/* Predifined conditions for PC's only */
enum PCCondition
{
   DRUNK  = 0,
   FULL   = 1,
   THIRST = 2,
};
/* For PC flags */
enum PCFlags
{
   PC_BRIEF      = 0x0001, /* Brief descriptions?                     */
   PC_PK_RELAXED = 0x0002, /* Relaxed PK mode?                        */
   PC_INFORM     = 0x0004, /* Get `Nnn has entered the world.'?       */
   PC_COMPACT    = 0x0008, /* Compact Display Bit                     */
   PC_NOWIZ      = 0x0010, /* Can wiz? Can hear wiz?                  */
   PC_SPIRIT     = 0x0020, /* When player is dead                     */
   PC_EXPERT     = 0x0040, /* When set, get details about point system*/
   PC_PROMPT     = 0x0080, /* Used with prompt on/off                 */
   PC_ECHO       = 0x0200, /* Echo communication commands to pc?      */
   PC_EXITS      = 0x0400, /* Display exits                           */
   PC_NOSHOUTING = 0x1000, /* Can shout? (usually wiz punishment)     */
   PC_NOTELLING  = 0x2000, /* Can tell? (usually wiz punishment)      */
   PC_NOSHOUT    = 0x4000, /* Can hear shouts?                        */
   PC_NOTELL     = 0x8000, /* Can hear tells?                         */
};
/* ------------------------- Zone Reset modes ------------------------------
 RESET_NOT
   Zone is never reset, except at boot time.

 RESET_IFEMPTY
   A zone is only reset if no players are present in the zone.

 RESET_ANYHOW
   A zone is reset regardless of wheter it contains players or not.
*/
enum ZoneReset
{
   RESET_NOT     = 0,
   RESET_IFEMPTY = 1,
   RESET_ANYHOW  = 2,
};
/* ------------------------- Sector Types ------------------------------
 Common for all sector types are that when you specify one for your
 location, it does not mean that your location is that type, but
 rather that moving inside your location is like moving in an area
 similar to the one you specify. For example:

   movement SECT_CITY

 Naturally this should be used in a city, but it might be used in a
 maze if you judge that moving about in your maze is similar to
 walking in a city.


 SECT_INSIDE
   This is used when moving is like moving around inside (ie. no
   weather etc.)

 SECT_CITY
   This is used when moving is like moving in a city.

 SECT_FIELD
   This is used when moving is like moving in a field.

 SECT_FOREST
   This is used when moving is like moving in a forest (ie. soft ground
   and obstacles (trees)).

 SECT_HILLS
   This is used when moving is like moving in hills (ie. up and down hill
   movement).

 SECT_MOUNTAIN
   This is used when moving is like moving in mountains (ie. steep up and
   downhills, dangerous passages, loose rocks etc)

 SECT_DESERT
   This is used when moving is like moving in a desert (ie. very heavy
   to move feet in sand, like walking on a beach with loose sand)

 SECT_SWAMP
   This is used when moving is like moving in a swamp (ie. your feet
   are stuck each time you take a step)

 SECT_WATER_SWIM
   This is used when moving is like swimming in water. When the movement
   type is water, things dropped will sink to the bottom. Diving is
   only possible in movement types of type water to types of underwater.

 SECT_WATER_SAIL
   This is used to specify that the room is water, but you can only enter
   if in a boat or similar device. Actually this is meant to be used when
   you want to make a ocean "room" or similar rooms. Swimming is possible
   but probably fatal - an ocean should thus be at least two rooms wide.

 SECT_UNDER_WATER
   This is used when underwater (i.e. breathing isn't possible).

*/

enum SectorType
{
   SECT_INSIDE      = 0,
   SECT_CITY        = 1,
   SECT_FIELD       = 2,
   SECT_FOREST      = 3,
   SECT_HILLS       = 4,
   SECT_MOUNTAIN    = 5,
   SECT_DESERT      = 6,
   SECT_SWAMP       = 7,
   SECT_WATER_SWIM  = 8,
   SECT_WATER_SAIL  = 9,
   SECT_UNDER_WATER = 10,
   SECT_SNOW        = 11,
   SECT_SLUSH       = 12, /* Wet snow */
   SECT_ICE         = 13,
};
/* ------------------------- NPC Flags ------------------------------
 INFINITE_CAPACITY
   Set capacity to this if you want it to be infinitely big
*/

#define INFINITE_CAPACITY -1

/* ------------------------- NPC Flags ------------------------------

 NPC_NICE_THIEF
   The mobile will treat characters that attempt to steal from it non-
   violently.

*/
#define NPC_NICE_THIEF 0x0008

/* ------------------------- UNIT Flags ------------------------------
 UNIT_FL_PRIVATE
   Set this flag if the unit is a wizard unit. For rooms this implies
   private conversations, ei. max two characters can goto a particular
   room.

 UNIT_FL_INVISIBLE
   Set this flag if the unit is invisible.

 UNIT_FL_NO_BURY
   Set this flag if units can NOT be buried inside this unit.
   I.e. set this flag in rooms like a canoe, or a house.

 UNIT_FL_BURIED
   Set this flag if the unit is buried inside whatever it is loaded
   into.

 UNIT_FL_NO_TELEPORT
   Set this flag if no teleport to/from/with this unit is allowed.

 UNIT_FL_NO_MOB
   Set this flag in a unit, if mobiles are not allowed to enter it.

 UNIT_FL_NO_WEATHER
   Set this flag in a unit if no weather information shall be passed
   to the players inside the unit. When set it also indicates that no
   natural light, lightning, rain etc. is available in the unit.

 UNIT_FL_INDOORS
   Set this bit if the unit is "indoors" ie. it has walls/ceiling
   (living room/cave/dungeon etc). This bit does not affect the
   weather flag, but might have an impact on spells such as
   earthquake :)

 UNIT_FL_TRANS
   Set this flag, if the unit is to be transperant, as f.inst.
   a riding horse, boat, or other object, where you can
   communicate with/see characters outside.

 UNIT_FL_NOSAVE
   If this flag is set the unit is not allowed to be saved as
   inventory. This is useful for the safe key etc.
   If a player is (recursively) inside a room with this flag,
   he won't be loaded into this location when reentering the game.
   This is useful for treasure rooms, dangerous rooms, etc.

 UNIT_FL_SACRED
    When set, all characters in the room will gain hit-points, mana-
   points and endurance points at double rate.


 UNIT_FL_MAGIC
   When this flag is set the unit is radiating an aura of magic when
   it is examined.
*/

enum UnitFlags
{
   UNIT_FL_PRIVATE     = 0x0001,
   UNIT_FL_INVISIBLE   = 0x0002,
   UNIT_FL_NO_BURY     = 0x0004,
   UNIT_FL_BURIED      = 0x0008,
   UNIT_FL_NO_TELEPORT = 0x0010,
   UNIT_FL_SACRED      = 0x0020,
   UNIT_FL_NO_MOB      = 0x0040,
   UNIT_FL_NO_WEATHER  = 0x0080,
   UNIT_FL_INDOORS     = 0x0100,
   UNIT_FL_TRANS       = 0x0400,
   UNIT_FL_XXXUNUSED2  = 0x0800,
   UNIT_FL_NOSAVE      = 0x1000,
   UNIT_FL_MAGIC       = 0x2000,
};
/* ------------------------ Positions ------------------------------

 POSITION_DEAD
   Do not use this position. Make a corpse instead.

 POSITION_MORTALLYW

 POSITION_INCAP

 POSITION_STUNNED
   A stunned monster will quickly recover to position standing, no need
   to use this position.

 POSITION_SLEEPING

 POSITION_RESTING

 POSITION_SITTING

 POSITION_FIGHTING
   Do not use this position.

 POSITION_STANDING
*/
enum Position
{
   POSITION_DEAD      = 0,
   POSITION_MORTALLYW = 1,
   POSITION_INCAP     = 2,
   POSITION_STUNNED   = 3,
   POSITION_SLEEPING  = 4,
   POSITION_RESTING   = 5,
   POSITION_SITTING   = 6,
   POSITION_FIGHTING  = 7,
   POSITION_STANDING  = 8,
};
/* ------------------------- MANIPULATE Flags ------------------------------
 MANIPULATE_TAKE
   Set this flag if the unit can be taken (picked up/moved about).

 MANIPULATE_ENTER
   Set this flag if it is possible to enter a unit, ie set it in a
   coffin if you want players to be able to enter the coffin.

 These flags are set to indicate on what body positions a particular
 object can be worn:

 MANIPULATE_WEAR_FINGER
 MANIPULATE_WEAR_NECK
 MANIPULATE_WEAR_BODY
 MANIPULATE_WEAR_HEAD
 MANIPULATE_WEAR_LEGS
 MANIPULATE_WEAR_FEET
 MANIPULATE_WEAR_HANDS
 MANIPULATE_WEAR_ARMS
 MANIPULATE_WEAR_SHIELD
 MANIPULATE_WEAR_ABOUT
   Cloak, robes etc.
 MANIPULATE_WEAR_WAIST
 MANIPULATE_WEAR_WRIST
 MANIPULATE_WIELD
 MANIPULATE_HOLD
   Torches etc.

 MANIPULATE_WEAR_EAR
   Earrings...

 MANIPULATE_WEAR_BACK
   Backpacks, quivers, shields...

 MANIPULATE_WEAR_CHEST
   Across chest, Baldric

 MANIPULATE_WEAR_ANKLE
   Anklets
*/
enum ManipulateFlags
{
   MANIPULATE_TAKE        = 0x00001,
   MANIPULATE_WEAR_FINGER = 0x00002,
   MANIPULATE_WEAR_NECK   = 0x00004,
   MANIPULATE_WEAR_BODY   = 0x00008,
   MANIPULATE_WEAR_HEAD   = 0x00010,
   MANIPULATE_WEAR_LEGS   = 0x00020,
   MANIPULATE_WEAR_FEET   = 0x00040,
   MANIPULATE_WEAR_HANDS  = 0x00080,
   MANIPULATE_WEAR_ARMS   = 0x00100,
   MANIPULATE_WEAR_SHIELD = 0x00200,
   MANIPULATE_WEAR_ABOUT  = 0x00400,
   MANIPULATE_WEAR_WAIST  = 0x00800,
   MANIPULATE_WEAR_WRIST  = 0x01000,
   MANIPULATE_WIELD       = 0x02000,
   MANIPULATE_HOLD        = 0x04000,
   MANIPULATE_ENTER       = 0x08000,
   MANIPULATE_WEAR_EAR    = 0x10000,
   MANIPULATE_WEAR_BACK   = 0x20000,
   MANIPULATE_WEAR_CHEST  = 0x40000,
   MANIPULATE_WEAR_ANKLE  = 0x80000,
};
/* ------------------------- CHAR Flags ------------------------------
 CHAR_PROTECTED
   Set this flag if the character is protected by the law-system.

 CHAR_LEGAL_TARGET
   Do not use.

 CHAR_OUTLAW
   Do not use.

 CHAR_GROUP
   Do not use.

 CHAR_BLIND
   Set flag if character is blinded.

 CHAR_HIDE
   Set flag if character is hidden.

 CHAR_MUTE
   Set flag if character is mute.

 CHAR_SNEAK
   Set flag if character is in sneaking mode.

 CHAR_DETECT_ALIGN
   No actual effect on NPC's.

 CHAR_DETECT_INVISIBLE
   Set flag if character can see invisible units.

 CHAR_DETECT_MAGIC
   No actual effect on NPC's.

 CHAR_DETECT_POISON
   No actual effect on NPC's.

 CHAR_DETECT_UNDEAD
   No actual effect on NPC's.

 CHAR_DETECT_CURSE
   No actual effect on NPC's.

 CHAR_DETECT_LIFE
   No actual effect on NPC's.

 CHAR_WIMPY
   Set flag if character if wimpy. Wimpy characters flee when they are
   low on hitpoints, and they gain less experience when killing others.
   If a character is both wimpy and aggressive (NPC_AGGRESSIVE) it will
   only attack sleeping players.

 CHAR_SELF_DEFENCE
   Do not use - Internal.

*/
enum CharacterFlags
{
   CHAR_PROTECTED        = 0x00001,
   CHAR_LEGAL_TARGET     = 0x00002,
   CHAR_OUTLAW           = 0x00004,
   CHAR_GROUP            = 0x00008,
   CHAR_BLIND            = 0x00010,
   CHAR_HIDE             = 0x00020,
   CHAR_MUTE             = 0x00040,
   CHAR_SNEAK            = 0x00080,
   CHAR_DETECT_ALIGN     = 0x00100,
   CHAR_DETECT_INVISIBLE = 0x00200,
   CHAR_DETECT_MAGIC     = 0x00400,
   CHAR_DETECT_POISON    = 0x00800,
   CHAR_DETECT_UNDEAD    = 0x01000,
   CHAR_DETECT_CURSE     = 0x02000,
   CHAR_DETECT_LIFE      = 0x04000,
   CHAR_WIMPY            = 0x08000,
   CHAR_SELF_DEFENCE     = 0x20000,
   CHAR_PEACEFUL         = 0x40000, /* Auto-attack stuff? */
};
/* ------------------------- ITEM Types ------------------------------
 ITEM_LIGHT
   Items of this type can be lighted and extinguished.
   value[0] How many hours of burning left, 0 if
            burnt out, -1 if eternal.
   value[1] Number of light sources when burning
            (normally >= 1, never > 10. 1 = torch)

 ITEM_SCROLL
 ITEM_WAND
 ITEM_STAFF
 ITEM_WEAPON
 ITEM_FIREWEAPON
 ITEM_MISSILE
 ITEM_TREASURE
 ITEM_ARMOR
 ITEM_POTION
 ITEM_WORN
 ITEM_OTHER
 ITEM_TRASH
 ITEM_TRAP
 ITEM_CONTAINER
 ITEM_NOTE
 ITEM_DRINKCON
   Value[0] Max-contains
   Value[1] Contains
   Value[2] Liquid Type (LIQ_XXX)
   Value[3] Poison Factor
   Value[4] Unused

   Warning. Setting the value [1] without adjusting the weight of the
   unit will cause a series of weight errors in the environment.

 ITEM_KEY
 ITEM_FOOD
 ITEM_MONEY
 ITEM_PEN
 ITEM_BOAT
 ITEM_SPELL
   A page in a spell book
 ITEM_BOOK
   Spell book
 ITEM_SHIELD
*/
enum ItemTypes
{
   ITEM_LIGHT      = 1,
   ITEM_SCROLL     = 2,
   ITEM_WAND       = 3,
   ITEM_STAFF      = 4,
   ITEM_WEAPON     = 5,
   ITEM_FIREWEAPON = 6,
   ITEM_MISSILE    = 7,
   ITEM_TREASURE   = 8,
   ITEM_ARMOR      = 9,
   ITEM_POTION     = 10,
   ITEM_WORN       = 11,
   ITEM_OTHER      = 12,
   ITEM_TRASH      = 13,
   ITEM_TRAP       = 14,
   ITEM_CONTAINER  = 15,
   ITEM_NOTE       = 16,
   ITEM_DRINKCON   = 17,
   ITEM_KEY        = 18,
   ITEM_FOOD       = 19,
   ITEM_MONEY      = 20,
   ITEM_PEN        = 21,
   ITEM_BOAT       = 22,
   ITEM_SPELL      = 23,
   ITEM_BOOK       = 24,
   ITEM_SHIELD     = 25,
   ITEM_SKIN       = 26,
};
/* ------------------------- ITEM Types ------------------------------
 WEAR_UNUSED
   Do not use.
 WEAR_FINGER_R
 WEAR_FINGER_L
 WEAR_NECK_1
 WEAR_NECK_2
 WEAR_BODY
 WEAR_HEAD
 WEAR_LEGS
 WEAR_FEET
 WEAR_HANDS
 WEAR_ARMS
 WEAR_SHIELD
 WEAR_ABOUT
 WEAR_WAIST
 WEAR_WRIST_R
 WEAR_WRIST_L
 WEAR_WIELD
 WEAR_HOLD
 WEAR_MAX
   Do not use - internal value.
*/
enum WearTypes
{
   WEAR_UNUSED   = 1,
   WEAR_FINGER_R = 2,
   WEAR_FINGER_L = 3,
   WEAR_NECK_1   = 4,
   WEAR_NECK_2   = 5,
   WEAR_BODY     = 6,
   WEAR_HEAD     = 7,
   WEAR_LEGS     = 8,
   WEAR_FEET     = 9,
   WEAR_HANDS    = 10,
   WEAR_ARMS     = 11,
   WEAR_SHIELD   = 12,
   WEAR_ABOUT    = 13,
   WEAR_WAIST    = 14,
   WEAR_WRIST_R  = 15,
   WEAR_WRIST_L  = 16,
   WEAR_WIELD    = 17,
   WEAR_HOLD     = 18,
   WEAR_CHEST    = 19,
   WEAR_BACK     = 20,
   WEAR_EAR_L    = 21,
   WEAR_EAR_R    = 22,
   WEAR_ANKLE_L  = 23,
   WEAR_ANKLE_R  = 24,
   WEAR_MAX      = 24, /* Top of "table" */
};
/* ------------------------- Sex Types ------------------------------
 Obivious, aren't they?
 SEX_NEUTRAL
 SEX_MALE
 SEX_FEMALE
*/
enum SexType
{
   SEX_NEUTRAL = 0,
   SEX_MALE    = 1,
   SEX_FEMALE  = 2,
};
/* ------------------------- Lock Flags ------------------------------
 Use these values in door reset commands, or use with "open" flags in
 units.

 EX_OPEN_CLOSE
   Set flag if unit (container)/door can be open and closed.

 EX_CLOSED
   Set flag if the unit(container)/door is closed.

 EX_LOCKED
   Set flag if the unit(container)/door is locked.

 EX_PICKPROOF
   Set flag if the unit(container)/door is pick proof.

 EX_INSIDE_OPEN
   Set if the unit(container only) can be opened and locked from the inside.
   For example a coffin could be opened from both sides, whereas a closet
   perhaps only can be opened from the outside.

 EX_HIDDEN
   When bit is set the exit is only revealed by a command to 'search' in
   a specific direction.
*/
enum LockFlag
{
   EX_OPEN_CLOSE  = 0x01,
   EX_CLOSED      = 0x02,
   EX_LOCKED      = 0x04,
   EX_PICKPROOF   = 0x08,
   EX_INSIDE_OPEN = 0x10,
   EX_HIDDEN      = 0x20,
};
/* ------------------------- Room Flags ------------------------------
 Use the room flags with "romflags" when defining a room.

*/

/* ------------------------- Object Flags ------------------------------
 Use the object flags with "romflags" when defining an object.

 OBJ_NO_UNEQUIP
   When set, the object can not be un-equipped.

 OBJ_TWO_HANDS
   Used to indicate if an object requires two hands to use. E.g. if set
   on a long sword, then it can not be used in any dual-wield combination,
   or if set on a wand, then no weapon can be wielded simultaneously.

 OBJ_NOCOVER
   Used to indicate that as equipment, this object does not cover
   the bodypart.

 OBJ_NO_DUAL
   Used on weapons to indicate that it can not be used as a secondary
   weapon in a dual-wield.
*/
enum ObjectFlag
{
   OBJ_NO_UNEQUIP = 0x0001,
   OBJ_TWO_HANDS  = 0x0002,
   OBJ_NOCOVER    = 0x0004,
   OBJ_NO_DUAL    = 0x0008,
};
/* ------------------------- Money Values -------------------------------
 */

#define ANY_CURRENCY -1 /* For internal use only. */
#define DEF_CURRENCY 0

#define MAX_CURRENCY 0

#define IRON_PIECE     "ip" /* 0 */
#define COPPER_PIECE   "cp" /* 1 */
#define SILVER_PIECE   "sp" /* 2 */
#define GOLD_PIECE     "gp" /* 3 */
#define PLATINUM_PIECE "pp" /* 4 */

#define MAX_MONEY 4 /* This is the number of strings above - 1 */

/* Remember to change these if you change in the money file!! */
enum CoinMuliplier
{
   IRON_MULT     = 10,
   COPPER_MULT   = (8 * IRON_MULT),
   SILVER_MULT   = (8 * COPPER_MULT),
   GOLD_MULT     = (8 * SILVER_MULT),
   PLATINUM_MULT = (8 * GOLD_MULT),
};
/* ------------------------- Liquid Values ------------------------------
 When defining a liquid container, this specifies the liquid types to
 put into the apropriate value[]
*/
enum LiquidType
{
   LIQ_WATER     = 0,
   LIQ_BEER      = 1,
   LIQ_WINE      = 2,
   LIQ_ALE       = 3,
   LIQ_DARKALE   = 4,
   LIQ_WHISKY    = 5,
   LIQ_LEMONADE  = 6,
   LIQ_FIREBRT   = 7,
   LIQ_LOCALSPC  = 8,
   LIQ_SLIME     = 9,
   LIQ_MILK      = 10,
   LIQ_TEA       = 11,
   LIQ_COFFE     = 12,
   LIQ_BLOOD     = 13,
   LIQ_SALTWATER = 14,
   LIQ_COKE      = 15,
   LIQ_VODKA     = 16,
   LIQ_BRANDY    = 17,
   LIQ_MAX       = 17,
};
/* If your can never empty it */
constexpr int INFINITE_LIQUID   = -1;

/* ------------------------- Light Values  ------------------------------
 INFINITE_BURNTIME
 Set this if the light source never burns out.
*/
constexpr int INFINITE_BURNTIME = -1;

/* ------------------------- Speed  ------------------------------
   The combat speed is 12 by default and is lower when faster.
   4 is the fastest speed and means 3 attacks per combat round.
   200 is slowest and means 1 attack per 200/12 combat rounds.

   SPEED_DEFAULT is also the same as the duration of one combat round.
*/

enum Speed
{
   SPEED_DEFAULT = 12,
   SPEED_MIN     = 4,
   SPEED_MAX     = 200,
};
/* ------------------------- Damage Groups  ------------------------------
    These are the possible damage groups... For the weapon group, the
    attack number is one of WPN_XXX, for the spell group attack number
    is one of SPL_XXX, for the skill group one of SKI_XXX and finally
    for the _OTHER group, one of MSG_OTHER_XXX
*/

enum DamageGroup
{
   MSG_TYPE_WEAPON = 0,
   MSG_TYPE_SPELL  = 1,
   MSG_TYPE_SKILL  = 2,
   MSG_TYPE_OTHER  = 3,
};
/* ------------------------- Damage Groups  ------------------------------
    These are the damage identifiers fro the MSG_TYPE_OTHER damage group,
    as shown above.
*/

#define MSG_OTHER_BLEEDING   1 /* Messages when bleeding on ground */
#define MSG_OTHER_POISON     2 /* Messages when poison suffering   */
#define MSG_OTHER_STARVATION 3 /* Messages when starving/thirsting */

/* ------------------------- Armour Types  ------------------------------
   ARM_XXX is the armour type. Use the definied macros instead of these
           values when defining armours.
*/
enum ArmorType
{
   ARM_CLOTHES  = 0, /* Naked / clothes is only protection    */
   ARM_LEATHER  = 1, /* A soft flexible leather base armour   */
   ARM_HLEATHER = 2, /* A hard unflexible leather base armour */
   ARM_CHAIN    = 3, /* A flexible armour composed of interlocking rings */
   ARM_PLATE    = 4, /* An unflexible plate armour. */
};

/* ------------------------- Monster Size  ------------------------------
   SIZ_XXX is the size of the monster. This has effect on the maximum
           damage it can give with NATURAL attacks (hand, bite, etc).
           The size is dependant on WEIGHT only.
*/
enum MonsterSize
{
   SIZ_TINY   = 0, /*   0 -   5 lbs */
   SIZ_SMALL  = 1, /*  11 -  40 lbs */
   SIZ_MEDIUM = 2, /*  41 - 160 lbs */
   SIZ_LARGE  = 3, /* 161 - 500 lbs */
   SIZ_HUGE   = 4, /* 500+ lbs      */
};
/* ------------------------- Skill Values  ------------------------------
   ABIL_XXX refers to the abilitity. Use 'ability[ABIL_XXX] = yyy' in
            the zone, or use predefined macros.
*/
enum SkillType
{
   ABIL_MAG      = 0,
   ABIL_DIV      = 1,
   ABIL_STR      = 2,
   ABIL_DEX      = 3,
   ABIL_CON      = 4,
   ABIL_CHA      = 5,
   ABIL_BRA      = 6,
   ABIL_HP       = 7,
   ABIL_TREE_MAX = 8, /* Do not use */
};
/* All the nodes in the weapon tree

   WPN_XXX refers to the weapon skills, attack types, and weapon types.
           For the skill, you should only use the 'group' weapons (0..6),
           or use the predefined macros
*/

enum WeaponType
{
   WPN_ROOT         = 0, /* Root of weapon tree        */
   WPN_AXE_HAM      = 1, /* Axe/hammer category        */
   WPN_SWORD        = 2, /* Sword Category             */
   WPN_CLUB_MACE    = 3, /* Club/Mace/Flail Category   */
   WPN_POLEARM      = 4, /* Polearm category           */
   WPN_UNARMED      = 5, /* Natural attacks            */
   WPN_SPECIAL      = 6, /* Special weapons            */
   WPN_GROUP_MAX    = 7, /* #[0..6] of groups          */

   WPN_BATTLE_AXE   = 7, /* Two Handed */
   WPN_HAND_AXE     = 8,
   WPN_WAR_MATTOCK  = 9, /* Two Handed */
   WPN_WAR_HAMMER   = 10,

   WPN_GREAT_SWORD  = 11, /* Two Handed */
   WPN_SCIMITAR     = 12,
   WPN_KATANA       = 13,
   WPN_FALCHION     = 14,
   WPN_KOPESH       = 15,
   WPN_BROAD_SWORD  = 16,
   WPN_LONG_SWORD   = 17,
   WPN_RAPIER       = 18,
   WPN_SHORT_SWORD  = 19,
   WPN_DAGGER       = 20,

   WPN_BATTLE_MACE  = 21, /* Two Handed */
   WPN_MACE         = 22,
   WPN_BATTLE_CLUB  = 23, /* Two handed */
   WPN_CLUB         = 24,
   WPN_MORNING_STAR = 25,
   WPN_FLAIL        = 26,

   WPN_QUARTERSTAFF = 27,
   WPN_SPEAR        = 28,
   WPN_HALBERD      = 29,
   WPN_BARDICHE     = 30,
   WPN_SICKLE       = 31,
   WPN_SCYTHE       = 32, /* Two handed */
   WPN_TRIDENT      = 33,

   WPN_FIST         = 34,
   WPN_KICK         = 35,
   WPN_BITE         = 36,
   WPN_STING        = 37,
   WPN_CLAW         = 38,
   WPN_CRUSH        = 39,

   WPN_WHIP         = 40,
   WPN_WAKIZASHI    = 41,
   WPN_BOW          = 42,
   WPN_THROW        = 43,

   WPN_BLANK_0      = 44,
   WPN_BLANK_1      = 45,
   WPN_BLANK_2      = 46,
   WPN_BLANK_3      = 47,
   WPN_BLANK_4      = 48,
   /* Insert any new definitions here... */
   WPN_TREE_MAX     = 60, /* Top of weapon list  #[=0,..=48,] = =49, */
};

/* For use with armours and shields, in the 'value[X]' */

enum ShieldSize
{
   SHIELD_SMALL  = 0, /* Small Shield */
   SHIELD_MEDIUM = 1, /* Medium Shield */
   SHIELD_LARGE  = 2, /* Large Shield */
};
/* Shield methods set for each weapon type & spell type */
enum ShieldMethod
{
   SHIELD_M_BLOCK   = 0, /* Can completely block the attack              */
   SHIELD_M_REDUCE  = 1, /* Can partially reduce the damage of an attack */
   SHIELD_M_USELESS = 2, /* Can not help at all against an attack        */
};
/*
   Races. If the race you are seeking isn't here, mail to seifert@diku.dk
   and request a name, and a number. This will aid compatibility between
   different versions of Valhalla
*/

enum Race
{
   RACE_HUMAN           = 0, /* PC race */
   RACE_ELF             = 1, /* PC race */
   RACE_DWARF           = 2, /* PC race */
   RACE_HALFLING        = 3, /* PC race */
   RACE_GNOME           = 4, /* PC race */
   RACE_HALF_ORC        = 5,
   RACE_HALF_OGRE       = 6,
   RACE_HALF_ELF        = 7,
   RACE_BROWNIE         = 8,
   RACE_GROLL           = 9,
   RACE_DARK_ELF        = 10,

   RACE_BLANK0          = 11, /* PC race unused */
   RACE_BLANK1          = 12, /* PC race unused */
   RACE_BLANK2          = 13, /* PC race unused */
   RACE_BLANK3          = 14, /* PC race unused */
   RACE_BLANK4          = 15, /* PC race unused */
   RACE_BLANK5          = 16, /* PC race unused */
   RACE_BLANK6          = 17, /* PC race unused */
   RACE_BLANK7          = 18, /* PC race unused */
   RACE_BLANK8          = 19, /* PC race unused */
   RACE_BLANK9          = 20, /* PC race unused */

   PC_RACE_MAX          = 21, /* There are=21, player races [0..20] */

   RACE_SKAVEN          = 120,
   RACE_GNOLL           = 121,
   RACE_GOBLIN          = 122,
   RACE_HOBGOBLIN       = 123,
   RACE_KOBOLD          = 124,
   RACE_NIXIE           = 125,
   RACE_NYMPH           = 126,
   RACE_OGRE            = 127,
   RACE_ORC             = 128,
   RACE_SATYR           = 129,
   RACE_FAUN            = 130,
   RACE_SPRITE          = 131,
   RACE_DRYAD           = 132,
   RACE_LEPRECHAUN      = 133,
   RACE_PIXIE           = 134,
   RACE_SYLPH           = 135,
   RACE_HERMIT          = 136,
   RACE_SHARGUGH        = 137,
   RACE_GIANT           = 138, /* Prolly a humanoid..   */
   RACE_WARDEN          = 139, /* Warden???             */
   RACE_TROLL           = 140,
   RACE_NORSE_GOD       = 142, /* Hmmmm. prolly need better categories */
   RACE_MERMAID         = 145,
   RACE_SIREN           = 146,
   RACE_NAIAD           = 147,
   RACE_MERMAN          = 148,
   RACE_MINOTAUR        = 149,
   RACE_YETI            = 150,

   RACE_OTHER_HUMANOID  = 999,

   /* Is Giant a humanoid or a creature? */

   RACE_BEAR            = 1000,
   RACE_DOG             = 1001,
   RACE_WOLF            = 1002,
   RACE_FOX             = 1003,
   RACE_CAT             = 1004,
   RACE_RABBIT          = 1005,
   RACE_DEER            = 1006,
   RACE_COW             = 1007,
   RACE_HARE            = 1008,
   RACE_GOAT            = 1009,
   RACE_EAGLE           = 1010,
   RACE_PIG             = 1011,

   RACE_DUCK            = 1100, /* This will interest the biologists... */
   RACE_BIRD            = 1101, /* This will interest the biologists... */
   RACE_RAT             = 1102,
   RACE_HORSE           = 1103,
   RACE_BADGER          = 1104,
   RACE_SKUNK           = 1105,
   RACE_BOAR            = 1106,
   RACE_MOUSE           = 1107,
   RACE_MONKEY          = 1108,
   RACE_PORCUPINE       = 1110,
   RACE_ELEPHANT        = 1112,
   RACE_CAMEL           = 1113,
   RACE_FERRET          = 1114,
   RACE_VULTURE         = 1115,
   RACE_SQUIRREL        = 1116,
   RACE_OWL             = 1117,
   RACE_LEMURE          = 1118, /* Half-monkey (Makier) */
   RACE_ELK             = 1119, /* Larger deer (Whapiti-deer) */
   RACE_LION            = 1120,
   RACE_TIGER           = 1121,
   RACE_LEOPARD         = 1122,

   RACE_OTHER_MAMMAL    = 1999,

   RACE_TREE            = 2000,
   RACE_VINE            = 2001,
   RACE_FLOWER          = 2002,
   RACE_SEAWEED         = 2003,
   RACE_CACTUS          = 2004,

   RACE_OTHER_PLANT     = 2999,

   RACE_MAGGOT          = 3000,
   RACE_BEETLE          = 3001,
   RACE_SPIDER          = 3002,
   RACE_COCKROACH       = 3003,
   RACE_BUTTERFLY       = 3004,
   RACE_ANT             = 3005,
   RACE_WORM            = 3006,
   RACE_LEECH           = 3008,
   RACE_DRAGONFLY       = 3009,
   RACE_MOSQUITO        = 3010,

   RACE_OTHER_INSECT    = 3999,

   RACE_LIZARD          = 4000,
   RACE_SNAKE           = 4001,
   RACE_FROG            = 4002,
   RACE_ALLIGATOR       = 4004,
   RACE_DINOSAUR        = 4005,
   RACE_CHAMELEON       = 4006,
   RACE_SCORPION        = 4007,
   RACE_TURTLE          = 4008,
   RACE_BAT             = 4009,
   RACE_TOAD            = 4010,

   RACE_OTHER_REPTILE   = 4999,

   RACE_CAVE_WIGHT      = 5001, /* Some kinda creature... */
   RACE_UR_VILE         = 5002, /* Some kinda creature... */
   RACE_STONE_RENDER    = 5003, /* Some kinda creature... */
   RACE_VAMPIRE         = 5005,
   RACE_SLIME           = 5006,
   RACE_WYRM            = 5007,
   RACE_AUTOMATON       = 5008,
   RACE_UNICORN         = 5009,

   RACE_DRAGON_MIN      = 5010, /* For use with special object */
   RACE_DRAGON_BLACK    = 5010,
   RACE_DRAGON_BLUE     = 5011,
   RACE_DRAGON_GREEN    = 5012,
   RACE_DRAGON_RED      = 5013,
   RACE_DRAGON_WHITE    = 5014,
   RACE_DRAGON_SILVER   = 5015,
   RACE_DRAGON_TURTLE   = 5016,
   RACE_DRAGON_LAVA     = 5017,
   RACE_DRAGON_SHADOW   = 5018,
   RACE_DRAGON_LIZARD   = 5019,
   RACE_DRAGON_MAX      = 5020, /* For use with special object */

   RACE_LESSER_DEMON    = 5020, /* Approx. Level <=100,          */
   RACE_GREATER_DEMON   = 5021, /* Approx. Level >=100,          */
   RACE_SERVANT_DEMON   = 5022, /* Approx. < level=20,           */
   RACE_PRINCE_DEMON    = 5023, /* Almost god, max level=149, (no more!) */
   RACE_LESSER_DEVIL    = 5025, /* Approx. Level <=100, */
   RACE_GREATER_DEVIL   = 5026, /* Approx. Level >=100, */
   RACE_SHADOW_DEVIL    = 5027,
   RACE_ARCH_DEVIL      = 5028,

   RACE_MEDUSA          = 5030,
   RACE_WINGED_HORSE    = 5031,
   RACE_GARGOYLE        = 5033,
   RACE_GOLEM           = 5034,
   RACE_YOGOLOTH        = 5035,
   RACE_MIST_DWELLER    = 5036,

   RACE_WEREWOLF        = 5037,
   RACE_WERERAT         = 5038,

   RACE_ELEMENTAL_AIR   = 5040,
   RACE_ELEMENTAL_EARTH = 5041,
   RACE_ELEMENTAL_FIRE  = 5042,
   RACE_ELEMENTAL_FROST = 5043,
   RACE_ELEMENTAL_WATER = 5044,
   RACE_ELEMENTAL_LIGHT = 5045,

   RACE_DEVOURER        = 5600,
   RACE_DANALEK         = 5601,

   RACE_FAMILIAR        = 5900, /* Wierdo race... */

   RACE_OTHER_CREATURE  = 5999,

   RACE_ZOMBIE          = 6000,
   RACE_LICH            = 6001,
   RACE_GHOUL           = 6002,
   RACE_SKELETON        = 6003,
   RACE_GHOST           = 6004,
   RACE_SPIRIT          = 6005,
   RACE_MUMMIE          = 6006,
   RACE_BANSHEE         = 6007,
   RACE_NAGA_SOUL       = 6008,

   RACE_OTHER_UNDEAD    = 6999,

   RACE_CRAB            = 7000,
   RACE_SAND_SPIDER     = 7002,
   RACE_RIVER_LEECH     = 7003,
   RACE_SAND_CRAWLER    = 7004,
   RACE_SEA_HORSE       = 7005,
   RACE_SHARK           = 7006,
   RACE_LAMPREY         = 7007,
   RACE_MANTA_RAY       = 7008,
   RACE_CLIFF_HUGGER    = 7009,
   RACE_ALGAE_MAN       = 7010,
   RACE_WHELK           = 7011,
   RACE_OYSTER          = 7012,
   RACE_KRAKEN          = 7013,
   RACE_CAVE_FISHER     = 7014, /* Tiamat: lobster / spider breed */
   RACE_OCTOPUS         = 7015,
   RACE_WHALE           = 7016,
   RACE_DOLPHIN         = 7017,
   RACE_EEL             = 7018,

   RACE_FISH            = 7998,
   RACE_OTHER_MARINE    = 7999,

   RACE_DO_NOT_USE      = 15000,
};
/* ..................................................................... */
/*                              S P E L L S                              */
/* ..................................................................... */

/* Spell numbers. Only use the groups for 'spell[X]'... */
enum Spell
{
   SPL_NONE             = -1, /* For wand, scrolls, etc. */
   SPL_ALL              = 0,
   SPL_DIVINE           = 1,
   SPL_PROTECTION       = 2,
   SPL_DETECTION        = 3,
   SPL_SUMMONING        = 4,
   SPL_CREATION         = 5,
   SPL_MIND             = 6,
   SPL_HEAT             = 7,  /* Fire          */
   SPL_COLD             = 8,  /* Frost         */
   SPL_CELL             = 9,  /* Electricity   */
   SPL_INTERNAL         = 10, /* Poison        */
   SPL_EXTERNAL         = 11, /* Acid          */
   SPL_GROUP_MAX        = 12, /* #[0..11] =12, */

   SPL_CALL_LIGHTNING   = 12, /* Cell Group  */
   SPL_BLESS            = 13, /* D I V I N E */
   SPL_CURSE            = 14,
   SPL_REMOVE_CURSE     = 15,
   SPL_CURE_WOUNDS_1    = 16,
   SPL_CURE_WOUNDS_2    = 17,
   SPL_CURE_WOUNDS_3    = 18,
   SPL_CAUSE_WOUNDS_1   = 19,
   SPL_CAUSE_WOUNDS_2   = 20,
   SPL_CAUSE_WOUNDS_3   = 21,
   SPL_DISPEL_EVIL      = 22,
   SPL_REPEL_UNDEAD_1   = 23,
   SPL_REPEL_UNDEAD_2   = 24,
   SPL_BLIND            = 25,
   SPL_CURE_BLIND       = 26,
   SPL_LOCATE_OBJECT    = 27,
   SPL_LOCATE_CHAR      = 28,

   SPL_RAISE_MAG        = 29, /* P R O T E C T I O N */
   SPL_RAISE_DIV        = 30,
   SPL_RAISE_STR        = 31,
   SPL_RAISE_DEX        = 32,
   SPL_RAISE_CON        = 33,
   SPL_RAISE_CHA        = 34,
   SPL_RAISE_BRA        = 35,
   SPL_SUN_RAY          = 36,
   SPL_DIVINE_RESIST    = 37,
   SPL_QUICKEN          = 38,
   SPL_HASTE            = 39,
   SPL_RAISE_SUMMONING  = 40,
   SPL_AWAKEN           = 41,
   SPL_MIND_SHIELD      = 42,
   SPL_HEAT_RESI        = 43,
   SPL_COLD_RESI        = 44,
   SPL_ELECTRICITY_RESI = 45,
   SPL_POISON_RESI      = 46,
   SPL_ACID_RESI        = 47,
   SPL_PRO_EVIL         = 48,
   SPL_SANCTUARY        = 49,
   SPL_DISPEL_MAGIC     = 50,
   SPL_SUSTAIN          = 51,
   SPL_LOCK             = 52,
   SPL_UNLOCK           = 53,
   SPL_DROWSE           = 54,
   SPL_SLOW             = 55,
   SPL_DUST_DEVIL       = 56,

   SPL_DET_ALIGN        = 57, /* D E T E C T I O N */
   SPL_DET_INVISIBLE    = 58,
   SPL_DET_MAGIC        = 59,
   SPL_DET_POISON       = 60,
   SPL_DET_UNDEAD       = 61,
   SPL_DET_CURSE        = 62,
   SPL_SENSE_LIFE       = 63,
   SPL_IDENTIFY_1       = 64,
   SPL_IDENTIFY_2       = 65,

   SPL_RANDOM_TELEPORT  = 66, /* S U M M O N I N G */
   SPL_CLEAR_SKIES      = 67,
   SPL_STORM_CALL       = 68,
   SPL_WORD_OF_RECALL   = 69,
   SPL_CONTROL_TELEPORT = 70,
   SPL_MINOR_GATE       = 71,
   SPL_GATE             = 72,

   SPL_CREATE_FOOD      = 73, /* C R E A T I O N */
   SPL_CREATE_WATER     = 74,
   SPL_LIGHT_1          = 75,
   SPL_LIGHT_2          = 76,
   SPL_DARKNESS_1       = 77,
   SPL_DARKNESS_2       = 78,
   SPL_STUN             = 79,
   SPL_HOLD             = 80,
   SPL_ANIMATE_DEAD     = 81,
   SPL_LEATHER_SKIN     = 82,
   SPL_BARK_SKIN        = 83,
   SPL_CONTROL_UNDEAD   = 84,
   SPL_BONE_SKIN        = 85,
   SPL_STONE_SKIN       = 86,
   SPL_AID              = 87,

   SPL_COLOURSPRAY_1    = 88, /* M I N D */
   SPL_COLOURSPRAY_2    = 89,
   SPL_COLOURSPRAY_3    = 90,
   SPL_INVISIBILITY     = 91,
   SPL_WIZARD_EYE       = 92,
   SPL_FEAR             = 93,
   SPL_CONFUSION        = 94,
   SPL_SLEEP            = 95,
   SPL_XRAY_VISION      = 96,
   SPL_CALM             = 97,
   SPL_SUMMER_RAIN      = 98,
   SPL_COMMAND          = 99,
   SPL_LEAVING          = 100,

   SPL_FIREBALL_1       = 101, /* H E A T */
   SPL_FIREBALL_2       = 102,
   SPL_FIREBALL_3       = 103,

   SPL_FROSTBALL_1      = 104, /* C O L D */
   SPL_FROSTBALL_2      = 105,
   SPL_FROSTBALL_3      = 106,

   SPL_LIGHTNING_1      = 107, /* C E L L */
   SPL_LIGHTNING_2      = 108,
   SPL_LIGHTNING_3      = 109,

   SPL_STINKING_CLOUD_1 = 110, /* I N T E R N A L */
   SPL_STINKING_CLOUD_2 = 111,
   SPL_STINKING_CLOUD_3 = 112,
   SPL_POISON           = 113,
   SPL_REMOVE_POISON    = 114,
   SPL_ENERGY_DRAIN     = 115,
   SPL_DISEASE_1        = 116,
   SPL_DISEASE_2        = 117,
   SPL_REM_DISEASE      = 118,

   SPL_ACIDBALL_1       = 119, /* E X T E R N A L */
   SPL_ACIDBALL_2       = 120,
   SPL_ACIDBALL_3       = 121,

   SPL_MANA_BOOST       = 122, /* Creation */
   SPL_FIND_PATH        = 123, /* Divine   */
   SPL_DISPEL_GOOD      = 124,
   SPL_PRO_GOOD         = 125,
   SPL_TRANSPORT        = 126,

   SPL_FIRE_BREATH      = 127,
   SPL_FROST_BREATH     = 128,
   SPL_LIGHTNING_BREATH = 129,
   SPL_ACID_BREATH      = 130,
   SPL_GAS_BREATH       = 131,
   SPL_LIGHT_BREATH     = 132,
   SPL_HOLD_MONSTER     = 133,
   SPL_HOLD_UNDEAD      = 134,
   SPL_RAISE_DEAD       = 135,
   SPL_RESURRECTION     = 136,
   SPL_TOTAL_RECALL     = 137,
   SPL_UNDEAD_DOOR      = 138,
   SPL_LIFE_PROTECTION  = 139,

   SPL_ENERGY_BOLT      = 140,
   SPL_CLENCHED_FIST    = 141,
   SPL_METEOR_SHOWER    = 142,
   SPL_SUN_BEAM         = 143,
   SPL_SOLAR_FLARE      = 144,

   SPL_SUMMON_DEVIL     = 145,
   SPL_SUMMON_DEMON     = 146,
   SPL_SUMMON_FIRE      = 147,
   SPL_SUMMON_WATER     = 148,
   SPL_SUMMON_AIR       = 149,
   SPL_SUMMON_EARTH     = 150,

   SPL_CHARGE_WAND      = 151,
   SPL_CHARGE_STAFF     = 152,
   SPL_MENDING          = 153,
   SPL_REPAIR           = 154,
   SPL_RECONSTRUCT      = 155,
   SPL_SENDING          = 156,
   SPL_REFIT            = 157,
   SPL_FIND_WANTED      = 158,
   SPL_LOCATE_WANTED    = 159,

   /* Just insert any new spells here... just keep incrementing by one */

   SPL_STORM_GATE       = 160,
   SPL_SUN_GLOBE        = 161,
   SPL_MAGIC_CANDLE     = 162,
   SPL_SONIC_BREATH     = 163,
   SPL_SHARD_BREATH     = 164,
   SPL_CONE_SHARD       = 165,
   SPL_SACRED           = 166,

   SPL_BLANK1           = 167,
   SPL_BLANK2           = 168,
   SPL_BLANK3           = 169,
   SPL_BLANK4           = 170,
   SPL_BLANK5           = 171,
   SPL_BLANK6           = 172,
   SPL_BLANK7           = 173,
   SPL_BLANK8           = 174,
   SPL_BLANK9           = 175,

   SPL_TREE_MAX         = 220, /* #[0..167] =168, */
};

/* ..................................................................... */
/*                              S K I L L S                              */
/* ..................................................................... */
enum Skill
{
   SKI_TURN_UNDEAD    = 0,
   SKI_SCROLL_USE     = 1,
   SKI_WAND_USE       = 2,
   SKI_CONSIDER       = 3,
   SKI_DIAGNOSTICS    = 4,
   SKI_APPRAISAL      = 5,
   SKI_VENTRILOQUATE  = 6,
   SKI_WEATHER_WATCH  = 7,
   SKI_FLEE           = 8,
   SKI_SNEAK          = 9,
   SKI_BACKSTAB       = 10,
   SKI_HIDE           = 11,
   SKI_FIRST_AID      = 12,
   SKI_PICK_LOCK      = 13,
   SKI_STEAL          = 14,
   SKI_RESCUE         = 15,
   SKI_SEARCH         = 16,
   SKI_LEADERSHIP     = 17,
   SKI_KICK           = 18,
   SKI_SWIMMING       = 19,
   SKI_BASH           = 20,
   SKI_CLIMB          = 21,
   SKI_SHIELD         = 22,
   SKI_TRIP           = 23,
   SKI_DUAL_WIELD     = 24,
   SKI_CUFF           = 25,
   SKI_RESIZE_CLOTHES = 26,
   SKI_RESIZE_LEATHER = 27,
   SKI_RESIZE_METAL   = 28,
   SKI_EVALUATE       = 29, /* "Fake skill to simulate combinations */
   SKI_PEEK           = 30,
   SKI_PICK_POCKETS   = 31,
   SKI_FILCH          = 32,
   SKI_DISARM         = 33,
   SKI_SKIN           = 34,
   SKI_BLANK5         = 35,
   SKI_BLANK6         = 36,
   SKI_BLANK7         = 37,
   SKI_BLANK8         = 38,
   SKI_BLANK9         = 39,

   SKI_TREE_MAX       = 70,
};
/* ------------------------- Special Functions ------------------------------
When SFR_BLOCK is returned, all other scannings for further special routines
are canceled, as well as blocking any command issued.
   SFR_BLOCK

The function may share with others/continue command sequence
   SFR_SHARE
*/

#define SFR_BLOCK 5
#define SFR_SHARE 6

/* ------------------------- Special Functions ------------------------------
  Used in conjunction with 'special ... bits SFB_XXX' keyword.

  SFB_PRIORITY
    When this bit is set, no routines 'below' the specified routine will
    be allowed to be called on any basis. This is only performed locally
    relative to the unit in question ('below' is considered ->next).

  SFB_RANTIME
    (for compatibility also: SFB_TIME1, SFB_TIME2, SFB_TIME3)
    When this bit is set, the next event 'time' will be randomly requested
    at (T=time):   [T-T/2..T+T/2]
*/
enum SpecialFunctionBits
{
   SFB_PRIORITY = 0x0001, /* Dont allow SFUNS below when this is set   */
   SFB_RANTIME  = 0x0002, /* Time variation                            */
   SFB_CMD      = 0x0004, /* normal command                            */
   SFB_TICK     = 0x0008, /* tick messages                             */
   SFB_DEAD     = 0x0010, /* death message                             */
   SFB_COM      = 0x0020, /* combat event                              */
   SFB_MSG      = 0x0040, /* message event                             */
   SFB_SAVE     = 0x0080, /* save event                                */
   SFB_AWARE    = 0x0100, /* Activate self? (self awareness)           */
   SFB_ACTIVATE = 0x0200, /* DIL only! .. on actiavtion                */
   SFB_PRE      = 0x0400, /* Used to pre ack (block) messages          */
   SFB_DONE     = 0x0800, /* Used to tell when something has been done */

   SFB_ALL      = SFB_CMD | SFB_TICK | SFB_DEAD | SFB_COM | SFB_MSG | SFB_SAVE | SFB_PRE,
};
/* ------------------------- Special Functions ------------------------------

SFUN_DEATH_SEQ
   INTERNAL USE ONLY.

SFUN_BULLETIN_BOARD
   Ticks  : Do not set.
   Syntax : special SFUN_BULLETIN_BOARD "<board-name> [L<level>]"
   Example: special SFUN_BULLETIN_BOARD "wiz L200"
            special SFUN_BULLETIN_BOARD "citizen"

   The '[' and ']' means the enclosed is optional.
   Consult a higher level God to get permission to use a board which is NOT
   the standard one (`citizen')

   Later on, other exceptions to boardusage can easily, and without corrution
   of existing files, be added for guild (?), race, whatever.

SFUN_MOB_COMMAND
   Ticks  :
   Syntax :
   Example:

SFUN_HUNTING
   INTERNAL USE ONLY.

SFUN_POSTMAN
   For use with Jim the Postman.

SFUN_INTERN_SHOP
   INTERNAL DO NOT USE.

SFUN_EAT_AND_DELETE
   INTERNAL USE ONLY.

SFUN_ACCUSE
   Ticks  : Do not set.
   Synax  : special SFUN_ACCUSE
   Example: special SFUN_ACCUSE

   NPC's with this function can register accusations
   (for example Guard Captain).

SFUN_PROTECT_LAWFUL

SFUN_PAIN
   INTERNAL USE ONLY.

SFUN_PAIN_INIT

SFUN_NPC_VISIT_ROOM
   INTERNAL USE ONLY.

SFUN_MERCENARY_HIRE

SFUN_MERCENARY_HUNT
   INTERNAL USE ONLY.

SFUN_BANK

SFUN_DUMP

SFUN_FIDO
SFUN_JANITOR

SFUN_WARD_GUARD

SFUN_GUARD_RETURN
   INTERNAL USE ONLY.

SFUN_GUARD_ARRESTING

SFUN_CUFFS

SFUN_CUFFS_LINK
   INTERNAL USE ONLY.

SFUN_CAPTAIN

SFUN_TEACH_ABL

SFUN_TEACH_WPN

SFUN_RANDOM_GLOBAL_MOVE

SFUN_RANDOM_ZONE_MOVE

SFUN_SCAVENGER

SFUN_AGGRESSIVE
SFUN_AGGRES_REVERSE_ALIGN
   Ticks  : Set to how often you feel it shall check. Use SFB_RANTIME to
            get varied time. Probably set high value on ticks, like perhaps:
            time PULSE_SEC*20 bits SFB_RANTIME
   Syntax : special SFUN_AGGRESSIVE ["<string-to-room>"]
   Example: special SFUN_AGGRESSIVE "$1n ambushes $3n!" time 160
                                                        bits SFB_RANTIME

   When the NPC is called it will check if there are any PC's to attack.
   Use with for example robbers, 'teamwork' and 'rescue' - pissed dragons,
   etc-

  The REVERSE_ALIGN will only attack if the players has opposite alignment
  of the monster (good vs. evil or evil vs. good).

SFUN_TEACH_SPL

SFUN_TEACH_SKI

SFUN_COMBAT_MAGIC

SFUN_BLOW_AWAY

SFUN_ARREST_CHECK

SFUN_SOFA

SFUN_COMBAT_POISON

SFUN_HAON_UNDER_WEB
   For use in Haon-Dor specific location only.

SFUN_COMBAT_MAGIC
   Ticks  : Do not set.
   Syntax : special SFUN_COMBAT_MAGIC "cast '<spell>'"
   Example: special SFUN_COMBAT_MAGIC "cast 'fireball'"

   Used on NPC's. Activates in combat, and attacks opponent with spell.


SFUN_COMBAT_MAGIC_HEAL
   Ticks  : Do not set.
   Syntax : special SFUN_COMBAT_MAGIC_HEAL "cast '<spell>'"
   Example: special SFUN_COMBAT_MAGIC_HEAL "cast 'heal'"

   Used on NPC's. Activates in combat if hitpoints are below 52% of max.


SFUN_GUARD_WAY
   Ticks  : Do not set.
   Syntax: special SFUN_GUARD_WAY <direction>[location]@[exclude]@<to_char>@<to_others>
      Where direction is 0..5 for n,e,s,w,u,d respectively.
      location is the symbolic name of the unit that the npc must be in
      if the routine is to be valid.
      Exclude is list of names of characters to be excluded (spearated by /).
      Example: "@" or "captain collin@" or "king welmar/tim/tom@"

   Example: special SFUN_GUARD_WAY "0corridor_ne@king welmar/tim/tom@$1n stops you.@$1n prevents $3n from going north."
            special SFUN_GUARD_WAY "0@@$1n stops you.@$1n prevents $3n from going north."

   Used on NPC's to prevent any character from moving the specified direction.


SFUN_GUARD_DOOR
   Ticks  : Do not set.
   Syntax: special SFUN_GUARD_DOOR [location]@<door-name>@[exclude]@<to_char>@<to_others>
      Where door name is the name of the door.
      See also SFUN_GUARD_WAY

   Example: special SFUN_GUARD_DOOR "guard_office@east door@captain@$1n stops you.@$1n prevents $3n from opening the door."

   Used on NPC's to prevent any character from opening/unlocking the door.


SFUN_GUARD_UNIT
   Ticks  : Do not set.
   Syntax: special SFUN_GUARD_UNIT [location]@<unit-name>@[exclude]@<to_char>@<to_others>
      Where unit-name is the name of the unit to guard.
      See also SFUN_GUARD_WAY

   Example: special SFUN_GUARD_DOOR "@safe@@$1n stops you.@$1n prevents $3n from opening the safe."

   Used on NPC's to prevent any character from opening/unlocking the unit.

SFUN_GUARD_WAY_LEVEL
   Ticks  : Do not set.
   Syntax: special SFUN_GUARD_WAY_LEVEL <direction>[location]@[<min_lev>-<max_lev>]@<to_char>@<to_others>
      Where direction is 0..5 for n,e,s,w,u,d respectively.
      location is the symbolic name of the unit that the npc must be in
      if the routine is to be valid.
      min_lev is the level you have to be above to pass, and max_level is
      the level you have to be below to pass....

   Example: special SFUN_GUARD_WAY_LEVEL "0corridor_ne@1-30@$1n stops you.@$1n prevents $3n from going north."

   Used on NPC's to prevent pcs outside a given levelfield from moving the
   specified direction.

SFUN_RESCUE

   Ticks  : Do not set, uses auto-combat.
   Syntax : special SFUN_RESCUE "<char name>{</><char-name>}"
   Example: special SFUN_RESCUE "captain/guard/mayor"

   Used if a npc is supposed to be guarding certain characters.
   An NPC will attempt to rescue only if he has 50% or more
   of his hitpoints left, and the victim has less than 50% of
   his hitpoints left.

SFUN_TEAMWORK

   Ticks  : Do not set, uses auto combat.
   Syntax : special SFUN_TEAMWORK "<char name>{</><char-name>}"
   Example: special SFUN_TEAMWORK "robber/thief"

   Used on NPC's which are a team. It causes each npc with the
   function to engage in combat as soon as it discovers that
   one of it's mates are in combat.
   Makes a brilliant combination with SFUN_RESCUE.
   Naturally there is no need to use this routine if the NPC
   auto-attacks by other means (for example aggressive or
   cityguard function as these protect each other anyway).


SFUN_HIDEAWAY

   Ticks  : Set to a high value, no need to do this often.
   Syntax : special SFUN_HIDEAWAY
   Example: special SFUN_HIDEAWAY time WAIT_SEC*120 bits SFB_TIME1

   At each auto-tick (only) the NPC checks if it is hiding.
   If not, it will set the hide bit. It ought to use the
   real hide skill, but at the moment we only need this for
   robbers. Let me know if you feel you need the 'real'
   skill. Note that the HIDE bit is removed in case damage
   is sustained.


SFUN_ODIN_STATUE

   Ticks  : Do not set, uses pray command.
   Syntax : special SFUN_ODIN_STATUE
   Example: special SFUN_ODIN_STATUE

   Do not use, intended for internal use.

SFUN_FORCE_MOVE
   Ticks   : Set to how often you feel it shall work. Use SFB_TIMEX as
             you like (determines random distribution).
   Syntax  :
      special SFUN_FORCE_MOVE ["<to>!<string-to-victims>[@<string-to-others>]"]
   Use with: Rooms and Objects.
   Examples: special SFUN_FORCE_MOVE
      "river/gentle_stream!You drift to a more quite area of the river."
      time 160 time SFB_TIME1

      "haunted_house/second_floor!The floorboards break under your weight!@$1n crashes through the floor."

   It will make chars to move to the symbolic location specified in the
   <to> field. If you want to make a teleporting closet, a waterfall or
   perhaps a river, this is a great function.

SFUN_OBJ_GOOD_ONLY

   Ticks  : Do not set.
   Syntax : special SFUN_OBJ_GOOD_ONLY

   Set this function on objects which are only supposed to be used by
   characters of good alignment. When using this, also set the magic
   flags (UNIT_FL_MAGIC) and objects alignment (+1000) just to keep
   things right.
   Players of non-good alignment are damaged by the object by 25% of
   their own maximum hitpoints.

SFUN_OBJ_EVIL_ONLY

   Reverse effect of SFUN_OBJ_GOOD_ONLY

SFUN_DEATH_ROOM
 Ticks:  Set to suitable amount, suggest WAIT_SEC*5 .. WAIT_SEC*30
 Syntax: special SFUN_DEATH_ROOM ["<no>"] time ...

Where <no> is the amount of damage to give - no amount will cause
certain death (10.000 points of damage). Rooms using this function
might have exits, but a room for certain death should not! (or else
the character might have time to flee).
Generally we discourage the use of "certain death" - if for example
a player ends up in a lava-pit, just give him "300" damage points every
10 seconds. If he is lucky, he will have time to escape. Perhaps even
combine it with a SFUN_FORCE_MOVE to get him out of there automatically
(in waterfalls for example).

SFUN_EVALUATE
  Ticks  : None
  Used on: Monsters
  Syntax : special SFUN_EVALUATE ["<cost>"]
  Example: special SFUN_EVALUATE "17"

     The monster will be able to evaluate the quality of weapons,
     shields and armours at the cost <cost> per item (none specified
     makes default amount of 50).

SFUN_CLIMB HAS BEEN REPLACED WITH THE DIL "climb@function"

SFUN_RESTRICT_OBJ
  Ticks  : None
  Used on: OBJECTS
  Syntax : special SFUN_RESTRICT_OBJ "{ability <num><NL>} [level <num><NL>]"
  Example: special SFUN_RESTRICT_OBJ "Level = 10
                                      Str = 30
                                      Mag = 5"

           Is used to restrict use of items so that they require certain
           abilities or a certain level to use.

SFUN_SACRIFICE
  Ticks  : None
  Used on: ROOMS (OBJECTS, see note)
  Syntax : special SFUN_SACRIFICE
  Example: special SFUN_SACRIFICE

           This routine, when set inside a room, allows sacrifices to
           'nobles'. Normally only sacrifices to demigods are allowed,
           so this is supposed to be used at the nobles own house, king
           welmar, etc. It *is* possible to use this on an object, however
           until you convince Papi otherwise, I will not allow it.

*/

enum SpecialFunction
{
   SFUN_PERSIST_INTERNAL     = 0, /* Dont use.                       */
   SFUN_DILCOPY_INTERNAL     = 1, /* Dont use.                       */
   SFUN_BULLETIN_BOARD       = 2, /* Bulletin Board                  */
   SFUN_MOB_COMMAND          = 3, /* mobile command execution        */
   SFUN_HUNTING              = 4, /* INTERNAL, DO NOT USE!           */
   SFUN_POSTMAN              = 5, /* Mail, apply to post-man         */
   SFUN_INTERN_SHOP          = 6,
   SFUN_EAT_AND_DELETE       = 7,  /* INTERNAL, DO NOT USE!           */
   SFUN_ACCUSE               = 8,  /* Accuse function for law system  */
   SFUN_PROTECT_LAWFUL       = 9,  /* Protection of protected chars   */
   SFUN_PAIN                 = 10, /* INTERNAL, DO NOT USE!           */
   SFUN_PAIN_INIT            = 11, /* Basic NPC language              */
   SFUN_NPC_VISIT_ROOM       = 12, /* INTERNAL, DO NOT USE!           */
   SFUN_MERCENARY_HIRE       = 13, /* Mercenary hire routine          */
   SFUN_MERCENARY_HUNT       = 14, /* INTERNAL, DO NOT USE!           */
   SFUN_BANK                 = 15, /* Banker routine                  */
   SFUN_PERSIST_INIT         = 16, /* Persist routine                 */
   SFUN_FIDO                 = 17, /* Devour corpse function          */
   SFUN_JANITOR              = 18, /* Pick up trash function          */

   SFUN_CHAOS_DAEMON         = 19, /* Daemon of random events         */
   SFUN_OBSOLETE1            = 20,
   SFUN_OBSOLETE2            = 21,
   SFUN_OBSOLETE3            = 22,
   SFUN_OBSOLETE4            = 23,
   SFUN_OBSOLETE5            = 24,

   SFUN_TEACH_INIT           = 25, /* Midgaard teacher function       */
   SFUN_TEACHING             = 26, /* INTERNAL DO NOT USE!            */
   SFUN_RANDOM_GLOBAL_MOVE   = 27,
   SFUN_RANDOM_ZONE_MOVE     = 28,
   SFUN_SCAVENGER            = 29,
   SFUN_AGGRESSIVE           = 30,

   SFUN_LOG_OBJECT           = 31, /* Wiz tool.                       */
   SFUN_RECEP_DAEMON         = 32, /* Daemon to check players expiry  */
   SFUN_COMBAT_MAGIC         = 33, /* Use magic in combat             */

   SFUN_BLOW_AWAY            = 34,
   SFUN_OBSOLETE6            = 35,
   SFUN_COMPETITION_BOARD    = 36,

   SFUN_COMBAT_POISON        = 37,
   SFUN_OBSOLETE_XXX1        = 38,
   SFUN_OBSOLETE_XXX2        = 39,

   SFUN_COMBAT_MAGIC_HEAL    = 40, /* Use for self-healing            */

   SFUN_GUARD_WAY            = 41, /* Guards a direction agains move  */
   SFUN_GUARD_DOOR           = 42, /* Guard door against open/unlock  */
   SFUN_GUARD_UNIT           = 43, /* Guard unit against open/unlock  */
   SFUN_RESCUE               = 44,
   SFUN_TEAMWORK             = 45,
   SFUN_HIDEAWAY             = 46,
   SFUN_OBEY_ANIMAL          = 47, /* Was RAN_ATTACK */
   SFUN_SHOP_INIT            = 48,
   SFUN_AGGRES_REVERSE_ALIGN = 49,
   SFUN_CHARM_OF_DEATH       = 50, /* INTERNAL DO NOT USE!            */
   SFUN_DICTIONARY           = 51, /* Func for Alias object           */
   SFUN_XXX2                 = 52, /* Wiz toy? ;)                     */
   SFUN_GUARD_WAY_LEVEL      = 53, /* guards a dir from levels        */
   SFUN_ODIN_STATUE          = 54, /* Help for beginners              */
   SFUN_FORCE_MOVE           = 55,

   SFUN_WW_XXX1              = 56,
   SFUN_WW_XXX2              = 57,
   SFUN_WW_XXX3              = 58,
   SFUN_WW_XXX4              = 59,
   SFUN_WW_XXX5              = 60,
   SFUN_WW_XXX6              = 61,
   SFUN_WW_XXX8              = 62,
   SFUN_WW_XXX9              = 63,

   SFUN_OBJ_GOOD_ONLY        = 64,
   SFUN_OBJ_EVIL_ONLY        = 65,

   SFUN_CHANGE_HOMETOWN      = 66,

   SFUN_GUILD_GUARD_WAY      = 67,
   SFUN_MEMBERS_ONLY         = 68,
   SFUN_WHISTLE              = 69,

   SFUN_GUILD_MASTER         = 70,
   SFUN_GUILD_BASIS          = 71,

   SFUN_DEATH_ROOM           = 72,
   SFUN_XXXALSOOBSOLETE      = 73,
   SFUN_TUBORG               = 74,
   SFUN_GUILD_TITLES         = 75,
   SFUN_REWARD_BOARD         = 76,
   SFUN_REWARD_GIVE          = 77,
   SFUN_EVALUATE             = 78,
   SFUN_ERROR_ERASE          = 79, /* Do not use */
   SFUN_INFO_ERASE           = 80, /* Do not use */
   SFUN_XXXXX                = 81,
   SFUN_DIL_INTERNAL         = 82,
   SFUN_ORACLE               = 83,
   SFUN_ADMINISTRATOR        = 84,
   SFUN_OBEY                 = 85,
   SFUN_CRYSTAL_BALL         = 86,
   SFUN_SLIME_ADM            = 87,
   SFUN_FROZEN               = 88, /* Internal use only */
   SFUN_RETURN_TO_ORIGIN     = 89, /* Internal use only */
   SFUN_GUILD_INTERNAL       = 90, /* Internal use only */
   SFUN_RESTRICT_OBJ         = 91,
   SFUN_DEMI_STUFF           = 92, /* Internal use only */
   SFUN_LINK_DEAD            = 93, /* Internal use only */
   SFUN_SACRIFICE            = 94, /* Noble sacrifice   */

   SFUN_OBSOLETE_XXX5        = 95,
   SFUN_OBSOLETE_XXX6        = 96,
   SFUN_OBSOLETE_XXX7        = 97,

   SFUN_RESTRICT_QUEST       = 98,
   SFUN_RESTRICT_GUILD       = 99,
   SFUN_XXX                  = 100,
   SFUN_DIL_COPY             = 101,

   SFUN_TOP_IDX              = 101, /* SANITY CHECKING, ADJUST */
};
/* ------------------------- Affect Types ------------------------------
 Dont ask, and don't use. Use the predefined macros in wmacros.h
*/
enum AffectType
{
   ID_FILE_STORED         = 0, /* For empty affects in player-file */
   ID_NONE                = 1,
   ID_CORPSE              = 2, /* Makes corpses disappear */
   ID_VALHALLA            = 3,
   ID_LIFE_PROTECTION     = 4,
   ID_RAISE_MAG           = 5,
   ID_RAISE_DIV           = 6,
   ID_RAISE_STR           = 7,
   ID_RAISE_DEX           = 8,
   ID_RAISE_CON           = 9,
   ID_RAISE_CHA           = 10,
   ID_RAISE_BRA           = 11,
   ID_RAISE_HPP           = 12,
   ID_RAISE_DIVINE        = 13,
   ID_RAISE_SUMMONING     = 16,
   ID_RAISE_MIND          = 18,
   ID_RAISE_HEAT          = 19,
   ID_RAISE_COLD          = 20,
   ID_RAISE_CELL          = 21,
   ID_RAISE_INTERNAL      = 22,
   ID_RAISE_EXTERNAL      = 23,
   ID_POISON              = 24,
   ID_MAGIC_LIGHT         = 25, /* For magic spells              */
   ID_LIGHT_EXTINGUISH    = 26, /* For light/extinguish commands */
   ID_DETECT_ALIGN        = 27,
   ID_DETECT_INVISIBLE    = 28,
   ID_DETECT_MAGIC        = 29,
   ID_DETECT_POISON       = 30,
   ID_DETECT_UNDEAD       = 31,
   ID_DETECT_CURSE        = 32,
   ID_DETECT_LIFE         = 33,
   ID_BLIND_CHAR          = 34,
   ID_FEAR                = 35,
   ID_INVISIBILITY        = 36,
   ID_SNEAK               = 37, /* for sneak skill   */
   ID_HIDE                = 38, /* for hide skill    */
   ID_WITNESS             = 39, /* Used by justice.c */
   ID_SLEEP               = 40,
   ID_BLESS               = 41,
   ID_CURSE               = 42,
   ID_SANCTUARY           = 43,
   ID_INSANITY            = 44,
   ID_PROT_EVIL           = 45,
   ID_SUSTAIN             = 46,
   ID_PRISON              = 47,
   ID_PLAGUE              = 48,
   ID_ENCHANT_ARMOUR      = 49,
   ID_ENCHANT_WEAPON      = 50,
   ID_HOLD                = 51,
   ID_CHARM               = 52,
   ID_CLONE               = 53,
   ID_ABSORBTION          = 54, /* for indicating absorbtion spell */
   ID_ENERGY_DRAIN        = 55,
   ID_SPOTTED_SECRET      = 56,
   ID_HIDDEN_DIFFICULTY   = 57, /* Use macro: SECRET_DOOR_DIFFICULTY */
   ID_REWARD              = 58,
   ID_CONFUSION           = 59,
   ID_BURIED              = 60,
   ID_BACKSTABBED         = 61, /* When backstabbed, this improves defense */
   ID_MAX_ATTACKER        = 62, /* Record of highest level attacker        */
   ID_PROT_GOOD           = 63,
   ID_NATURAL_ARMOUR      = 64,
   ID_SPEED               = 65,
   ID_CHARFLAGS           = 66, /* For the transfer functions */
   ID_SPELL               = 67, /* ditto */
   ID_SKILL               = 68, /* ditto */
   ID_WEAPON              = 69, /* ditto */
   ID_MAGIC_DARK          = 70, /* For magic spells              */

   ID_SPL_RAISE_MAG       = 71,
   ID_SPL_RAISE_DIV       = 72,
   ID_SPL_RAISE_STR       = 73,
   ID_SPL_RAISE_DEX       = 74,
   ID_SPL_RAISE_CON       = 75,
   ID_SPL_RAISE_CHA       = 76,
   ID_SPL_RAISE_BRA       = 77,
   ID_SPL_RAISE_HPP       = 78,
   ID_SPL_RAISE_DIVINE    = 79,
   ID_SPL_RAISE_SUMMONING = 80,
   ID_SPL_RAISE_MIND      = 81,
   ID_SPL_RAISE_HEAT      = 82,
   ID_SPL_RAISE_COLD      = 83,
   ID_SPL_RAISE_CELL      = 84,
   ID_SPL_RAISE_INTERNAL  = 85,
   ID_SPL_RAISE_EXTERNAL  = 86,

   ID_WEAPON_DEFENSE      = 87,
   ID_SPELL_DEFENSE       = 88,

   ID_BLANK0              = 89,
   ID_BLANK1              = 90,
   ID_BLANK2              = 91,
   ID_BLANK3              = 92,
   ID_BLANK4              = 93,
   ID_BLANK5              = 94,
   ID_BLANK6              = 95,
   ID_BLANK7              = 96,
   ID_BLANK8              = 97,
   ID_BLANK9              = 98,
   ID_BLANK10             = 99,
   ID_BLANK11             = 100,

   ID_TOP_IDX             = 101, /* For sanity checking, adjust */

   ID_TRANSFER_MAG        = -ID_RAISE_MAG,
   ID_TRANSFER_DIV        = -ID_RAISE_DIV,
   ID_TRANSFER_STR        = -ID_RAISE_STR,
   ID_TRANSFER_DEX        = -ID_RAISE_DEX,
   ID_TRANSFER_CON        = -ID_RAISE_CON,
   ID_TRANSFER_CHA        = -ID_RAISE_CHA,
   ID_TRANSFER_BRA        = -ID_RAISE_BRA,
   ID_TRANSFER_HPP        = -ID_RAISE_HPP,
   ID_TRANSFER_SPEED      = -ID_SPEED,
   ID_PROT_GOOD_TRANSFER  = -ID_PROT_GOOD,
   ID_PROT_EVIL_TRANSFER  = -ID_PROT_EVIL,

   ID_TRANSFER_CHARFLAGS  = -ID_CHARFLAGS,
   ID_SPELL_TRANSFER      = -ID_SPELL,
   ID_SKILL_TRANSFER      = -ID_SKILL,
   ID_WEAPON_TRANSFER     = -ID_WEAPON,
};

enum Affect
{
   APF_NONE           = -1,
   APF_ABILITY        = 0,
   APF_SPELL_ADJ      = 1,
   APF_LIGHT_DARK     = 2,
   APF_MOD_CHAR_FLAGS = 3,
   APF_MOD_UNIT_FLAGS = 4,
   APF_MOD_OBJ_FLAGS  = 5,
   APF_SKILL_ADJ      = 6,
   APF_WEAPON_ADJ     = 7,
   APF_NATURAL_ARMOUR = 8,
   APF_SPEED          = 9,

   APF_MAX            = 9,
};

#define TIF_NONE             -1
#define TIF_CORPSE_DECAY     0
#define TIF_CORPSE_ZAP       1
#define TIF_VALHALLA_RET     2
#define TIF_HIT_INC          3
#define TIF_HIT_DEC          4
#define TIF_MAG_INC          5
#define TIF_MAG_DEC          6
#define TIF_DIV_INC          7
#define TIF_DIV_DEC          8
#define TIF_STR_INC          9
#define TIF_STR_DEC          10
#define TIF_DEX_INC          11
#define TIF_DEX_DEC          12
#define TIF_CON_INC          13
#define TIF_CON_DEC          14
#define TIF_CHA_INC          15
#define TIF_CHA_DEC          16
#define TIF_BRA_INC          17
#define TIF_BRA_DEC          18
#define TIF_TGH_INC          19
#define TIF_TGH_DEC          20
#define TIF_PROTECT_INC      21
#define TIF_PROTECT_DEC      22
#define TIF_POISON_ON        23
#define TIF_POISON_OFF       24
#define TIF_POISON_SUFFER    25
#define TIF_CHANGE_LIGHT_ADD 26
#define TIF_CHANGE_LIGHT_SUB 27
#define TIF_TORCH_TICK       28
#define TIF_EYES_TINGLE      29
#define TIF_BLIND_ON         30
#define TIF_BLIND_OFF        31
#define TIF_FEAR_CHECK       32
#define TIF_INVISIBILITY_ON  33
#define TIF_INVISIBILITY_OFF 34
#define TIF_SNEAK_ON         35
#define TIF_SNEAK_OFF        36
#define TIF_SNEAK_TICK       37
#define TIF_HIDE_ON          38
#define TIF_HIDE_OFF         39
#define TIF_HIDE_TICK        40
#define TIF_NOHIDE_TICK      41
#define TIF_SLEEP_ON         42
#define TIF_SLEEP_CHECK      43
#define TIF_SLEEP_OFF        44
#define TIF_BLESS_ON         45
#define TIF_BLESS_TICK       46
#define TIF_BLESS_OFF        47
#define TIF_CURSE_ON         48
#define TIF_CURSE_OFF        49
#define TIF_SANCTUARY_ON     50
#define TIF_SANCTUARY_TICK   51
#define TIF_SANCTUARY_OFF    52
#define TIF_INSANITY_ON      53
#define TIF_INSANITY_TICK    54
#define TIF_INSANITY_OFF     55
#define TIF_PROT_EVIL_ON     56
#define TIF_PROT_EVIL_OFF    57
#define TIF_SUSTAIN_ON       58
#define TIF_SUSTAIN_TICK     59
#define TIF_SUSTAIN_OFF      60
#define TIF_JAIL_WAIT        61
#define TIF_JAIL_RELEASE     62
#define TIF_PLAGUE_ON        63
#define TIF_PLAGUE_TICK      64
#define TIF_PLAGUE_OFF       65
#define TIF_REWARD_ON        66
#define TIF_REWARD_OFF       67
#define TIF_SPL_INC          68
#define TIF_SPL_DEC          69
#define TIF_SKI_INC          70
#define TIF_SKI_DEC          71
#define TIF_WPN_INC          72
#define TIF_WPN_DEC          73
#define TIF_CONFUSION_ON     74
#define TIF_CONFUSION_OFF    75
#define TIF_CONFUSION_TICK   76
#define TIF_BURIED_DESTRUCT  77
#define TIF_PROT_GOOD_ON     78
#define TIF_PROT_GOOD_OFF    79
#define TIF_ARMOUR_ON        80
#define TIF_ARMOUR_OFF       81
#define TIF_SPEED_BETTER     82
#define TIF_SPEED_WORSE      83

#define TIF_MAX 83

#define CMD_AUTO_DAMAGE  -10 /* Used when any damage is given               */
#define CMD_AUTO_ENTER   -9  /* Used when room is entered (walk, sail, etc) */
#define CMD_AUTO_SAVE    -8  /* Used when player is saved                   */
#define CMD_AUTO_MSG     -7  /* Used to pass messages                       */
#define CMD_AUTO_EXTRACT -6  /* Used when extract_unit is called            */
#define CMD_NON_USABLE   -5  /* Used when there is nothing apropriate       */
#define CMD_AUTO_UNKNOWN -4  /* Used when cmd_interpreter doesn't recognize */
                             /* command typed by ch. cmd->cmd_str is used   */
#define CMD_AUTO_DEATH  -3   /* Used when someone is killed (combat)        */
#define CMD_AUTO_COMBAT -2   /* Used when combat round                      */
#define CMD_AUTO_TICK   -1   /* MUST BE -1 ,Used when units are "ticking"   */

#define CMD_NORTH NORTH /* MUST BE 0 */
#define CMD_EAST  EAST  /* MUST BE 1 */
#define CMD_SOUTH SOUTH /* MUST BE 2 */
#define CMD_WEST  WEST  /* MUST BE 3 */
#define CMD_UP    UP    /* MUST BE 4 */
#define CMD_DOWN  DOWN  /* MUST BE 5 */

#define CMD_ENTER     7
#define CMD_EXIT      8
#define CMD_GET       10
#define CMD_DRINK     11
#define CMD_EAT       12
#define CMD_WEAR      13
#define CMD_WIELD     14
#define CMD_LOOK      15
#define CMD_SCORE     16
#define CMD_SAY       17
#define CMD_SHOUT     18
#define CMD_TELL      19
#define CMD_INVENTORY 20
#define CMD_GUILD     21

#define CMD_KILL   25
#define CMD_STATUS 26

#define CMD_HELP  38
#define CMD_WHO   39
#define CMD_EMOTE 40
#define CMD_STAND 42
#define CMD_SIT   43
#define CMD_REST  44
#define CMD_SLEEP 45
#define CMD_WAKE  46

#define CMD_NEWS      54
#define CMD_EQUIPMENT 55
#define CMD_BUY       56
#define CMD_SELL      57
#define CMD_VALUE     58
#define CMD_LIST      59
#define CMD_DROP      60
#define CMD_GOTO      61
#define CMD_WEATHER   62
#define CMD_READ      63
#define CMD_POUR      64
/* CMD_GRAB is CMD_HOLD */
#define CMD_REMOVE  66
#define CMD_PUT     67
#define CMD_SAVE    69
#define CMD_HIT     70
#define CMD_SET     71
#define CMD_GIVE    72
#define CMD_QUIT    73
#define CMD_TIME    76
#define CMD_IDEA    80
#define CMD_TYPO    81
#define CMD_BUG     82
#define CMD_WHISPER 83
#define CMD_CAST    84
#define CMD_AT      85
#define CMD_ASK     86
#define CMD_ORDER   87
#define CMD_SIP     88
#define CMD_TASTE   89
#define CMD_FOLLOW  91
#define CMD_RENT    92
#define CMD_OFFER   93
#define CMD_JOIN    94
#define CMD_AREAS   95

#define CMD_OPEN     99
#define CMD_CLOSE    100
#define CMD_LOCK     101
#define CMD_UNLOCK   102
#define CMD_LEAVE    103
#define CMD_MOUNT    104
#define CMD_DISMOUNT 105
#define CMD_RIDE     106
#define CMD_SAIL     107

#define CMD_WRITE    149
#define CMD_HOLD     150
#define CMD_FLEE     151
#define CMD_SNEAK    152
#define CMD_HIDE     153
#define CMD_BACKSTAB 154
#define CMD_PICK     155
#define CMD_STEAL    156
#define CMD_BASH     157
#define CMD_RESCUE   158
#define CMD_KICK     159
#define CMD_PRACTICE 164

#define CMD_EXAMINE 166
#define CMD_INFO    168

#define CMD_USE   172
#define CMD_WHERE 173
#define CMD_LEVEL 174
#define CMD_WIZ   175

#define CMD_BRIEF         199
#define CMD_WIZLIST       200
#define CMD_CONSIDER      201
#define CMD_GROUP         202
#define CMD_QUAFF         206
#define CMD_RECITE        207
#define CMD_POSE          209
#define CMD_CREDITS       212
#define CMD_COMPACT       213
#define CMD_DIG           214
#define CMD_BURY          215
#define CMD_TURN          217
#define CMD_DIAGNOSE      218
#define CMD_APPRAISE      219
#define CMD_VENTRILOQUATE 220
#define CMD_AID           221
#define CMD_CLIMB         222
#define CMD_LIGHT         223
#define CMD_EXTINGUISH    224
#define CMD_WIMPY         225
#define CMD_PEACEFUL      226
#define CMD_DRAG          227
#define CMD_TRIP          228
#define CMD_CUFF          229

#define CMD_EXITS      230 /* Also 'directions' and 'doors' */
#define CMD_MAIL       231
#define CMD_REQUEST    232
#define CMD_CONTRACT   233
#define CMD_BREAK      235
#define CMD_KNOCK      236
#define CMD_CHANGE     237
#define CMD_SPLIT      238
#define CMD_PROMPT     239
#define CMD_REPLY      240
#define CMD_ECHOSAY    241
#define CMD_SEARCH     242
#define CMD_QUESTS     243
#define CMD_DECAPITATE 244
#define CMD_PURSE      245
#define CMD_SACRIFICE  246
#define CMD_COMMANDS   247
#define CMD_SOCIALS    248
#define CMD_ACCOUNT    249
#define CMD_INFORM     250
#define CMD_EXPERT     251
#define CMD_RESIZE     252
#define CMD_EVALUATE   253
#define CMD_DITCH      254
#define CMD_IGNORE     255

#define CMD_PEEK        300
#define CMD_FILCH       301
#define CMD_PICK_POCKET 302
#define CMD_DISARM      303
#define CMD_DONATE      304
#define CMD_ASSIST      305
#define CMD_SKIN        306
#define CMD_SKILL7      307
#define CMD_SKILL8      308
#define CMD_SKILL9      309
#define CMD_SKILL9      309
#define CMD_SKILL10     310
#define CMD_SKILL11     311
#define CMD_SKILL12     312
#define CMD_SKILL13     313
#define CMD_SKILL14     314
#define CMD_SKILL15     315
#define CMD_SKILL16     316
#define CMD_SKILL17     317
#define CMD_SKILL18     318
#define CMD_SKILL19     319
#define CMD_SKILL20     320
#define CMD_SKILL21     321
#define CMD_SKILL22     322
#define CMD_SKILL23     323
#define CMD_SKILL24     324
#define CMD_SKILL25     325
#define CMD_SKILL26     326
#define CMD_SKILL27     327
#define CMD_SKILL28     328
#define CMD_SKILL29     329
#define CMD_SKILL30     330

#define CMD_FREEZE    1000
#define CMD_NOSHOUT   1002
#define CMD_PATH      1003
#define CMD_WIZINV    1004
#define CMD_RESTORE   1005
#define CMD_BAN       1006
#define CMD_SWITCH    1007
#define CMD_USERS     1008
#define CMD_WIZHELP   1009
#define CMD_NOTELL    1010
#define CMD_FILE      1011
#define CMD_CRASH     1012
#define CMD_ADVANCE   1013
#define CMD_LOAD      1014
#define CMD_PURGE     1015
#define CMD_SHUTDOWN  1016
#define CMD_ECHO      1017
#define CMD_FORCE     1018
#define CMD_TRANSFER  1019
#define CMD_REROLL    1020
#define CMD_SNOOP     1021
#define CMD_WSTAT     1022
#define CMD_SETSKILL  1023
#define CMD_FINGER    1024
#define CMD_MESSAGE   1025
#define CMD_BROADCAST 1026
#define CMD_DELETE    1027
#define CMD_TITLE     1028
#define CMD_MAKEMONEY 1029
#define CMD_WIZLOCK   1030
#define CMD_RESET     1031
#define CMD_MANIFEST  1032
#define CMD_VERIFY    1033
#define CMD_BOARDS    1034
#define CMD_KICKIT    1035
#define CMD_CORPSES   1036
#define CMD_REBOOT    1037
#define CMD_EXECUTE   1038

#define CMD_A_SOCIAL                                                                                                                       \
   1000 /* This is a little special... if set,                                                                                             \
           then the executed command is a social cmd */
#define CMD_PRAY   1998
#define CMD_INSULT 1999

#define CMD_BOW     "bow"
#define CMD_NOD     "nod"
#define CMD_SHAKE   "shake"
#define CMD_PRESS   "press"
#define CMD_PUSH    "push"
#define CMD_TOUCH   "touch"
#define CMD_WORSHIP "worship"

/* #define CMD_TAKE        167 replaced by GET      */
/* #define CMD_'           169 replaced by say      */
/* #define CMD_PRACTISE    170 replaced by practiCe */
/* #define CMD_,           177 replaced by emote    */
