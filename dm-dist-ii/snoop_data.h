#pragma once
#include "unit_data.h"
struct snoop_data
{
   unit_data *snooping; /* Who is this char snooping        */
   unit_data *snoop_by; /* And who is snooping on this char */
};
