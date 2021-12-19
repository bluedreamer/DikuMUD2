#pragma once
/* *********************************************************************** *
 * File   : utils.h                                   Part of Valhalla MUD *
 * Version: 2.05                                                           *
 * Author : all                                                            *
 *                                                                         *
 * Purpose: Macros for accessing fields in structures, and defines.        *
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

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "essential.h"
#include "file_index_type.h"
#include "structs.h"

extern const int8_t time_light[];

/* ..................................................................... */

constexpr bool PK_RELAXED = false;
constexpr bool PK_STRICT  = true;

#define IS_BEGINNER(ch)      (CHAR_LEVEL(ch) <= START_LEVEL)
#define IS_MORTAL(ch)        (CHAR_LEVEL(ch) < IMMORTAL_LEVEL)
#define IS_IMMORTAL(ch)      (CHAR_LEVEL(ch) >= IMMORTAL_LEVEL)
#define IS_GOD(ch)           (CHAR_LEVEL(ch) >= GOD_LEVEL)
#define IS_CREATOR(ch)       (CHAR_LEVEL(ch) >= CREATOR_LEVEL)
#define IS_OVERSEER(ch)      (CHAR_LEVEL(ch) >= OVERSEER_LEVEL)
#define IS_ADMINISTRATOR(ch) (CHAR_LEVEL(ch) >= ADMINISTRATOR_LEVEL)
#define IS_ULTIMATE(ch)      (CHAR_LEVEL(ch) == ULTIMATE_LEVEL)

/* ..................................................................... */

/*  Do NOT use these macros unless you know PRECISELY what you are doing!!!
 *  and only if you have to assign directly to them (as in db_utils.c)
 */
//#define U_CHAR(u) ((u)->data.ch)
inline auto U_CHAR(unit_data *u) -> char_data *&
{
   return u->data.ch;
}
//#define U_NPC(u)  (U_CHAR(u)->specific.npc)
inline auto U_NPC(unit_data *u) -> npc_data *&
{
   return U_CHAR(u)->specific.npc;
}
//#define U_PC(u)   (U_CHAR(u)->specific.pc)
inline auto U_PC(unit_data *u) -> pc_data *&
{
   return U_CHAR(u)->specific.pc;
}
//#define U_OBJ(u)  ((u)->data.obj)
inline auto U_OBJ(unit_data *u) -> obj_data *&
{
   return u->data.obj;
}
//#define U_ROOM(u) ((u)->data.room)
inline auto U_ROOM(unit_data *u) -> room_data *&
{
   return u->data.room;
}

#ifdef MUD_DEBUG
   #define DEBUG_HISTORY
   #define UNIT_TYPE_DEBUG
#endif

inline auto IS_CHAR(const unit_data *ptr) -> bool;
#ifdef UNIT_TYPE_DEBUG
//   #define UCHAR(u) (assert(u && IS_CHAR(u)), U_CHAR(u))
inline auto UCHAR(unit_data *u) -> char_data *&
{
   assert(u && IS_CHAR(u));

   return U_CHAR(u);
}
   #define UNPC(u)  (assert(u &&IS_NPC(u)), U_NPC(u))
   #define UPC(u)   (assert(u &&IS_PC(u)), U_PC(u))
   #define UOBJ(u)  (assert(u &&IS_OBJ(u)), U_OBJ(u))
   #define UROOM(u) (assert(u &&IS_ROOM(u)), U_ROOM(u))
#else
   #define UCHAR(u) U_CHAR(u)
   #define UNPC(u)  U_NPC(u)
   #define UPC(u)   U_PC(u)
   #define UOBJ(u)  U_OBJ(u)
   #define UROOM(u) U_ROOM(u)
#endif

/* ..................................................................... */

