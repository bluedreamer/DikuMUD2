#pragma once

#include "dilprg.h"
#include "dilval.h"
class dil_func_type
{
public:
   void (*func)(dilprg *, dilval *);
};

extern dil_func_type dilfe_func[];
