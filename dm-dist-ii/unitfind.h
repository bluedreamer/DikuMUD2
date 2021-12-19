#pragma once
/* *********************************************************************** *
 * File   : unitfind.h                                Part of Valhalla MUD *
 * Version: 2.24                                                           *
 * Author : Var.                                                           *
 *                                                                         *
 * Purpose: Basic routines for finding units.                              *
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

#include "unit_data.h"

#define UVI(i) (unit_vector.units[i])
#define UVITOP (unit_vector.top)

struct unit_vector_data
{
   unit_data **units;
   int         size;
   int         top;
};

extern struct unit_vector_data unit_vector;

auto same_surroundings(unit_data *u1, unit_data *u2) -> bool;
void scan4_unit_room(unit_data *room, uint8_t type);
void scan4_unit(unit_data *ch, uint8_t type);
auto scan4_ref(unit_data *ch, unit_data *fu) -> unit_data *;

auto random_direction(unit_data *ch) -> int;
auto find_unit(const unit_data *ch, char **arg, const unit_data *list, uint32_t bitvector) -> unit_data *;
auto find_unit_general(const unit_data *viewer, const unit_data *ch, char **arg, const unit_data *list, uint32_t bitvector) -> unit_data *;

auto find_symbolic_instance(file_index_type *fi) -> unit_data *;
auto find_symbolic(char *zone, char *name) -> unit_data *;
auto find_symbolic_instance_ref(unit_data *ref, file_index_type *fi, uint16_t bitvector) -> unit_data *;
auto random_unit(unit_data *ref, int sflags, int tflags) -> unit_data *;
