#pragma once

#include <cstdint>

class dilintr
{
public:
   uint16_t flags; /* what message types to react on 0=off */
   uint8_t *lab;   /* where to perform check */
};
