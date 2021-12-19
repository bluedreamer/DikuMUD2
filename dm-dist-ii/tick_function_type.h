#pragma once

#include "unit_affected_type.h"

/* These functions may send messages and destroy any unit. */
struct tick_function_type
{
   const char *descr;
   void (*func)(unit_affected_type *af, unit_data *unit);
};
extern tick_function_type tif[];
