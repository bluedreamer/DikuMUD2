#pragma once
/* *********************************************************************** *
 * File   : magic.h                                   Part of Valhalla MUD *
 * Version: 1.03                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for magic.c                                             *
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

/* #define MAGIC_POWER(ch)  \
   std::max(CHAR_MAG(ch), CHAR_DIV(ch)) */
#include "spell_args.h"
#include "unit_data.h"

auto dil_effect(const char *pStr, spell_args *sa) -> int;
auto object_power(unit_data *unit) -> int;
auto use_mana(unit_data *medium, int mana) -> bool;
auto cast_magic_now(unit_data *ch, int mana) -> bool;

auto may_teleport_away(unit_data *unit) -> bool;
auto may_teleport_to(unit_data *unit, unit_data *dest) -> bool;
auto may_teleport(unit_data *unit, unit_data *dest) -> bool;

auto spell_cast_check(unit_data *att, int spell) -> int;
auto spell_resistance(unit_data *att, unit_data *def, int spell) -> int;
auto spell_ability(unit_data *u, int ability, int spell) -> int;
auto spell_attack_ability(unit_data *medium, int spell) -> int;
auto spell_attack_skill(unit_data *unit, int spell) -> int;
auto spell_defense_skill(unit_data *unit, int spell) -> int;

auto spell_offensive(spell_args *sa, int spell_number, int bonus = 0) -> int;

auto variation(int n, int d, int u) -> int;

/* These are all the spell definitions... */

void spell_bless(spell_args *sa);
void spell_curse(spell_args *sa);
void spell_remove_curse(spell_args *sa);
void spell_cure_wounds_1(spell_args *sa);
void spell_cure_wounds_2(spell_args *sa);
void spell_cure_wounds_3(spell_args *sa);
void spell_cause_wounds_1(spell_args *sa);
void spell_cause_wounds_2(spell_args *sa);
void spell_cause_wounds_3(spell_args *sa);
void spell_dispel_evil(spell_args *sa);
void spell_dispel_good(spell_args *sa);
void spell_repel_undead_1(spell_args *sa);
void spell_repel_undead_2(spell_args *sa);
void spell_blind(spell_args *sa);
void spell_cure_blind(spell_args *sa);
void spell_locate_object(spell_args *sa);
void spell_locate_char(spell_args *sa);
void spell_raise_mag(spell_args *sa);
void spell_raise_div(spell_args *sa);
void spell_raise_str(spell_args *sa);
void spell_raise_dex(spell_args *sa);
void spell_raise_con(spell_args *sa);
void spell_raise_cha(spell_args *sa);
void spell_raise_bra(spell_args *sa);
void spell_raise_tgh(spell_args *sa);
void spell_raise_divine(spell_args *sa);
void spell_raise_protection(spell_args *sa);
void spell_raise_detection(spell_args *sa);
void spell_raise_summoning(spell_args *sa);
void spell_raise_creation(spell_args *sa);
void spell_raise_mind(spell_args *sa);
void spell_raise_heat(spell_args *sa);
void spell_raise_cold(spell_args *sa);
void spell_raise_cell(spell_args *sa);
void spell_raise_internal(spell_args *sa);
void spell_raise_external(spell_args *sa);
void spell_pro_evil(spell_args *sa);
void spell_pro_good(spell_args *sa);
void spell_sanctuary(spell_args *sa);
void spell_dispel_magic(spell_args *sa);
void spell_sustain(spell_args *sa);
void spell_lock(spell_args *sa);
void spell_unlock(spell_args *sa);
void spell_magic_lock(spell_args *sa);
void spell_magic_unlock(spell_args *sa);
void spell_great_knock(spell_args *sa);
void spell_det_align(spell_args *sa);
void spell_det_invisible(spell_args *sa);
void spell_det_magic(spell_args *sa);
void spell_det_poison(spell_args *sa);
void spell_det_undead(spell_args *sa);
void spell_det_curse(spell_args *sa);
void spell_sense_life(spell_args *sa);
void spell_identify_1(spell_args *sa);
void spell_identify_2(spell_args *sa);
void spell_random_teleport(spell_args *sa);
void spell_clear_skies(spell_args *sa);
void spell_storm_call(spell_args *sa);
void spell_control_teleport(spell_args *sa);
void spell_summon_char_1(spell_args *sa);
void spell_summon_char_2(spell_args *sa);
void spell_create_food(spell_args *sa);
void spell_create_water(spell_args *sa);
void spell_light_1(spell_args *sa);
void spell_light_2(spell_args *sa);
void spell_darkness_1(spell_args *sa);
void spell_darkness_2(spell_args *sa);
void spell_enchant_weapon(spell_args *sa);
void spell_enchant_armour(spell_args *sa);
void spell_animate_dead(spell_args *sa);
void spell_heroism(spell_args *sa);
void spell_armour_fitting(spell_args *sa);
void spell_control_undead(spell_args *sa);
void spell_absorbtion(spell_args *sa);
void spell_permanency(spell_args *sa);
void spell_clone(spell_args *sa);
void spell_colourspray_1(spell_args *sa);
void spell_colourspray_2(spell_args *sa);
void spell_colourspray_3(spell_args *sa);
void spell_invisibility(spell_args *sa);
void spell_wizard_eye(spell_args *sa);
void spell_fear(spell_args *sa);
void spell_confusion(spell_args *sa);
void spell_xray_vision(spell_args *sa);
void spell_calm(spell_args *sa);
void spell_hold(spell_args *sa);
void spell_command(spell_args *sa);
void spell_charm(spell_args *sa);
void spell_fireball_1(spell_args *sa);
void spell_fireball_2(spell_args *sa);
void spell_fireball_3(spell_args *sa);
void spell_frostball_1(spell_args *sa);
void spell_frostball_2(spell_args *sa);
void spell_frostball_3(spell_args *sa);
void spell_lightning_1(spell_args *sa);
void spell_lightning_2(spell_args *sa);
void spell_lightning_3(spell_args *sa);
void spell_stinking_cloud_1(spell_args *sa);
void spell_stinking_cloud_2(spell_args *sa);
void spell_stinking_cloud_3(spell_args *sa);
void spell_poison(spell_args *sa);
void spell_remove_poison(spell_args *sa);
void spell_disease_1(spell_args *sa);
void spell_disease_2(spell_args *sa);
void spell_rem_disease(spell_args *sa);
void spell_acidball_1(spell_args *sa);
void spell_acidball_2(spell_args *sa);
void spell_acidball_3(spell_args *sa);
void spell_mana_boost(spell_args *sa);
void spell_find_path(spell_args *sa);
void spell_transport(spell_args *sa);
void spell_undead_door(spell_args *sa);
