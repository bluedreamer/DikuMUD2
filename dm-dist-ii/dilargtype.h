#pragma once

#include <cstdint>
/* The following "dilarg" structs are used only for DMC purposes to allow
   the user to enter the : dilcopy prg@zon(arg, arg, arg); instead of the
   stupid string format */

struct dilargtype
{
   uint8_t type;
   union
   {
      char   *string;
      char  **stringlist;
      int64_t num; // MS2020
   } data;
};
