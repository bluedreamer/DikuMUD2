#pragma once
/* *********************************************************************** *
 * File   : affect.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for the Affect system.                                  *
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

#include "essential.h"
#include "structs.h"

void apply_affect(unit_data *unit);
void start_affect(unit_data *unit);
void stop_affect(unit_data *unit);
auto affected_by_spell(const unit_data *unit, int16_t id) -> unit_affected_type *;
void unlink_affect(unit_data *u, unit_affected_type *af);

void create_affect(unit_data *unit, unit_affected_type *orgaf);
void destroy_affect(unit_affected_type *af);
void affect_clear_unit(unit_data *unit);

/* These functions may not send messages - nor destroy units. Only */
/* affect a units values                                           */
struct apply_function_type
{
   const char *descr;
   bool (*func)(unit_affected_type *af, unit_data *unit, bool set);
};

/* These functions may send messages and destroy any unit. */
struct tick_function_type
{
   const char *descr;
   void (*func)(unit_affected_type *af, unit_data *unit);
};