//#define UNIT_FUNC(unit) ((unit)->func)
inline auto UNIT_FUNC(unit_data *u) -> unit_fptr *&
{
   return u->func;
}
//#define UNIT_FILE_INDEX(unit) ((unit)->fi)
inline auto UNIT_FILE_INDEX(unit_data *u) -> file_index_type *&
{
   return u->fi;
}
//#define UNIT_MANIPULATE(unit) ((unit)->manipulate)
inline auto UNIT_MANIPULATE(unit_data *u) -> uint32_t &
{
   return u->manipulate;
}
//#define UNIT_FLAGS(unit) ((unit)->flags)
inline auto UNIT_FLAGS(unit_data *u) -> uint16_t &
{
   return u->flags;
}
//#define UNIT_WEIGHT(unit) ((unit)->weight)
inline auto UNIT_WEIGHT(unit_data *u) -> int16_t &
{
   return u->weight;
}
//#define UNIT_BASE_WEIGHT(unit) ((unit)->base_weight)
inline auto UNIT_BASE_WEIGHT(unit_data *u) -> int16_t &
{
   return u->base_weight;
}
//#define UNIT_SIZE(unit) ((unit)->size)
inline auto UNIT_SIZE(unit_data *u) -> uint16_t &
{
   return u->size;
}
//#define UNIT_CAPACITY(unit) ((unit)->capacity)
inline auto UNIT_CAPACITY(unit_data *u) -> int16_t &
{
   return u->capacity;
}
//#define UNIT_BRIGHT(unit) ((unit)->bright)
inline auto UNIT_BRIGHT(unit_data *u) -> int8_t &
{
   return u->bright;
}
//#define UNIT_LIGHTS(unit) ((unit)->light)
inline auto UNIT_LIGHTS(unit_data *u) -> int8_t &
{
   return u->light;
}
//#define UNIT_ILLUM(unit) ((unit)->illum)
inline auto UNIT_ILLUM(unit_data *u) -> int8_t &
{
   return u->illum;
}
//#define UNIT_CHARS(unit) ((unit)->chars)
inline auto UNIT_CHARS(unit_data *u) -> uint8_t &
{
   return u->chars;
}
//#define UNIT_CONTAINS(unit) ((unit)->inside)
inline auto UNIT_CONTAINS(unit_data *u) -> unit_data *&
{
   return u->inside;
}
//#define UNIT_IN(unit) ((unit)->outside)
inline auto UNIT_IN(unit_data *u) -> unit_data *&
{
   return u->outside;
}
//#define UNIT_AFFECTED(unit) ((unit)->affected)
inline auto UNIT_AFFECTED(unit_data *u) -> unit_affected_type *&
{
   return u->affected;
}
//#define UNIT_NAMES(unit) ((unit)->names)
inline auto UNIT_NAMES(unit_data *u) -> cNamelist &
{
   return u->names;
}
//#define UNIT_NAME(unit) (UNIT_NAMES(unit).Name())
inline auto UNIT_NAME(unit_data *u) -> const char *
{
   return UNIT_NAMES(u).Name();
}
//#define UNIT_KEY(unit) ((unit)->key)
inline auto UNIT_KEY(unit_data *u) -> file_index_type *&
{
   return u->key;
}
//#define UNIT_OPEN_FLAGS(unit) ((unit)->open_flags)
inline auto UNIT_OPEN_FLAGS(unit_data *u) -> uint8_t &
{
   return u->open_flags;
}
//#define UNIT_TYPE(unit) ((unit)->status)
inline auto UNIT_TYPE(const unit_data *u) -> uint8_t
{
   return u->status;
}

//#define UNIT_ALIGNMENT(unit) ((unit)->alignment)
inline auto UNIT_ALIGNMENT(unit_data *u) -> int16_t &
{
   return u->alignment;
}
//#define UNIT_HIT(unit) ((unit)->hp)
inline auto UNIT_HIT(unit_data *u) -> int32_t &
{
   return u->hp;
}
//#define UNIT_MAX_HIT(unit) ((unit)->max_hp)
inline auto UNIT_MAX_HIT(unit_data *u) -> int32_t &
{
   return u->max_hp;
}
//#define UNIT_MINV(u) ((u)->minv)
inline auto UNIT_MINV(unit_data *u) -> uint8_t &
{
   return u->minv;
}
/* --- Swapped --- */

