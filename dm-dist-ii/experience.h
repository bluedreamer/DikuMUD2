#pragma once
#include "unit_data.h"
auto spell_bonus(unit_data *att, unit_data *medium, unit_data *def, int hit_loc, int spell_number, int *pDef_armour_type,
                 unit_data **pDef_armour) -> int;
