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

#ifndef _MUD_INTERPRETER_H
#define _MUD_INTERPRETER_H

#include "dil.h"

int  char_is_playing(std::shared_ptr<unit_data> u);
int  descriptor_is_playing(struct descriptor_data *d);
void set_descriptor_fptr(struct descriptor_data *d, void (*fptr)(struct descriptor_data *, char *), ubit1 call);
void descriptor_interpreter(struct descriptor_data *d, char *arg);
void interpreter_string_add(struct descriptor_data *d, char *str);

struct spec_arg
{
   std::shared_ptr<unit_data> owner;     /* Who is this?                       */
   std::shared_ptr<unit_data> activator; /* Who performed the operation        */
   std::shared_ptr<unit_data> medium;    /* Possibly what is used in operation */
   std::shared_ptr<unit_data> target;    /* Possible target of operation       */

   struct command_info *cmd;
   struct unit_fptr    *fptr; /* The fptr is allowed to be modified, destroyed */

   int *pInt; /* Potential int to modify */

   const char *arg;
   ubit16      mflags; /* Would like to make constant, but then can't define.. */
};

struct command_info
{
   ubit8 combat_speed;  /* The speed of a combat command         */
   ubit8 combat_buffer; /* Use the combat speed / buffer system? */

   const char *cmd_str;

   sbit32 no;

   ubit8 minimum_position;

   void (*cmd_fptr)(std::shared_ptr<unit_data> ch, char *arg, const struct command_info *c);

   ubit8 minimum_level;
   ubit8 log_level; /* For logging certain immortal commands */

   struct diltemplate *tmpl; /* Perhaps a DIL template...         */

   /* Also put noise/provokation and impact values here            */

   /* Example: if ((cmd->noise > 50) || (cmd->impact > 7))         */
   /*          { wake(dragon); .... }                              */
   /* Example: if (cmd->provokation < 0) { do_kiss(maid, ch); .. } */
};

/* Bitmasks to determine what kind of messages is to be send
   to a special function. */

struct unit_function_array_type
{
   const char *name;
   int (*func)(struct spec_arg *sarg);
   int    save_w_d; /* May it be saved if it has data? True/false */
   ubit16 sfb;      /* what kind of messages should be send */
   sbit16 tick;     /* Default tick count */
};

/* To check for commands by string */
ubit1 is_command(const struct command_info *cmd, const char *str);

/* Check to see if typed command is abbreviated */
ubit1 cmd_is_abbrev(std::shared_ptr<unit_data> ch, const struct command_info *cmd);

/* Interpreter routines */
void wrong_position(std::shared_ptr<unit_data> ch);
void command_interpreter(std::shared_ptr<unit_data> ch, char *argument);
void argument_interpreter(const char *argument, char *first_arg, char *second_arg);
void half_chop(char *string, char *arg1, char *arg2);

/* The routine to check for special routines */

int unit_function_scan(std::shared_ptr<unit_data> u, struct spec_arg *sarg);
int function_activate(std::shared_ptr<unit_data> u, struct spec_arg *sarg);
#ifdef DMSERVER
int basic_special(std::shared_ptr<unit_data> ch, struct spec_arg *sarg, ubit16 mflt, std::shared_ptr<unit_data> extra_target = NULL);
#endif
int  send_preprocess(std::shared_ptr<unit_data> ch, const struct command_info *cmd, char *arg);
void send_done(std::shared_ptr<unit_data> activator,
               std::shared_ptr<unit_data> medium,
               std::shared_ptr<unit_data> target,
               int                        i,
               const struct command_info *cmd,
               const char                *arg,
               std::shared_ptr<unit_data> extra_target = NULL);
int  send_ack(std::shared_ptr<unit_data> activator,
              std::shared_ptr<unit_data> medium,
              std::shared_ptr<unit_data> target,
              int                       *i,
              const struct command_info *cmd,
              const char                *arg,
              std::shared_ptr<unit_data> extra_target);
int  send_message(std::shared_ptr<unit_data> ch, char *arg);
int  send_death(std::shared_ptr<unit_data> ch);
int  send_combat(std::shared_ptr<unit_data> ch);
int  send_save_to(std::shared_ptr<unit_data> from, std::shared_ptr<unit_data> to);

#include "spec_assign.h"

void do_ignore(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_reply(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_expert(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_manifest(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_sacrifice(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_pray(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_exit(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_look(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_read(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_say(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_snoop(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_delete(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_insult(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_quit(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_help(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_who(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_emote(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_echo(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_trans(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_reset(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_kill(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_stand(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_sit(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_rest(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_sleep(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wake(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_force(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_get(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_drop(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_news(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_score(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_guild(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_status(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_inventory(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_equipment(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_shout(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_not_here(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_tell(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wear(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wield(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_grab(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_remove(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_put(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_shutdown(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_reboot(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_execute(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_save(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_hit(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_set(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_setskill(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_give(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wstat(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_setskill(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_time(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_weather(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_load(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_purge(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_ideatypobug(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_whisper(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_cast(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_at(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_goto(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_ask(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_drink(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_eat(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_pour(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_sip(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_taste(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_order(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_follow(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_rent(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_offer(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_advance(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_close(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_open(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_lock(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_unlock(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_exits(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_enter(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_leave(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_write(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_flee(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_sneak(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_hide(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_backstab(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_pick(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_steal(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_bash(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_rescue(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_kick(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_search(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_examine(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_info(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_users(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_where(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_level(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_reroll(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_brief(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wizlist(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_consider(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_group(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_restore(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_ban(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_switch(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_quaff(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_recite(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_use(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_pose(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_noshout(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_change(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_prompt(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_echosay(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_notell(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wizhelp(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_credits(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_compact(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_dig(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_bury(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_turn(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_diagnose(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_appraise(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_ventriloquate(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_aid(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_light(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_extinguish(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wimpy(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_peaceful(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wizinv(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_drag(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_path(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_freeze(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_file(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_knock(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_message(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_broadcast(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wiz(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_title(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_split(std::shared_ptr<unit_data> , char *, const struct command_info *);

void do_crash(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_wizlock(std::shared_ptr<unit_data> , char *, const struct command_info *);

void do_practice(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_move(std::shared_ptr<unit_data> , char *, const struct command_info *);

void do_ride(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_sail(std::shared_ptr<unit_data> , char *, const struct command_info *);

void do_quests(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_decapitate(std::shared_ptr<unit_data> , char *, const struct command_info *);

void do_purse(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_makemoney(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_verify(std::shared_ptr<unit_data> , char *, const struct command_info *);

void do_commands(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_socials(std::shared_ptr<unit_data> , char *, const struct command_info *);

void do_boards(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_reimb(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_finger(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_account(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_kickit(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_corpses(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_inform(std::shared_ptr<unit_data> , char *, const struct command_info *);
void do_areas(std::shared_ptr<unit_data> , char *, const struct command_info *);

#endif /* _MUD_INTERPRETER_H */
