#pragma once

#include "cMultiHook.h"

#define MAX_MULTI 5 /* Maximum five multiconnects */

class cMultiMaster
{
public:
   cMultiMaster();

   int        nCount;
   cMultiHook Multi[MAX_MULTI];
};

extern cMultiMaster Multi;
