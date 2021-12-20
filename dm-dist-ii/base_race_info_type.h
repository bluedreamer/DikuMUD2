#pragma once
#include "dice_type.h"

#include <cstdint>

class base_race_info_type
{
public:
   uint16_t  height;
   dice_type height_dice;

   uint16_t  weight;
   dice_type weight_dice;

   uint16_t  lifespan;
   dice_type lifespan_dice;
};
