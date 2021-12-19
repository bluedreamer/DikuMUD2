#pragma once

#include "dilargtype.h"

#include <cstdint>
/* The following "dilarg" structs are used only for DMC purposes to allow
   the user to enter the : dilcopy prg@zon(arg, arg, arg); instead of the
   stupid string format */

struct dilargstype
{
   uint8_t    no;
   char      *name;
   dilargtype dilarg[256];
};
