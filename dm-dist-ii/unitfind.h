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

#ifndef _MUD_UNITFIND_H
#define _MUD_UNITFIND_H

#include "essential.h"
#include "structs.h"

#include <memory>

#define UVI(i) (unit_vector.units[i])
#define UVITOP (unit_vector.top)

struct unit_vector_data
{
   std::vector<std::shared_ptr<unit_data>> units;
   int                         size;
   int                         top;
};

ubit1                      same_surroundings(std::shared_ptr<unit_data> u1, std::shared_ptr<unit_data> u2);
void                       scan4_unit_room(std::shared_ptr<unit_data> room, ubit8 type);
void                       scan4_unit(std::shared_ptr<unit_data> ch, ubit8 type);
std::shared_ptr<unit_data> scan4_ref(std::shared_ptr<unit_data> ch, std::shared_ptr<unit_data> fu);
int                        random_direction(std::shared_ptr<unit_data> ch);
std::shared_ptr<unit_data>
find_unit(const std::shared_ptr<unit_data> ch, char **arg, const std::shared_ptr<unit_data> list, const ubit32 bitvector);
std::shared_ptr<unit_data> find_unit_general(const std::shared_ptr<unit_data> viewer,
                                             const std::shared_ptr<unit_data> ch,
                                             char                           **arg,
                                             const std::shared_ptr<unit_data> list,
                                             const ubit32                     bitvector);
std::shared_ptr<unit_data> find_symbolic_instance(std::shared_ptr<file_index_type> fi);
std::shared_ptr<unit_data> find_symbolic(char *zone, char *name);
std::shared_ptr<unit_data>
find_symbolic_instance_ref(std::shared_ptr<unit_data> ref, std::shared_ptr<file_index_type> fi, ubit16 bitvector);
std::shared_ptr<unit_data> random_unit(std::shared_ptr<unit_data> ref, int sflags, int tflags);

#endif
