#pragma once
/* *********************************************************************** *
 * File   : limits.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : ???                                                            *
 *                                                                         *
 * Purpose: Prototypes for the Limit/Gain control module                   *
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

#include "structs.h"

auto char_can_carry_unit(unit_data *ch, unit_data *unit) -> int;
auto char_can_get_unit(unit_data *ch, unit_data *unit) -> int;

auto char_carry_n(unit_data *unit) -> int;
auto char_carry_n_limit(unit_data *ch) -> int;
auto char_can_carry_n(unit_data *ch, int n = 1) -> int;

auto char_carry_w_limit(unit_data *ch) -> int;
auto char_can_carry_w(unit_data *ch, int weight) -> int;
auto char_drag_w_limit(unit_data *ch) -> int;
auto char_can_drag_w(unit_data *ch, int weight) -> int;

auto mana_limit(unit_data *ch) -> int;
auto hit_limit_number(int) -> int;
auto hit_limit(unit_data *ch) -> int;
auto move_limit(unit_data *ch) -> int;

auto mana_gain(unit_data *ch) -> int;
auto hit_gain(unit_data *ch) -> int;
auto move_gain(unit_data *ch) -> int;

void set_title(unit_data *ch);

void advance_level(unit_data *ch);
void gain_exp(unit_data *ch, int gain);
void gain_exp_regardless(unit_data *ch, int gain);
void gain_condition(unit_data *ch, int condition, int value);
