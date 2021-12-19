#pragma once
#include "unit_affected_type.h"
auto apf_ability_adj(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_spell_adj(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_light(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_mod_char_flags(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_mod_unit_flags(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_mod_obj_flags(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_skill_adj(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_weapon_adj(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_natural_armour(unit_affected_type *af, unit_data *unit, bool set) -> bool;
auto apf_speed(unit_affected_type *af, unit_data *unit, bool set) -> bool;
