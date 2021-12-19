#pragma once

#include "unit_data.h"
#include <cstdint>

/* structure for securing unit pointers */
struct dilsecure
{
   unit_data *sup; /* A direct reference to the variabel! */
   uint8_t   *lab; /* address to jump to, NULL=foreach */
};
