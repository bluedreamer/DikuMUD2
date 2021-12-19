#pragma once
/* *********************************************************************** *
 * File   : interpreter.h                             Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : ???                                                            *
 *                                                                         *
 * Purpose: Prototypes and types for the command interpreter module.       *
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

#include "command_info.h"
#include "descriptor_data.h"
#include "dil.h"
#include "essential.h"

auto char_is_playing(unit_data *u) -> int;
auto descriptor_is_playing(descriptor_data *d) -> int;
void descriptor_interpreter(descriptor_data *d, const char *arg);
void interpreter_string_add(descriptor_data *d, const char *str);
void add_func_history(unit_data *u, uint16_t idx, uint16_t flags);

/* Bitmasks to determine what kind of messages is to be send
   to a special function. */

/* To check for commands by string */
auto is_command(const command_info *cmd, const char *str) -> bool;

/* Check to see if typed command is abbreviated */
auto cmd_is_abbrev(unit_data *ch, const command_info *cmd) -> bool;

/* Interpreter routines */
void wrong_position(unit_data *ch);
void command_interpreter(unit_data *ch, const char *arg);
void argument_interpreter(const char *argument, char *first_arg, char *second_arg);
void half_chop(char *string, char *arg1, char *arg2);

/* The routine to check for special routines */

auto unit_function_scan(unit_data *u, struct spec_arg *sarg) -> int;
auto function_activate(unit_data *u, struct spec_arg *sarg) -> int;
#ifdef DMSERVER
auto basic_special(unit_data *ch, struct spec_arg *sarg, uint16_t mflt, unit_data *extra_target = nullptr) -> int;
#endif
auto send_preprocess(unit_data *ch, const command_info *cmd, const char *arg) -> int;
void send_done(unit_data          *activator,
               unit_data          *medium,
               unit_data          *target,
               int                 i,
               const command_info *cmd,
               const char         *arg,
               unit_data          *extra_target = nullptr);
auto send_ack(
   unit_data *activator, unit_data *medium, unit_data *target, int *i, const command_info *cmd, const char *arg, unit_data *extra_target)
   -> int;
auto send_message(unit_data *ch, const char *arg) -> int;
auto send_death(unit_data *ch) -> int;
auto send_combat(unit_data *ch) -> int;
auto send_save_to(unit_data *from, unit_data *to) -> int;

#include "spec_assign.h"

void do_ignore(unit_data *, char *, const command_info *);
void do_reply(unit_data *, char *, const command_info *);
void do_expert(unit_data *, char *, const command_info *);
void do_manifest(unit_data *, char *, const command_info *);
void do_sacrifice(unit_data *, char *, const command_info *);
void do_pray(unit_data *, char *, const command_info *);
void do_exit(unit_data *, char *, const command_info *);
void do_look(unit_data *, char *, const command_info *);
void do_read(unit_data *, char *, const command_info *);
void do_say(unit_data *, char *, const command_info *);
void do_snoop(unit_data *, char *, const command_info *);
void do_delete(unit_data *, char *, const command_info *);
void do_insult(unit_data *, char *, const command_info *);
void do_quit(unit_data *, char *, const command_info *);
void do_help(unit_data *, char *, const command_info *);
void do_who(unit_data *, char *, const command_info *);
void do_emote(unit_data *, char *, const command_info *);
void do_echo(unit_data *, char *, const command_info *);
void do_trans(unit_data *, char *, const command_info *);
void do_reset(unit_data *, char *, const command_info *);
void do_kill(unit_data *, char *, const command_info *);
void do_stand(unit_data *, char *, const command_info *);
void do_sit(unit_data *, char *, const command_info *);
void do_rest(unit_data *, char *, const command_info *);
void do_sleep(unit_data *, char *, const command_info *);
void do_wake(unit_data *, char *, const command_info *);
void do_force(unit_data *, char *, const command_info *);
void do_get(unit_data *, char *, const command_info *);
void do_drop(unit_data *, char *, const command_info *);
void do_news(unit_data *, char *, const command_info *);
void do_score(unit_data *, char *, const command_info *);
void do_guild(unit_data *, char *, const command_info *);
void do_status(unit_data *, char *, const command_info *);
void do_inventory(unit_data *, char *, const command_info *);
void do_equipment(unit_data *, char *, const command_info *);
void do_shout(unit_data *, char *, const command_info *);
void do_not_here(unit_data *, char *, const command_info *);
void do_tell(unit_data *, char *, const command_info *);
void do_wear(unit_data *, char *, const command_info *);
void do_wield(unit_data *, char *, const command_info *);
void do_grab(unit_data *, char *, const command_info *);
void do_remove(unit_data *, char *, const command_info *);
void do_put(unit_data *, char *, const command_info *);
void do_shutdown(unit_data *, char *, const command_info *);
void do_reboot(unit_data *, char *, const command_info *);
void do_execute(unit_data *, char *, const command_info *);
void do_save(unit_data *, char *, const command_info *);
void do_hit(unit_data *, char *, const command_info *);
void do_set(unit_data *, char *, const command_info *);
void do_setskill(unit_data *, char *, const command_info *);
void do_give(unit_data *, char *, const command_info *);
void do_wstat(unit_data *, char *, const command_info *);

