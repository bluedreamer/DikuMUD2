#pragma once
/* *********************************************************************** *
 * File   : handler.h                                 Part of Valhalla MUD *
 * Version: 2.02                                                           *
 * Author : seifert@diku.dk and quinn@diku.dk                              *
 *                                                                         *
 * Purpose: Prototypes for handling units.                                 *
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

#include "descriptor_data.h"
#include "essential.h"
#include "unitfind.h"

auto unit_is_edited(unit_data *u) -> descriptor_data *;

void unit_messg(unit_data *ch, unit_data *unit, const char *type, const char *mesg_s, const char *mesg_o);

auto single_unit_messg(unit_data *unit, const char *type, const char *pSubStr, const char *mesg) -> const char *;

void szonelog(struct zone_type *zone, const char *fmt, ...);

/* From pcsave.c - I'm just tired of specifying them everywhere */
void save_player(unit_data *pc);
auto load_player(const char *pName) -> unit_data *;
void load_contents(const char *pFileName, unit_data *unit);
void save_player_contents(unit_data *pc, int fast);

/* handler.c */
auto quest_add(unit_data *ch, const char *name, char *descr) -> struct extra_descr_data *;

void insert_in_unit_list(unit_data *u);
void remove_from_unit_list(unit_data *unit);

auto find_fptr(unit_data *u, uint16_t index) -> unit_fptr *;
auto create_fptr(unit_data *u, uint16_t index, uint16_t beat, uint16_t flags, void *data) -> unit_fptr *;
void destroy_fptr(unit_data *u, unit_fptr *f);

void stop_following(unit_data *ch);
void start_following(unit_data *ch, unit_data *leader);

void modify_bright(unit_data *unit, int bright);
void trans_set(unit_data *u);
void trans_unset(unit_data *u);

auto equipment(unit_data *ch, uint8_t pos) -> unit_data *;
auto equipment_type(unit_data *ch, int pos, uint8_t type) -> unit_data *;
void equip_char(unit_data *ch, unit_data *obj, uint8_t pos);
auto unequip_char(unit_data *ch, uint8_t pos) -> unit_data *;
auto unequip_object(unit_data *obj) -> unit_data *;
void recalc_dex_red(unit_data *ch);

auto unit_recursive(unit_data *from, unit_data *to) -> int;
auto unit_zone(const unit_data *unit) -> struct zone_type *;
auto unit_room(unit_data *unit) -> unit_data *;

/* If the transfered unit MIGHT be money, remember to pile_money() it!!!!! */
void unit_up(unit_data *unit);
void unit_from_unit(unit_data *unit);
void unit_down(unit_data *unit, unit_data *to);
void unit_to_unit(unit_data *unit, unit_data *to);

void extract_unit(unit_data *unit);

void weight_change_unit(unit_data *unit, int weight);

auto find_unit_in_list_num(int num, unit_data *list) -> unit_data *;
auto find_unit_num(int num) -> unit_data *;
