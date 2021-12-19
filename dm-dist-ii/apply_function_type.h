#pragma once

#include "unit_affected_type.h"

/* These functions may not send messages - nor destroy units. Only */
/* affect a units values                                           */
struct apply_function_type
{
   const char *descr;
   bool (*func)(unit_affected_type *af, unit_data *unit, bool set);
};
extern apply_function_type apf[];
