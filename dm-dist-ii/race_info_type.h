#pragma once

#include "base_race_info_type.h"

struct race_info_type
{
   base_race_info_type male;
   base_race_info_type female;

   uint16_t  age;
   dice_type age_dice;
};
