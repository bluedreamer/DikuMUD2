#pragma once

#include "diltemplate.h"
#include "spell_args.h"

#include <cstdint>

class spell_info_type
{
public:
   uint8_t acttype;
   char   *tochar;
   char   *tovict;
   char   *torest;
   char   *toself;
   char   *toselfroom;
   void (*spell_pointer)(spell_args *sa);
   uint8_t      minimum_position; /* Position for caster               */
   int16_t      usesmana;         /* Amount of mana used by a spell    */
   int8_t       beats;            /* Heartbeats until ready for next   */
   uint16_t     targets;          /* See below for use with TAR_XXX    */
   uint8_t      media;            /* found in spells/potions/...       */
   uint8_t      cast_type;        /* Resist, self check, etc?          */
   uint16_t     demi_power;       /* Power used by demis (0..5)        */
   uint8_t      offensive;        /* Is this an offensive spell?       */
   uint8_t      realm;            /* Essence, Channeling, Mentalism?   */
   uint8_t      shield;           /* Shield method SHIELD_M_XXX        */
   diltemplate *tmpl;             /* Perhaps a DIL template...         */
};

extern spell_info_type spell_info[];