//#define UNIT_TITLE(unit) ((unit)->title)
inline auto UNIT_TITLE(const unit_data *u) -> const cStringInstance &
{
   return u->title;
}
//#define UNIT_OUT_DESCR(unit) ((unit)->out_descr)
inline auto UNIT_OUT_DESCR(unit_data *u) -> cStringInstance &
{
   return u->out_descr;
}
//#define UNIT_IN_DESCR(unit) ((unit)->in_descr)
inline auto UNIT_IN_DESCR(const unit_data *u) -> const cStringInstance &
{
   return u->in_descr;
}
//#define UNIT_EXTRA_DESCR(unit) ((unit)->extra_descr)
inline auto UNIT_EXTRA_DESCR(unit_data *u) -> extra_descr_data *&
{
   return u->extra_descr;
}
/* ..................................................................... */

//#define IS_ROOM(unit) (UNIT_TYPE(unit) == UNIT_ST_ROOM)
inline auto IS_ROOM(const unit_data *ptr) -> bool
{
   return UNIT_TYPE(ptr) == UNIT_ST_ROOM;
}

//#define IS_OBJ(unit) (UNIT_TYPE(unit) == UNIT_ST_OBJ)
inline auto IS_OBJ(const unit_data *ptr) -> bool
{
   return UNIT_TYPE(ptr) == UNIT_ST_OBJ;
}
//#define IS_NPC(unit) (UNIT_TYPE(unit) == UNIT_ST_NPC)
inline auto IS_NPC(const unit_data *ptr) -> bool
{
   return UNIT_TYPE(ptr) == UNIT_ST_NPC;
}
//#define IS_PC(unit) (UNIT_TYPE(unit) == UNIT_ST_PC)
inline auto IS_PC(const unit_data *ptr) -> bool
{
   return UNIT_TYPE(ptr) == UNIT_ST_PC;
}
//#define IS_CHAR(unit) (IS_SET(UNIT_TYPE(unit), UNIT_ST_PC | UNIT_ST_NPC))
inline auto IS_CHAR(const unit_data *ptr) -> bool
{
   return IS_SET(UNIT_TYPE(ptr), UNIT_ST_PC | UNIT_ST_NPC);
}
/* ............................FILE INDEX STUFF..................... */

//#define FI_ZONENAME(fi) ((fi) ? (fi)->zone->name : "NO-ZONE")
inline auto FI_ZONENAME(file_index_type *fi) -> const char *
{
   return fi != nullptr ? fi->zone->name : "NO-ZONE";
}
//#define FI_NAME(fi) ((fi) ? (fi)->name : "NO-NAME")
inline auto FI_NAME(file_index_type *fi) -> const char *
{
   return fi != nullptr ? fi->name : "NO-NAME";
}

/* ............................UNIT SUPERSTRUCTURES..................... */

#define UNIT_IS_TRANSPARENT(u)                                                                                                             \
   (!IS_SET(UNIT_FLAGS(u), UNIT_FL_BURIED) && IS_SET(UNIT_FLAGS(u), UNIT_FL_TRANS) && !IS_SET(UNIT_OPEN_FLAGS(u), EX_CLOSED))

#define UNIT_FI_ZONE(unit) (UNIT_FILE_INDEX(unit) ? (unit)->fi->zone : (struct zone_type *)NULL)

#define UNIT_FI_ZONENAME(unit) (FI_ZONENAME(UNIT_FILE_INDEX(unit)))

//#define UNIT_FI_NAME(unit) (FI_NAME(UNIT_FILE_INDEX(unit)))
inline auto UNIT_FI_NAME(unit_data *unit) -> const char *
{
   return FI_NAME(UNIT_FILE_INDEX(unit));
}

#define UNIT_WEAR(unit, part) (IS_SET(UNIT_MANIPULATE(unit), part))

#define UNIT_IS_OUTSIDE(unit) (!IS_SET(UNIT_FLAGS(UNIT_IN(unit)), UNIT_FL_INDOORS))

extern int sunlight;

#define UNIT_OUTSIDE_LIGHT(unit) (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_INDOORS) ? time_light[sunlight] : 0)

