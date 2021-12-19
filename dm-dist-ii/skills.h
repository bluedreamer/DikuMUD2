#pragma once
/* *********************************************************************** *
 * File   : skills.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for skills.c                                            *
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

#include "unit_data.h"
#include "values.h"

#include "dice_type.h"
#include "skill_interval.h"

auto skill_text(const skill_interval *si, int skill) -> const char *;

/* ---------------- COMBAT MESSAGE SYSTEM -------------------- */

#define COM_MAX_MSGS 60

#define COM_MSG_DEAD  -1
#define COM_MSG_MISS  -2
#define COM_MSG_NODAM -3
#define COM_MSG_EBODY -4

/* ---------------- RACES -------------------- */

#define RACE_IS_HUMANOID(race) ((race) <= RACE_OTHER_HUMANOID)

#define CHAR_IS_MAMMAL(ch) RACE_IS_MAMMAL(CHAR_RACE(ch))

#define RACE_IS_MAMMAL(race) (((race) > RACE_OTHER_HUMANOID) && ((race) <= RACE_OTHER_MAMMAL))

#define RACE_IS_UNDEAD(race) (((race) > RACE_OTHER_CREATURE) && ((race) <= RACE_OTHER_UNDEAD))

#define CHAR_IS_HUMANOID(ch) RACE_IS_HUMANOID(CHAR_RACE(ch))

#define CHAR_IS_UNDEAD(ch) RACE_IS_UNDEAD(CHAR_RACE(ch))

#define DEMIGOD_LEVEL_XP (40000000)

auto        object_two_handed(unit_data *obj) -> int;
void        roll_description(unit_data *att, const char *text, int roll);
auto        open_ended_roll(int size, int end) -> int;
inline auto open100() -> int
{
   return open_ended_roll(100, 5);
}

auto resistance_skill_check(int att_skill1, int def_skill1, int att_skill2, int def_skill2) -> int;
auto resistance_level_check(int att_level, int def_level, int att_skill, int def_skill) -> int;
auto skill_duration(int howmuch) -> int;

auto weapon_fumble(unit_data *weapon, int roll) -> int;
auto chart_damage(int roll, struct damage_chart_element_type *element) -> int;
auto chart_size_damage(int roll, struct damage_chart_element_type *element, int lbs) -> int;
auto weapon_damage(int roll, int weapon_type, int armour_type) -> int;
auto natural_damage(int roll, int weapon_type, int armour_type, int lbs) -> int;

auto basic_char_tgh_absorb(unit_data *ch) -> int;
auto basic_armor_absorb(unit_data *armour, int att_type) -> int;
auto basic_char_absorb(unit_data *ch, unit_data *armor, int att_type) -> int;

auto basic_char_weapon_dam(unit_data *ch, unit_data *weapon) -> int;
auto basic_char_hand_dam(unit_data *ch) -> int;
auto char_weapon_dam(unit_data *ch, unit_data *weapon) -> int;
auto char_hand_dam(unit_data *ch) -> int;

auto relative_level(int l1, int l2) -> int;
auto weapon_defense_skill(unit_data *ch, int skill) -> int;
auto weapon_attack_skill(unit_data *ch, int skill) -> int;
auto hit_location(unit_data *att, unit_data *def) -> int;
auto effective_dex(unit_data *ch) -> int;

auto av_value(int abila, int abilb, int skilla, int skillb) -> int;
auto av_howmuch(int av) -> int;
auto av_makes(int av) -> int;
void check_fitting(unit_data *u);

extern const char *pc_races[PC_RACE_MAX + 1];
extern const char *pc_race_adverbs[PC_RACE_MAX + 1];

extern int8_t racial_ability[ABIL_TREE_MAX][PC_RACE_MAX];
extern int8_t racial_weapons[WPN_TREE_MAX][PC_RACE_MAX];
extern int8_t racial_skills[SKI_TREE_MAX][PC_RACE_MAX];
extern int8_t racial_spells[SPL_TREE_MAX][PC_RACE_MAX];

extern int hit_location_table[];

extern const char *wpn_text[WPN_TREE_MAX + 1];
extern const char *spl_text[SPL_TREE_MAX + 1];
extern const char *ski_text[SKI_TREE_MAX + 1];
extern const char *abil_text[ABIL_TREE_MAX + 1];
