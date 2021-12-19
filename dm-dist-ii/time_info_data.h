#pragma once
#include <cstdint>
/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct time_info_data
{
   int8_t  hours, day, month;
   int16_t year;
};
