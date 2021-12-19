#pragma once
/* *********************************************************************** *
 * File   : dbfind.h                                  Part of Valhalla MUD *
 * Version: 1.05                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for the database stuff.                                 *
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

auto find_descriptor(const char *name, descriptor_data *except) -> descriptor_data *;
auto find_zone(const char *zonename) -> struct zone_type *;
auto find_file_index(const char *zonename, const char *name) -> file_index_type *;
auto find_dil_index(char *zonename, char *name) -> struct diltemplate *;
auto world_room(const char *zone, const char *name) -> unit_data *;
auto find_dil_template(const char *name) -> struct diltemplate *;

auto str_to_file_index(const char *str) -> file_index_type *;
auto pc_str_to_file_index(const unit_data *ch, const char *str) -> file_index_type *;
