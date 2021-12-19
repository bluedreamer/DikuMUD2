#pragma once
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

#include "spec_arg.h"

auto accuse(spec_arg *sarg) -> int;
auto admin_obj(spec_arg *sarg) -> int;
auto aggres_rev_align(spec_arg *sarg) -> int;
auto aggressive(spec_arg *sarg) -> int;
auto ball(spec_arg *sarg) -> int;
auto bank(spec_arg *sarg) -> int;
auto blow_away(spec_arg *sarg) -> int;
auto board(spec_arg *sarg) -> int;
auto breath_weapon(spec_arg *sarg) -> int;
auto change_hometown(spec_arg *sarg) -> int;
auto chaos_daemon(spec_arg *sarg) -> int;
auto charm_of_death(spec_arg *sarg) -> int;
auto climb(spec_arg *sarg) -> int;
auto combat_magic(spec_arg *sarg) -> int;
auto combat_magic_heal(spec_arg *sarg) -> int;
auto combat_poison_sting(spec_arg *sarg) -> int;
auto competition_board(spec_arg *sarg) -> int;
auto dead_only(spec_arg *sarg) -> int;
auto death_obj(spec_arg *sarg) -> int;
auto death_room(spec_arg *sarg) -> int;
auto demi_stuff(spec_arg *sarg) -> int;
auto dictionary(spec_arg *sarg) -> int;
auto dil_direct_init(spec_arg *sarg) -> int;
auto dil_init(spec_arg *sarg) -> int;
auto dump(spec_arg *sarg) -> int;
auto eat_and_delete(spec_arg *sarg) -> int;
auto error_rod(spec_arg *sarg) -> int;
auto evaluate(spec_arg *sarg) -> int;
auto fido(spec_arg *sarg) -> int;
auto force_move(spec_arg *sarg) -> int;
auto frozen(spec_arg *sarg) -> int;
auto green_tuborg(spec_arg *sarg) -> int;
auto guard_door(spec_arg *sarg) -> int;
auto guard_guild_way(spec_arg *sarg) -> int;
auto guard_unit(spec_arg *sarg) -> int;
auto guard_way(spec_arg *sarg) -> int;
auto guard_way_level(spec_arg *sarg) -> int;
auto guild_basis(spec_arg *sarg) -> int;
auto guild_master(spec_arg *sarg) -> int;
auto guild_master_init(spec_arg *sarg) -> int;
auto guild_title(spec_arg *sarg) -> int;
auto hideaway(spec_arg *sarg) -> int;
auto hunting(spec_arg *sarg) -> int;
auto info_rod(spec_arg *sarg) -> int;
auto janitor(spec_arg *sarg) -> int;
auto justice_scales(spec_arg *sarg) -> int;
auto link_dead(spec_arg *sarg) -> int;
auto log_object(spec_arg *sarg) -> int;
auto mercenary_hire(spec_arg *sarg) -> int;
auto mercenary_hunt(spec_arg *sarg) -> int;
auto mob_command(spec_arg *sarg) -> int;
auto npc_visit_room(spec_arg *sarg) -> int;
auto obey(spec_arg *sarg) -> int;
auto obey_animal(spec_arg *sarg) -> int;
auto obj_evil(spec_arg *sarg) -> int;
auto obj_good(spec_arg *sarg) -> int;
auto obj_guild(spec_arg *sarg) -> int;
auto obj_quest(spec_arg *sarg) -> int;
auto odin_statue(spec_arg *sarg) -> int;
auto oracle(spec_arg *sarg) -> int;
auto pain_exec(spec_arg *sarg) -> int;
auto pain_init(spec_arg *sarg) -> int;
auto persist_init(spec_arg *sarg) -> int;
auto persist_intern(spec_arg *sarg) -> int;
auto postman(spec_arg *sarg) -> int;
auto protect_lawful(spec_arg *sarg) -> int;
auto random_move(spec_arg *sarg) -> int;
auto random_zonemove(spec_arg *sarg) -> int;
auto recep_daemon(spec_arg *sarg) -> int;
auto rescue(spec_arg *sarg) -> int;
auto restrict_obj(spec_arg *sarg) -> int;
auto return_to_origin(spec_arg *sarg) -> int;
auto reward_board(spec_arg *sarg) -> int;
auto reward_give(spec_arg *sarg) -> int;
auto run_dil(spec_arg *sarg) -> int;
auto sacrifice(spec_arg *sarg) -> int;
auto scavenger(spec_arg *sarg) -> int;
auto shop_init(spec_arg *sarg) -> int;
auto shop_keeper(spec_arg *sarg) -> int;
auto slime_obj(spec_arg *sarg) -> int;
auto spec_unused(spec_arg *sarg) -> int;
auto spider_room_attack(spec_arg *sarg) -> int;
auto teach_init(spec_arg *sarg) -> int;
auto teach_members_only(spec_arg *sarg) -> int;
auto teaching(spec_arg *sarg) -> int;
auto teamwork(spec_arg *sarg) -> int;
auto whistle(spec_arg *sarg) -> int;
auto ww_block_attack(spec_arg *sarg) -> int;
auto ww_block_river(spec_arg *sarg) -> int;
auto ww_earth_blood(spec_arg *sarg) -> int;
auto ww_pool(spec_arg *sarg) -> int;
auto ww_quest_done(spec_arg *sarg) -> int;
auto ww_raver(spec_arg *sarg) -> int;
auto ww_stone_render(spec_arg *sarg) -> int;
auto ww_vat_machine(spec_arg *sarg) -> int;