#define UNIT_IS_DARK(unit) (UNIT_LIGHTS(unit) + UNIT_OUTSIDE_LIGHT(unit) + (UNIT_IN(unit) ? UNIT_LIGHTS(UNIT_IN(unit)) : 0) < 0)

#define UNIT_IS_LIGHT(unit) (UNIT_LIGHTS(unit) + UNIT_OUTSIDE_LIGHT(unit) + (UNIT_IN(unit) ? UNIT_LIGHTS(UNIT_IN(unit)) : 0) >= 0)

#define UNIT_SEX(unit) (IS_CHAR(unit) ? CHAR_SEX(unit) : SEX_NEUTRAL)

#define UNIT_IS_GOOD(ch)    (UNIT_ALIGNMENT(ch) >= 350)
#define UNIT_IS_EVIL(ch)    (UNIT_ALIGNMENT(ch) <= -350)
#define UNIT_IS_NEUTRAL(ch) (!UNIT_IS_GOOD(ch) && !UNIT_IS_EVIL(ch))

#define UNIT_CONTAINING_W(u) (UNIT_WEIGHT(u) - UNIT_BASE_WEIGHT(u))

/* ..................................................................... */

#define ROOM_RESISTANCE(room) (UROOM(room)->resistance)

#define ROOM_LANDSCAPE(unit) (UROOM(unit)->movement_type)

#define ROOM_FLAGS(unit) (UROOM(unit)->flags)

#define ROOM_EXIT(unit, exit) (UROOM(unit)->dir_option[exit])

/* ..................................................................... */

#define OBJ_RESISTANCE(obj) (UOBJ(obj)->resistance)

#define OBJ_VALUE(unit, index) (UOBJ(unit)->value[index])

#define OBJ_PRICE(unit) (UOBJ(unit)->cost)

#define OBJ_PRICE_DAY(unit) (UOBJ(unit)->cost_per_day)

#define OBJ_TYPE(unit) (UOBJ(unit)->type)

#define OBJ_EQP_POS(unit) (UOBJ(unit)->equip_pos)

#define OBJ_FLAGS(obj) (UOBJ(obj)->flags)

/* ...........................OBJECT SUPERSTRUCTURES..................... */

#define OBJ_HAS_EXTRA(obj, stat) (IS_SET(OBJ_EXTRA(obj), stat))

#define UNIT_IS_EQUIPPED(obj) (IS_OBJ(obj) && OBJ_EQP_POS(obj))

/* ..................................................................... */

/*#define CHAR_DEX_RED(ch)     \  (UCHAR(ch)->points.dex_reduction)*/

//#define CHAR_DESCRIPTOR(ch) (UCHAR(ch)->descriptor)
inline auto CHAR_DESCRIPTOR(unit_data *ch) -> descriptor_data *&
{
   return UCHAR(ch)->descriptor;
}