void do_time(unit_data *, char *, const command_info *);
void do_weather(unit_data *, char *, const command_info *);
void do_load(unit_data *, char *, const command_info *);
void do_purge(unit_data *, char *, const command_info *);
void do_ideatypobug(unit_data *, char *, const command_info *);
void do_whisper(unit_data *, char *, const command_info *);
void do_cast(unit_data *, char *, const command_info *);
void do_at(unit_data *, char *, const command_info *);
void do_goto(unit_data *, char *, const command_info *);
void do_ask(unit_data *, char *, const command_info *);
void do_drink(unit_data *, char *, const command_info *);
void do_eat(unit_data *, char *, const command_info *);
void do_pour(unit_data *, char *, const command_info *);
void do_sip(unit_data *, char *, const command_info *);
void do_taste(unit_data *, char *, const command_info *);
void do_order(unit_data *, char *, const command_info *);
void do_follow(unit_data *, char *, const command_info *);
void do_rent(unit_data *, char *, const command_info *);
void do_offer(unit_data *, char *, const command_info *);
void do_advance(unit_data *, char *, const command_info *);
void do_close(unit_data *, char *, const command_info *);
void do_open(unit_data *, char *, const command_info *);
void do_lock(unit_data *, char *, const command_info *);
void do_unlock(unit_data *, char *, const command_info *);
void do_exits(unit_data *, char *, const command_info *);
void do_enter(unit_data *, char *, const command_info *);
void do_leave(unit_data *, char *, const command_info *);
void do_write(unit_data *, char *, const command_info *);
void do_flee(unit_data *, char *, const command_info *);
void do_sneak(unit_data *, char *, const command_info *);
void do_hide(unit_data *, char *, const command_info *);
void do_backstab(unit_data *, char *, const command_info *);
void do_pick(unit_data *, char *, const command_info *);
void do_steal(unit_data *, char *, const command_info *);
void do_bash(unit_data *, char *, const command_info *);
void do_rescue(unit_data *, char *, const command_info *);
void do_kick(unit_data *, char *, const command_info *);
void do_search(unit_data *, char *, const command_info *);
void do_examine(unit_data *, char *, const command_info *);
void do_info(unit_data *, char *, const command_info *);
void do_users(unit_data *, char *, const command_info *);
void do_where(unit_data *, char *, const command_info *);
void do_level(unit_data *, char *, const command_info *);
void do_reroll(unit_data *, char *, const command_info *);
void do_brief(unit_data *, char *, const command_info *);
void do_wizlist(unit_data *, char *, const command_info *);
void do_consider(unit_data *, char *, const command_info *);
void do_group(unit_data *, char *, const command_info *);
void do_restore(unit_data *, char *, const command_info *);
void do_ban(unit_data *, char *, const command_info *);
void do_switch(unit_data *, char *, const command_info *);
void do_quaff(unit_data *, char *, const command_info *);
void do_recite(unit_data *, char *, const command_info *);
void do_use(unit_data *, char *, const command_info *);
void do_pose(unit_data *, char *, const command_info *);
void do_noshout(unit_data *, char *, const command_info *);
void do_change(unit_data *, char *, const command_info *);
void do_prompt(unit_data *, char *, const command_info *);
void do_echosay(unit_data *, char *, const command_info *);
void do_notell(unit_data *, char *, const command_info *);
void do_wizhelp(unit_data *, char *, const command_info *);
void do_credits(unit_data *, char *, const command_info *);
void do_compact(unit_data *, char *, const command_info *);
void do_dig(unit_data *, char *, const command_info *);
void do_bury(unit_data *, char *, const command_info *);
void do_turn(unit_data *, char *, const command_info *);
void do_diagnose(unit_data *, char *, const command_info *);
void do_appraise(unit_data *, char *, const command_info *);
void do_ventriloquate(unit_data *, char *, const command_info *);
void do_aid(unit_data *, char *, const command_info *);
void do_light(unit_data *, char *, const command_info *);
void do_extinguish(unit_data *, char *, const command_info *);
void do_wimpy(unit_data *, char *, const command_info *);
void do_peaceful(unit_data *, char *, const command_info *);
void do_wizinv(unit_data *, char *, const command_info *);
void do_drag(unit_data *, char *, const command_info *);
void do_path(unit_data *, char *, const command_info *);
void do_freeze(unit_data *, char *, const command_info *);
void do_file(unit_data *, char *, const command_info *);
void do_knock(unit_data *, char *, const command_info *);
void do_message(unit_data *, char *, const command_info *);
void do_broadcast(unit_data *, char *, const command_info *);
void do_wiz(unit_data *, char *, const command_info *);
void do_title(unit_data *, char *, const command_info *);
void do_split(unit_data *, char *, const command_info *);

void do_crash(unit_data *, char *, const command_info *);
void do_wizlock(unit_data *, char *, const command_info *);

void do_practice(unit_data *, char *, const command_info *);
void do_move(unit_data *, char *, const command_info *);

void do_ride(unit_data *, char *, const command_info *);
void do_sail(unit_data *, char *, const command_info *);

void do_quests(unit_data *, char *, const command_info *);
void do_decapitate(unit_data *, char *, const command_info *);

void do_purse(unit_data *, char *, const command_info *);
void do_makemoney(unit_data *, char *, const command_info *);
void do_verify(unit_data *, char *, const command_info *);

void do_commands(unit_data *, char *, const command_info *);
void do_socials(unit_data *, char *, const command_info *);

void do_boards(unit_data *, char *, const command_info *);
void do_reimb(unit_data *, char *, const command_info *);
void do_finger(unit_data *, char *, const command_info *);
void do_account(unit_data *, char *, const command_info *);
void do_kickit(unit_data *, char *, const command_info *);
void do_corpses(unit_data *, char *, const command_info *);
void do_inform(unit_data *, char *, const command_info *);
void do_areas(unit_data *, char *, const command_info *);
