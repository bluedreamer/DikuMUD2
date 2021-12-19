#pragma once
/* *********************************************************************** *
 * File   : spells.h                                  Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : seifert@diku.dk                                                *
 *                                                                         *
 * Purpose: Header for spell_parser.                                       *
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

#include "dil.h"
#include "spelldef.h"
#include "unit_data.h"

auto                           spell_perform(int        spell_no,
                                             int        spell_type,
                                             unit_data *caster,
                                             unit_data *medium,
                                             unit_data *target,
                                             char      *argument,
                                             char      *pEffect = nullptr,
                                             int        bonus   = 0) -> int;
auto                           spell_legal_type(int spl, int type) -> bool;
auto                           spell_legal_target(int spl, unit_data *c, unit_data *t) -> bool;

/* Spell externs */
// TODO Nothing seems to use requirement_type
extern struct requirement_type spl_requirement[];

#define SPL_MAG_REQ(spell) (spl_requirement[spell].abilities[ABIL_MAG])

#define SPL_DIV_REQ(spell) (spl_requirement[spell].abilities[ABIL_DIV])

#define SPL_POW_REQ(spell) MIN(SPL_MAG_REQ(spell), SPL_DIV_REQ(spell))

void set_spellargs(struct spell_args *sa, unit_data *caster, unit_data *medium, unit_data *target, const char *arg, int hm);
