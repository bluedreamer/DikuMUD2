#pragma once

#include "unit_data.h"

#define UVI(i) (unit_vector.units[i])
#define UVITOP (unit_vector.top)

struct unit_vector_data
{
   unit_data **units;
   int         size;
   int         top;
};

extern unit_vector_data unit_vector;
