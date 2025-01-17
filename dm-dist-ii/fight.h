#pragma once

#include "essential.h"
#include "unit_data.h"

auto raw_kill(unit_data *ch) -> unit_data *;
auto provoked_attack(unit_data *victim, unit_data *ch) -> int;
void update_pos(unit_data *victim);
void damage(unit_data *ch,
            unit_data *victim,
            unit_data *medium,
            int        damage,
            int        attackcat,
            int        weapontype,
            int        hitloc,
            int        bDisplay = static_cast<int>(TRUE));
auto pk_test(unit_data *att, unit_data *def, int message) -> int;
auto one_hit(unit_data *att, unit_data *def, int bonus, int wpn_type, int primary = static_cast<int>(TRUE)) -> int;
auto simple_one_hit(unit_data *att, unit_data *def) -> int;
auto char_dual_wield(unit_data *ch) -> int;
void melee_violence(unit_data *ch, int primary);
auto melee_bonus(unit_data  *att,
                 unit_data  *def,
                 int         hit_loc,
                 int        *pAtt_weapon_type,
                 unit_data **pAtt_weapon,
                 int        *pDef_armour_type,
                 unit_data **pDef_armour,
                 int         primary = static_cast<int>(TRUE)) -> int;
auto shield_bonus(unit_data *att, unit_data *def, unit_data **pDef_shield) -> int;
void damage_object(unit_data *ch, unit_data *obj, int dam);
