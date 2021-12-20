#pragma once

#include "spec_arg.h"

#include <cstdint>

class unit_function_array_type
{
public:
   const char *name;
   int (*func)(spec_arg *sarg);
   int      save_w_d; /* May it be saved if it has data? True/false */
   uint16_t sfb;      /* what kind of messages should be send */
   int16_t  tick;     /* Default tick count */
};

extern unit_function_array_type unit_function_array[];
