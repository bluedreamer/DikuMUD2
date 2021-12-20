#pragma once
#include "unit_data.h"
class snoop_data
{
public:
   unit_data *snooping; /* Who is this char snooping        */
   unit_data *snoop_by; /* And who is snooping on this char */
};
