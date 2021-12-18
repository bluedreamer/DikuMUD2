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

#pragma once
#include "essential.h"

void apply_affect(struct unit_data *unit);
void start_affect(struct unit_data *unit);
void stop_affect(struct unit_data *unit);
auto affected_by_spell(const struct unit_data *unit, int16_t id) -> struct unit_affected_type *;
void unlink_affect(struct unit_data *u, struct unit_affected_type *af);

void create_affect(struct unit_data *unit, struct unit_affected_type *orgaf);
void destroy_affect(struct unit_affected_type *af);
void affect_clear_unit(struct unit_data *unit);

/* These functions may not send messages - nor destroy units. Only */
/* affect a units values                                           */
struct apply_function_type
{
   const char *descr;
   bool (*func)(struct unit_affected_type *af, struct unit_data *unit, bool set);
};

/* These functions may send messages and destroy any unit. */
struct tick_function_type
{
   const char *descr;
   void (*func)(struct unit_affected_type *af, struct unit_data *unit);
};
