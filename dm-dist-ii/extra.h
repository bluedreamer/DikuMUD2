#pragma once
/* *********************************************************************** *
 * File   : extra.h                                   Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert                                                        *
 *                                                                         *
 * Purpose: Extra descriptions                                             *
 * Bugs   : None known.                                                    *
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
#include "essential.h"
#include "extra_descr_data.h"
#include "unit_data.h"

auto unit_find_extra_string(unit_data *ch, char *word, unit_data *list) -> const char *;
auto char_unit_find_extra(unit_data *ch, unit_data **target, char *word, unit_data *list) -> extra_descr_data *;
auto unit_find_extra(const char *word, unit_data *unit) -> extra_descr_data *;
