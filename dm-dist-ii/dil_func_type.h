#pragma once

#include "dilprg.h"
#include "dilval.h"
struct dil_func_type
{
   void (*func)(dilprg *, dilval *);
};

extern dil_func_type dilfe_func[];