//#define CHAR_OFFENSIVE(unit) (UCHAR(unit)->points.offensive)
inline auto CHAR_OFFENSIVE(unit_data *ch) -> int16_t &
{
   return UCHAR(ch)->points.offensive;
}
//#define CHAR_DEFENSIVE(unit) (UCHAR(unit)->points.defensive)
inline auto CHAR_DEFENSIVE(unit_data *ch) -> int16_t &
{
   return UCHAR(ch)->points.defensive;
}
//#define CHAR_FLAGS(unit) (UCHAR(unit)->points.flags)
inline auto CHAR_FLAGS(unit_data *ch) -> uint32_t &
{
   return UCHAR(ch)->points.flags;
}
//#define CHAR_SEX(ch) (UCHAR(ch)->points.sex)
inline auto CHAR_SEX(unit_data *ch) -> uint8_t &
{
   return UCHAR(ch)->points.sex;
}
//#define CHAR_LAST_ROOM(unit) (UCHAR(unit)->last_room)
inline auto CHAR_LAST_ROOM(unit_data *ch) -> unit_data *&
{
   return UCHAR(ch)->last_room;
}
//#define CHAR_POS(ch) (UCHAR(ch)->points.position)
inline auto CHAR_POS(unit_data *ch) -> uint8_t &
{
   return UCHAR(ch)->points.position;
}
//#define CHAR_LEVEL(ch) (UCHAR(ch)->points.level)
inline auto CHAR_LEVEL(unit_data *ch) -> uint8_t &
{
   return UCHAR(ch)->points.level;
}
//#define CHAR_RACE(ch) (UCHAR(ch)->points.race)
inline auto CHAR_RACE(unit_data *ch) -> uint16_t &
{
   return UCHAR(ch)->points.race;
}
//#define CHAR_ABILITY(ch, index) (UCHAR(ch)->points.abilities[index])
inline auto CHAR_ABILITY(unit_data *ch, size_t index) -> uint8_t &
{
   return UCHAR(ch)->points.abilities[index];
}
//#define CHAR_STR(ch) (CHAR_ABILITY(ch, ABIL_STR))
inline auto CHAR_STR(unit_data *ch) -> uint8_t &
{
   return CHAR_ABILITY(ch, ABIL_STR);
}
//#define CHAR_DEX(ch) (CHAR_ABILITY(ch, ABIL_DEX))
inline auto CHAR_DEX(unit_data *ch) -> uint8_t &
{
   return CHAR_ABILITY(ch, ABIL_DEX);
}
//#define CHAR_CON(ch) (CHAR_ABILITY(ch, ABIL_CON))
inline auto CHAR_CON(unit_data *ch) -> uint8_t &
{
   return CHAR_ABILITY(ch, ABIL_CON);
}
//#define CHAR_CHA(ch) (CHAR_ABILITY(ch, ABIL_CHA))
inline auto CHAR_CHA(unit_data *ch) -> uint8_t &
{
   return CHAR_ABILITY(ch, ABIL_CHA);
}
//#define CHAR_BRA(ch) (CHAR_ABILITY(ch, ABIL_BRA))
inline auto CHAR_BRA(unit_data *ch) -> uint8_t &
{
   return CHAR_ABILITY(ch, ABIL_BRA);
}
//#define CHAR_MAG(ch) (CHAR_ABILITY(ch, ABIL_MAG))
inline auto CHAR_MAG(unit_data *ch) -> uint8_t &
{
   return CHAR_ABILITY(ch, ABIL_MAG);
}
//#define CHAR_DIV(ch) (CHAR_ABILITY(ch, ABIL_DIV))
inline auto CHAR_DIV(unit_data *ch) -> uint8_t &
{
   return CHAR_ABILITY(ch, ABIL_DIV);
}
//#define CHAR_HPP(ch) (CHAR_ABILITY(ch, ABIL_HP))
inline auto CHAR_HPP(unit_data *ch) -> uint8_t &
{
   return CHAR_ABILITY(ch, ABIL_HP);
}
#define CHAR_ENDURANCE(ch) (UCHAR(ch)->points.endurance)

#define CHAR_MANA(ch) (UCHAR(ch)->points.mana)

/* NOT to be used unless by db.... */
#define CHAR_MONEY(ch) (UCHAR(ch)->money)

#define CHAR_EXP(ch) (UCHAR(ch)->points.exp)

#define CHAR_ATTACK_TYPE(ch) (UCHAR(ch)->points.attack_type)

#define CHAR_NATURAL_ARMOUR(ch) (UCHAR(ch)->points.natural_armour)

#define CHAR_SPEED(ch) (UCHAR(ch)->points.speed)

#define CHAR_COMBAT(ch) (UCHAR(ch)->Combat)

#define CHAR_FIGHTING(ch) (UCHAR(ch)->Combat ? CHAR_COMBAT(ch)->Melee() : (unit_data *)NULL)

#define CHAR_MASTER(ch) (UCHAR(ch)->master)

#define CHAR_FOLLOWERS(ch) (UCHAR(ch)->followers)

/* ...........................CHAR SUPERSTRUCTURES....................... */

#define CHAR_IS_READY(ch) (CHAR_AWAKE(ch) && (CHAR_POS(ch) != POSITION_FIGHTING))

#define CHAR_IS_SNOOPING(ch) (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->snoop.snooping) : (unit_data *)NULL)

#define CHAR_IS_SNOOPED(ch) (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->snoop.snoop_by) : (unit_data *)NULL)

