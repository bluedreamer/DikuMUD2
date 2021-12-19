#pragma once
/* *********************************************************************** *
 * File   : movement.h                                Part of Valhalla MUD *
 * Version: 1.50                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for movement.                                           *
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

#include "dijkstra.h"
#include "interpreter.h"

#define ROOM_DOOR_NAME(room, dir) (ROOM_EXIT((room), (dir))->open_name.Name() ? ROOM_EXIT((room), (dir))->open_name.Name() : "UNDEFINED")

void backdoor(unit_data *ch, char *arg, const struct command_info *cmd);

auto low_find_door(unit_data *ch, char *doorstr, int err_msg, int check_hidden) -> int;

#define MOVE_GOAL   0 /* The NPC is now at it's destination         */
#define MOVE_CLOSER 1 /* The NPC was moved closer to destination    */
#define MOVE_DOOR   2 /* The NPC is working on a door/lock/room     */
#define MOVE_BUSY   3 /* The NPC is busy and can't do anything yet  */
#define MOVE_FAILED 4 /* It is currently impossible to reach desti. */
#define MOVE_DEAD   5 /* The NPC is dead, abort now!                */

#define DESTROY_ME 12

struct door_data
{
   unit_data       *thing;
   unit_data       *room;
   unit_data       *reverse; /* Reverse Room or Inside thing */
   file_index_type *key;
   uint8_t          direction; /* For rooms, which direction was picked? */
   uint8_t         *flags;
   uint8_t         *rev_flags;
   const char      *name;
};

struct visit_data
{
   int        state;
   unit_data *go_to;

   unit_data *start_room;
   unit_data *dest_room;

   /* Return DESTROY_ME to destroy moving function        */
   /*        SFR_SHARE to allow lower functions to handle */
   /*        SFR_SUPREME to not allow lower functions     */
   int (*what_now)(const unit_data *, struct visit_data *);

   void *data;
   int   non_tick_return; /* What to return upon non-ticks (SFR_...) */
};

void npc_set_visit(unit_data *npc, unit_data *dest_room, int what_now(const unit_data *, struct visit_data *), void *data,
                   int non_tick_return);

auto do_advanced_move(unit_data *ch, int direction, int following = static_cast<int>(FALSE)) -> int;
