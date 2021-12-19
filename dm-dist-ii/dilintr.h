#pragma once

#include <cstdint>

struct dilintr
{
   uint16_t flags; /* what message types to react on 0=off */
   uint8_t *lab;   /* where to perform check */
};