#define CHAR_IS_SWITCHED(ch) (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->original) : (unit_data *)NULL)

#define CHAR_SNOOPING(ch) (CHAR_IS_SNOOPING(ch) ? (CHAR_DESCRIPTOR(ch)->snoop.snooping) : (ch))

#define CHAR_SNOOPED(ch) (CHAR_IS_SNOOPED(ch) ? (CHAR_DESCRIPTOR(ch)->snoop.snoop_by) : (ch))

#define CHAR_ORIGINAL(ch) (CHAR_IS_SWITCHED(ch) ? (CHAR_DESCRIPTOR(ch)->original) : (ch))

#define CHAR_AWAKE(ch) (CHAR_POS(ch) > POSITION_SLEEPING)

#define CHAR_HAS_FLAG(ch, flags) (IS_SET(CHAR_FLAGS(ch), (flags)))

#define CHAR_ROOM_EXIT(ch, door) (IS_ROOM(UNIT_IN(ch)) ? ROOM_EXIT(UNIT_IN(ch), (door)) : (unit_data *)NULL)

#define CHAR_VISION(ch) (!CHAR_HAS_FLAG(ch, CHAR_BLIND))

#define CHAR_CAN_SEE(ch, unit)                                                                                                             \
   (CHAR_VISION(ch) && !IS_SET(UNIT_FLAGS(unit), UNIT_FL_BURIED) && (CHAR_LEVEL(ch) >= UNIT_MINV(unit)) &&                                 \
    (CHAR_LEVEL(ch) >= CREATOR_LEVEL ||                                                                                                    \
     (UNIT_IS_LIGHT(UNIT_IN(ch)) && (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_INVISIBLE) || CHAR_HAS_FLAG(ch, CHAR_DETECT_INVISIBLE)))))

#define CHAR_CAN_GO(ch, door)                                                                                                              \
   (ROOM_EXIT(UNIT_IN(ch), door) && (ROOM_EXIT(UNIT_IN(ch), door)->to_room) && !IS_SET(ROOM_EXIT(UNIT_IN(ch), door)->exit_info, EX_CLOSED))

/* ..................................................................... */

#define PC_ACCESS_LEVEL(pc) (UPC(pc)->nAccessLevel)

#define PC_CRACK_ATTEMPTS(pc) (UPC(pc)->crack_attempts)

#define PC_SETUP(pc) (UPC(pc)->setup)

#define PC_LIFESPAN(pc) (UPC(pc)->lifespan)

#define PC_ACCOUNT(pc) (UPC(pc)->account)

#define PC_VIRTUAL_LEVEL(pc) (UPC(pc)->vlvl)

#define PC_INFO(pc) (UPC(pc)->info)

#define PC_SETUP_ECHO(pc) (UPC(pc)->setup.echo)

#define PC_SETUP_REDRAW(pc) (UPC(pc)->setup.redraw)

#define PC_SETUP_WIDTH(pc) (UPC(pc)->setup.width)

#define PC_SETUP_HEIGHT(pc) (UPC(pc)->setup.height)

#define PC_SETUP_TELNET(pc) (UPC(pc)->setup.telnet)

#define PC_SETUP_EMULATION(pc) (UPC(pc)->setup.emulation)

#define PC_SETUP_COLOUR(pc) (UPC(pc)->setup.colour_convert)

#define PC_GUILD(pc) (UPC(pc)->guild)

#define PC_GUILD_TIME(pc) (UPC(pc)->guild_time)

#define PC_QUEST(pc) (UPC(pc)->quest)

#define PC_COND(ch, i) (UPC(ch)->conditions[i])

#define PC_ABILITY_POINTS(ch) (UPC(ch)->ability_points)

#define PC_SKILL_POINTS(ch) (UPC(ch)->skill_points)

#define PC_ABI_LVL(ch, index) (UPC(ch)->ability_lvl[index])

#define PC_ABI_COST(ch, index) (UPC(ch)->ability_cost[index])

#define PC_SKI_SKILL(ch, index) (UPC(ch)->skills[index])

