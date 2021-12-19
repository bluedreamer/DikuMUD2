/* *********************************************************************** *
 * File   : spec_assign.h                             Part of Valhalla MUD *
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
#ifndef _MUD_SPEC_ASS_H
#define _MUD_SPEC_ASS_H

#include "interpreter.h"

auto persist_intern(struct spec_arg *sarg) -> int;
auto persist_init(struct spec_arg *sarg) -> int;

auto competition_board(struct spec_arg *sarg) -> int;
auto dil_init(struct spec_arg *sarg) -> int;
auto dil_direct_init(struct spec_arg *sarg) -> int;
auto dead_only(struct spec_arg *sarg) -> int;
auto death_obj(struct spec_arg *sarg) -> int;
auto board(struct spec_arg *sarg) -> int;
auto mob_command(struct spec_arg *sarg) -> int;
auto hunting(struct spec_arg *sarg) -> int;
auto postman(struct spec_arg *sarg) -> int;
auto shop_keeper(struct spec_arg *sarg) -> int;
auto eat_and_delete(struct spec_arg *sarg) -> int;
auto accuse(struct spec_arg *sarg) -> int;
auto protect_lawful(struct spec_arg *sarg) -> int;
auto pain_init(struct spec_arg *sarg) -> int;
auto pain_exec(struct spec_arg *sarg) -> int;
auto npc_visit_room(struct spec_arg *sarg) -> int;
auto mercenary_hire(struct spec_arg *sarg) -> int;
auto mercenary_hunt(struct spec_arg *sarg) -> int;
auto bank(struct spec_arg *sarg) -> int;
auto dump(struct spec_arg *sarg) -> int;
auto fido(struct spec_arg *sarg) -> int;
auto janitor(struct spec_arg *sarg) -> int;
auto spec_unused(struct spec_arg *sarg) -> int;

auto teach_init(struct spec_arg *sarg) -> int;
auto teaching(struct spec_arg *sarg) -> int;

auto random_move(struct spec_arg *sarg) -> int;
auto random_zonemove(struct spec_arg *sarg) -> int;
auto scavenger(struct spec_arg *sarg) -> int;
auto aggressive(struct spec_arg *sarg) -> int;
auto aggres_rev_align(struct spec_arg *sarg) -> int;
auto combat_magic(struct spec_arg *sarg) -> int;
auto blow_away(struct spec_arg *sarg) -> int;
auto combat_poison_sting(struct spec_arg *sarg) -> int;
auto spider_room_attack(struct spec_arg *sarg) -> int;
auto combat_magic_heal(struct spec_arg *sarg) -> int;
auto guard_way(struct spec_arg *sarg) -> int;
auto guard_door(struct spec_arg *sarg) -> int;
auto guard_unit(struct spec_arg *sarg) -> int;
auto rescue(struct spec_arg *sarg) -> int;
auto teamwork(struct spec_arg *sarg) -> int;
auto hideaway(struct spec_arg *sarg) -> int;
auto shop_init(struct spec_arg *sarg) -> int;
auto charm_of_death(struct spec_arg *sarg) -> int;
auto dictionary(struct spec_arg *sarg) -> int;
auto log_object(struct spec_arg *sarg) -> int;
auto guard_way_level(struct spec_arg *sarg) -> int;
auto odin_statue(struct spec_arg *sarg) -> int;
auto force_move(struct spec_arg *sarg) -> int;
auto ww_pool(struct spec_arg *sarg) -> int;
auto ww_block_river(struct spec_arg *sarg) -> int;
auto ww_stone_render(struct spec_arg *sarg) -> int;
auto ww_earth_blood(struct spec_arg *sarg) -> int;
auto ww_raver(struct spec_arg *sarg) -> int;
auto ww_vat_machine(struct spec_arg *sarg) -> int;
auto ww_quest_done(struct spec_arg *sarg) -> int;
auto ww_block_attack(struct spec_arg *sarg) -> int;

auto obj_good(struct spec_arg *sarg) -> int;
auto obj_evil(struct spec_arg *sarg) -> int;
auto obj_quest(struct spec_arg *sarg) -> int;
auto obj_guild(struct spec_arg *sarg) -> int;

auto change_hometown(struct spec_arg *sarg) -> int;

auto guard_guild_way(struct spec_arg *sarg) -> int;
auto teach_members_only(struct spec_arg *sarg) -> int;
auto whistle(struct spec_arg *sarg) -> int;
auto guild_master(struct spec_arg *sarg) -> int;
auto guild_basis(struct spec_arg *sarg) -> int;
auto guild_title(struct spec_arg *sarg) -> int;

auto death_room(struct spec_arg *sarg) -> int;
auto breath_weapon(struct spec_arg *sarg) -> int;
auto green_tuborg(struct spec_arg *sarg) -> int;
auto reward_board(struct spec_arg *sarg) -> int;
auto reward_give(struct spec_arg *sarg) -> int;

auto recep_daemon(struct spec_arg *sarg) -> int;
auto chaos_daemon(struct spec_arg *sarg) -> int;

auto evaluate(struct spec_arg *sarg) -> int;

auto error_rod(struct spec_arg *sarg) -> int;
auto info_rod(struct spec_arg *sarg) -> int;
auto climb(struct spec_arg *sarg) -> int;
auto run_dil(struct spec_arg *sarg) -> int;
auto oracle(struct spec_arg *sarg) -> int;
auto admin_obj(struct spec_arg *sarg) -> int;
auto obey_animal(struct spec_arg *sarg) -> int;
auto obey(struct spec_arg *sarg) -> int;
auto ball(struct spec_arg *sarg) -> int;
auto slime_obj(struct spec_arg *sarg) -> int;
auto frozen(struct spec_arg *sarg) -> int;
auto return_to_origin(struct spec_arg *sarg) -> int;
auto guild_master_init(struct spec_arg *sarg) -> int;
auto restrict_obj(struct spec_arg *sarg) -> int;
auto demi_stuff(struct spec_arg *sarg) -> int;
auto link_dead(struct spec_arg *sarg) -> int;
auto sacrifice(struct spec_arg *sarg) -> int;

auto justice_scales(struct spec_arg *sarg) -> int;

#endif
