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

#ifndef _MUD_SPELLS_H
#define _MUD_SPELLS_H

#include "spelldef.h"

auto spell_perform(int spell_no, int spell_type, struct unit_data *caster, struct unit_data *medium, struct unit_data *target,
                   char *argument, char *pEffect = NULL, int bonus = 0) -> int;

#include "dil.h"

auto spell_legal_type(int spl, int type) -> bool;
auto spell_legal_target(int spl, struct unit_data *c, struct unit_data *t) -> bool;

struct spell_args
{
   class unit_data *caster, *medium, *target;
   const char      *arg;
   int              hm;
   const char      *pEffect;
};

struct spell_info_type
{
   uint8_t acttype;
   char   *tochar;
   char   *tovict;
   char   *torest;
   char   *toself;
   char   *toselfroom;

   void (*spell_pointer)(struct spell_args *sa);
   uint8_t  minimum_position; /* Position for caster               */
   int16_t  usesmana;         /* Amount of mana used by a spell    */
   int8_t   beats;            /* Heartbeats until ready for next   */
   uint16_t targets;          /* See below for use with TAR_XXX    */
   uint8_t  media;            /* found in spells/potions/...       */
   uint8_t  cast_type;        /* Resist, self check, etc?          */
   uint16_t demi_power;       /* Power used by demis (0..5)        */
   uint8_t  offensive;        /* Is this an offensive spell?       */
   uint8_t  realm;            /* Essence, Channeling, Mentalism?   */
   uint8_t  shield;           /* Shield method SHIELD_M_XXX        */

   struct diltemplate *tmpl; /* Perhaps a DIL template...         */
};

/* Spell externs */
extern struct spell_info_type  spell_info[];
extern struct requirement_type spl_requirement[];

#define SPL_MAG_REQ(spell) (spl_requirement[spell].abilities[ABIL_MAG])

#define SPL_DIV_REQ(spell) (spl_requirement[spell].abilities[ABIL_DIV])

#define SPL_POW_REQ(spell) MIN(SPL_MAG_REQ(spell), SPL_DIV_REQ(spell))

void set_spellargs(struct spell_args *sa, struct unit_data *caster, struct unit_data *medium, struct unit_data *target, const char *arg,
                   int hm);

#endif /* _MUD_SPELLS_H */
