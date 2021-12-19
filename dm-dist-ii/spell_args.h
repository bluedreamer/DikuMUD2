#pragma once

#include "unit_data.h"

struct spell_args
{
   unit_data  *caster, *medium, *target;
   const char *arg;
   int         hm;
   const char *pEffect;
};
