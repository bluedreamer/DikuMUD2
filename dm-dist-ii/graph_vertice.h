#pragma once

#include "unit_data.h"

#include <cstdint>

class graph_vertice
{
public:
   unit_data     *room;      /* Pointer to direction/edge info */
   graph_vertice *parent;    /* Path info for shortest path    */
   int32_t        dist;      /* Current Distance found         */
   uint8_t        direction; /* Path direction found           */
   uint16_t       hob_pos;   /* Position in Hob                */
};
