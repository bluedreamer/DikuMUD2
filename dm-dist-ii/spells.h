#pragma once
#include "dil.h"
#include "spell_args.h"
#include "spelldef.h"
#include "unit_data.h"

auto spell_perform(int        spell_no,
                   int        spell_type,
                   unit_data *caster,
                   unit_data *medium,
                   unit_data *target,
                   char      *argument,
                   char      *pEffect = nullptr,
                   int        bonus   = 0) -> int;
auto spell_legal_type(int spl, int type) -> bool;
auto spell_legal_target(int spl, unit_data *c, unit_data *t) -> bool;

/* Spell externs */
// TODO ADRIAN Nothing seems to use requirement_type
// extern requirement_type spl_requirement[];

#define SPL_MAG_REQ(spell) (spl_requirement[spell].abilities[ABIL_MAG])

#define SPL_DIV_REQ(spell) (spl_requirement[spell].abilities[ABIL_DIV])

#define SPL_POW_REQ(spell) std::min(SPL_MAG_REQ(spell), SPL_DIV_REQ(spell))

void set_spellargs(spell_args *sa, unit_data *caster, unit_data *medium, unit_data *target, const char *arg, int hm);