#define PC_SKI_LVL(ch, index) (UPC(ch)->skill_lvl[index])

#define PC_SKI_COST(ch, index) (UPC(ch)->skill_cost[index])

#define PC_WPN_LVL(ch, index) (UPC(ch)->weapon_lvl[index])

#define PC_WPN_COST(ch, index) (UPC(ch)->weapon_cost[index])

#define PC_WPN_SKILL(ch, index) (UPC(ch)->weapons[index])

#define PC_SPL_LVL(ch, index) (UPC(ch)->spell_lvl[index])

#define PC_SPL_COST(ch, index) (UPC(ch)->spell_cost[index])

#define PC_SPL_SKILL(ch, index) (UPC(ch)->spells[index])

#define PC_CRIMES(unit) (UPC(unit)->nr_of_crimes)

#define PC_PWD(unit) (UPC(unit)->pwd)

#define PC_LASTHOST(unit) (UPC(unit)->lasthosts)

#define PC_FILENAME(unit) (UPC(unit)->filename)

#define PC_ID(unit) (UPC(unit)->id)

#define PC_BANK(unit) (UPC(unit)->bank)

#define PC_FLAGS(unit) (UPC(unit)->flags)

#define PC_TIME(unit) (UPC(unit)->time)

#define PC_HOME(ch) (UPC(ch)->hometown)
/* .................... PC SUPER STRUCTURE ............................. */

#define PC_IMMORTAL(ch) (IS_PC(ch) && CHAR_LEVEL(ch) >= 200)

#define PC_MORTAL(ch) (IS_PC(ch) && CHAR_LEVEL(ch) < 200)

#define PC_IS_UNSAVED(ch) (PC_TIME(ch).played == 0)

/* ..................................................................... */

#define NPC_WPN_SKILL(ch, index) (UNPC(ch)->weapons[index])

#define NPC_SPL_SKILL(ch, index) (UNPC(ch)->spells[index])

#define NPC_DEFAULT(unit) (UNPC(unit)->default_pos)

#define NPC_FLAGS(unit) (UNPC(unit)->flags)

/* ..................................................................... */

#define UNIT_TITLE_STRING(unit) (UNIT_TITLE(unit).String())

//#define UNIT_OUT_DESCR_STRING(unit) (UNIT_OUT_DESCR(unit).String())
inline auto UNIT_OUT_DESCR_STRING(unit_data *unit) -> const char *
{
   return UNIT_OUT_DESCR(unit).String();
}
#define UNIT_IN_DESCR_STRING(unit) (UNIT_IN_DESCR(unit).String())

#define TITLENAME(unit) (IS_PC(unit) ? UNIT_NAME(unit) : UNIT_TITLE_STRING(unit))

#define SOMETON(unit) ((UNIT_SEX(unit) == SEX_NEUTRAL) ? "something" : "someone")

/* Title, Name or Someone/Something */
#define UNIT_SEE_TITLE(ch, unit) (CHAR_CAN_SEE(ch, unit) ? TITLENAME(unit) : SOMETON(unit))

#define UNIT_SEE_NAME(ch, unit) (CHAR_CAN_SEE(ch, unit) ? UNIT_NAME(unit) : SOMETON(unit))

/* ..................................................................... */

/* Invis people aren't supposed to have sex... /gnort */

#define B_HSHR(ch) ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "its" : ((CHAR_SEX(ch) == SEX_MALE) ? "his" : "her"))

#define HSHR(to, ch) (CHAR_CAN_SEE((to), (ch)) ? B_HSHR(ch) : "their")

#define B_HESH(ch) ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "it" : ((CHAR_SEX(ch) == SEX_MALE) ? "he" : "she"))

#define HESH(to, ch) (CHAR_CAN_SEE((to), (ch)) ? B_HESH(ch) : "they")

#define B_HMHR(ch) ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "it" : ((CHAR_SEX(ch) == SEX_MALE) ? "him" : "her"))

#define HMHR(to, ch) (CHAR_CAN_SEE((to), (ch)) ? B_HMHR(ch) : "them")

#define UNIT_ANA(unit) ANA(*UNIT_NAME(unit))
