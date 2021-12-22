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

#ifndef _MUD_LIMITS_H
#define _MUD_LIMITS_H

#include "structs.h"

int char_can_carry_unit(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> unit);
int char_can_get_unit(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> unit);

int char_carry_n(std::shared_ptr<unit_data> unit);
int char_carry_n_limit(std::shared_ptr<unit_data> ch);
int char_can_carry_n(std::shared_ptr<unit_data> ch, int n = 1);

int char_carry_w_limit(std::shared_ptr<unit_data> ch);
int char_can_carry_w(std::shared_ptr<unit_data> ch, int weight);
int char_drag_w_limit(std::shared_ptr<unit_data> ch);
int char_can_drag_w(std::shared_ptr<unit_data> ch, int weight);

int mana_limit(std::shared_ptr<unit_data> ch);
int hit_limit_number(int);
int hit_limit(std::shared_ptr<unit_data> ch);
int move_limit(std::shared_ptr<unit_data> ch);

int mana_gain(std::shared_ptr<unit_data> ch);
int hit_gain(std::shared_ptr<unit_data> ch);
int move_gain(std::shared_ptr<unit_data> ch);

void set_title(std::shared_ptr<unit_data> ch);

void advance_level(std::shared_ptr<unit_data> ch);
void gain_exp(std::shared_ptr<unit_data> ch, int gain);
void gain_exp_regardless(std::shared_ptr<unit_data> ch, int gain);
void gain_condition(std::shared_ptr<unit_data> ch, int condition, int value);

#endif /* _MUD_LIMITS_H */
