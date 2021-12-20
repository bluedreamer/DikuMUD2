#pragma once

#include "unit_data.h"

class spell_args
{
public:
   unit_data  *caster, *medium, *target;
   const char *arg;
   int         hm;
   const char *pEffect;
};
