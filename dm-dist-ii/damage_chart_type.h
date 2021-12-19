#pragma once

#include "damage_chart_element_type.h"
#include "values.h"

struct damage_chart_type
{
   int                       fumble; /* from 01 - fuble => fumble      */
   damage_chart_element_type element[5];
};
extern damage_chart_type spell_chart[SPL_TREE_MAX];
extern damage_chart_type weapon_chart[WPN_TREE_MAX];
